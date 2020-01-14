/*
 *
 *  Copyright (c) 2014-2015,2018-2020 Alessandro Sciarra
 *  Copyright (c) 2015 Christopher Czaban
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2019 David Leemueller
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <chrono>
#include "MomentsReweighter.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"
#include "../dataAnalysisUtilities/jackknifeAnalysis.hpp"
#include "../dataAnalysisUtilities/bootstrapAnalysis.hpp"

#include <boost/math/special_functions/expm1.hpp>
#include <boost/math/special_functions/log1p.hpp>

static void writeNewPoints(std::vector<std::vector<realFloat> >&, std::vector<std::vector<realFloat> >, std::vector<realFloat>, int=0, int=0);
static void findIflogZHasToBeCalculated(std::vector<realFloat>, std::vector<int>&);
static std::vector<bool> areInputObservablesUsingMultipleColumns(std::vector<unsigned int>, int, int, int);
static realFloat logarithmic_sum(realFloat, realFloat);

/*****************************************************************************************/

MomentsReweighterAbstract::MomentsReweighterAbstract(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformationIn)
	: momentsReweighterHelper(rawDataForReweightingAndMetainformationIn)
{
	reweightingParameterNames = momentsReweighterHelper.namesOfParametersIgnoringMetaParameters;
	valuesOfSimulationParameters = momentsReweighterHelper.valuesOfSimulationParametersIgnoringMetaParameters;
	newRangesOfParameters = rawDataForReweightingAndMetainformationIn.newRangesOfParameters;
	newNumberOfPointsOfParameters = rawDataForReweightingAndMetainformationIn.newNumberOfPointsOfParameters;
	useSimulatedPointsAsNewPoints = rawDataForReweightingAndMetainformationIn.useSimulatedPointsAsNewPoints;
	precisionOfIterativeProcedureToCalculateLogZ = rawDataForReweightingAndMetainformationIn.precisionToCalculateLogZ;
	if(precisionOfIterativeProcedureToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
	logZAtSimulatedPoints = rawDataForReweightingAndMetainformationIn.valuesOfSpecifiedLogZ;
	//Set to zero not given value of logZ. TODO: This should be done in calculateLogZAtSimulatedPoints
	for(size_t i=0; i<logZAtSimulatedPoints.size(); i++){
		if((boost::math::isnan)(logZAtSimulatedPoints[i])) //parenthesis around boost::math::isnan crucial otherwise the std lib macro is called!
			logZAtSimulatedPoints[i] = 0.0;
	}
	calculateNewPoints();
}


std::vector<std::vector<realFloat> > MomentsReweighterAbstract::getValuesOfSimulationParameters(){
	return valuesOfSimulationParameters;
}


std::vector<std::vector<realFloat> > MomentsReweighterAbstract::getValuesOfNewParameters(){
	return valuesOfNewParameters;
}


int MomentsReweighterAbstract::getNumberOfNewPoints(){
	return valuesOfNewParameters.size();
}


std::vector<realFloat> MomentsReweighterAbstract::getLogZAtSimulatedPoints(){
	return logZAtSimulatedPoints;
}


std::vector<realFloat> MomentsReweighterAbstract::getLogZAtNewPoints(){
    if(newRangesOfParameters.size() == 0 || newNumberOfPointsOfParameters.size() == 0)
        throw std::logic_error("Values of logZ at new points cannot be retrieved without setting before the new points!");
    return logZAtNewPoints;
}


realFloat MomentsReweighterAbstract::getPrecisionToCalculateLogZ(){
	return precisionOfIterativeProcedureToCalculateLogZ;
}


std::vector<std::vector<Moments> > MomentsReweighterAbstract::getMomentsAtNewPoints(){
	return momentsAtNewPoints;
}


std::vector<std::vector<MomentsEstimators> > MomentsReweighterAbstract::getMomentsEstimatorsAtNewPoints(){
	return momentsEstimatorsAtNewPoints;
}

std::vector<std::vector<Histogram> > MomentsReweighterAbstract::getProbabilityDistributionsAtNewPoints(){
    return probabilityDistributionsAtNewBetas;
}

std::vector<std::vector<HistogramEstimator> > MomentsReweighterAbstract::getProbabilityDistributionEstimatorsAtNewPoints(){
    return probabilityDistributionEstimatorsAtNewBetas;
}

void MomentsReweighterAbstract::setNewRangesOfParameters(std::vector<std::pair<realFloat, realFloat> >  newRangesOfParametersIn){
	if(useSimulatedPointsAsNewPoints)
		throw std::logic_error("Trying to reset the ranges while useSimulatedPointsAsNewPoints is true, NOT ALLOWED!");
	newRangesOfParameters = newRangesOfParametersIn;
	calculateNewPoints();
}


void MomentsReweighterAbstract::setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
	if(useSimulatedPointsAsNewPoints)
		throw std::logic_error("Trying to reset the number of points while useSimulatedPointsAsNewPoints is true, NOT ALLOWED!");
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}


void MomentsReweighterAbstract::setNewParameters(std::vector<std::pair<realFloat, realFloat> >  newRangesOfParametersIn,
		                               std::vector<unsigned int> newNumberOfPointsOfParametersIn)
{
	useSimulatedPointsAsNewPoints = false;
	newRangesOfParameters = newRangesOfParametersIn;
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}

void MomentsReweighterAbstract::setPrecisionToCalculateLogZ(realFloat precisionToCalculateLogZ){
	if(precisionToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
	precisionOfIterativeProcedureToCalculateLogZ = precisionToCalculateLogZ;
}


/*****************************************************************************************/
/************************** PROTECTED OR PRIVATE METHODS *********************************/
/*****************************************************************************************/


/*
 * In the following function the reweighting of the observables is performed. The error on the
 * reweighted quantities is estimated using the Jackknife approach, i.e. calculating here
 * the partial predictions leaving out one data from ALL data file (the first from all, the
 * second from all, etc.).
 *
 * NOTE: In order to reweight, the use of logarithms is highly encouraged. Hence we have to
 *       be sure that each observable is positive before taking the logarithm. This is achieved
 *       shifting them by twice the minimum, if some negative value occurs.
 *
 * TODO: Some operations are in common to bootstrap and jackknife: extract them from if else!
 */
void MomentsReweighterAbstract::calculateAndSetReweightedMomentsAndMomentsEstimators(){
    std::cout << "==========================================================\n";
    std::cout << " Starting reweighting of observables...\n";
    size_t numberOfNewPoints = valuesOfNewParameters.size();
    size_t numberOfObservablesToBeReweighted = momentsReweighterHelper.numberOfObservablesToBeReweighted;
    size_t numberOfObservablesGivenAsInput = momentsReweighterHelper.numberOfObservablesGivenAsInput;
    std::vector<realFloat> minimumOfEachObservable(numberOfObservablesToBeReweighted, std::numeric_limits<realFloat>::max());
    prepareObservablesBeforeReweighting(minimumOfEachObservable);
    std::cout << "   Calculating the moments of observables at new points... \n";
    std::vector<std::vector<realFloat> > reweightedObservablesFromRawData = calculateReweightedObservableValues();
    std::cout << "   ...done!\n";
    std::vector<realFloat> smartGuessForLogZ = logZAtSimulatedPoints;

    //Switch between different error calculation methods
    if(momentsReweighterHelper.errorMethod == jackknife){
    	//momentsReweighterHelper.simulationUncorrDataContainer already set for jackknife
    	size_t numberOfBinsUsedToBinData = momentsReweighterHelper.numberOfBinsToBeUsed[0];
    	std::valarray<std::vector<std::vector<realFloat> > >
    	            jackknifeEstimators(std::vector<std::vector<realFloat> >(numberOfNewPoints,
    	                                                                  std::vector<realFloat>(numberOfObservablesToBeReweighted)),
    	                                numberOfBinsUsedToBinData);
        std::valarray<std::vector<std::vector<Histogram> > > //For the case reweightProbabilityDistribution=false it is a small waste of memory
                    histogramJackknifeEstimators(std::vector<std::vector<Histogram> >(numberOfNewPoints,
                                                                                std::vector<Histogram>(numberOfObservablesGivenAsInput, Histogram(momentsReweighterHelper.probabilityDistributionBinsize))),
                                                numberOfBinsUsedToBinData);
		std::cout << "   Calculating the Jackknife estimators... \n";
		for(size_t i=0; i<numberOfBinsUsedToBinData; i++){
			std::vector<realFloat> logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry =
					calculateLogZAtSimulatedPoints(true, i, &smartGuessForLogZ);
			std::vector<realFloat> logZAtNewPointsUsingUncorrDataLeavingOutOneEntry =
					calculateLogZAtNewPoints(valuesOfNewParameters, true, i, &logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry);
			jackknifeEstimators[i] =
					calculateReweightedObservableValues(true, i, &logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry, &logZAtNewPointsUsingUncorrDataLeavingOutOneEntry, &histogramJackknifeEstimators[i]);
			smartGuessForLogZ = logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry;
		}
		std::cout << "   ...done!\n";
		restoreObservablesAfterReweighting(minimumOfEachObservable, &reweightedObservablesFromRawData, &jackknifeEstimators, &histogramJackknifeEstimators);
		extractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, jackknifeEstimators);
        if(momentsReweighterHelper.reweightProbabilityDistribution)
            extractAndSetReweightedHistogramEstimators(histogramJackknifeEstimators);
    }else if(momentsReweighterHelper.errorMethod == bootstrap){
    	if(momentsReweighterHelper.bootstrapNumber == NULL)
    		throw std::logic_error("In \"calculateAndGetReweightedObservables\" bootstrapNumber unset in the bootstrap case!! Aborting...");
    	std::valarray<std::vector<std::vector<realFloat> > >
    	        	            bootstrapEstimators(std::vector<std::vector<realFloat> >(numberOfNewPoints,
    	        	                                                                  std::vector<realFloat>(numberOfObservablesToBeReweighted)),
    	        	            				    *(momentsReweighterHelper.bootstrapNumber));
    	std::valarray<std::vector<std::vector<Histogram> > > //For the case reweightProbabilityDistribution=false it is a small waste of memory
                    histogramBootstrapEstimators(std::vector<std::vector<Histogram> >(numberOfNewPoints,
                                                                                std::vector<Histogram>(numberOfObservablesGivenAsInput, Histogram(momentsReweighterHelper.probabilityDistributionBinsize))),
                                                *(momentsReweighterHelper.bootstrapNumber));
        std::cout << "   Calculating the Bootstrap estimators... \n";
		// construct a trivial random generator engine from a time-based seed:
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		int numberOfBootstrapEstForOutput=0;
    	for(int iBoot=0; iBoot<(*(momentsReweighterHelper.bootstrapNumber)); iBoot++){
    		if(iBoot >= numberOfBootstrapEstForOutput){
    			std::cout << "     - Calculating estimator number " << iBoot << "...\n";
    			numberOfBootstrapEstForOutput += *(momentsReweighterHelper.bootstrapNumber)/10;
    		}
    		momentsReweighterHelper.simulationUncorrDataContainer =
    				momentsReweighterHelper.simulationRawDataContainer.getUncorrelatedSimulationDataSet(momentsReweighterHelper.numberOfBinsToBeUsed, bootstrap, &generator);
    		std::vector<realFloat> logZAtSimulatedPointsUsingUncorrData = calculateLogZAtSimulatedPoints(true, -1, &smartGuessForLogZ);
    		std::vector<realFloat> logZAtNewPointsUsingUncorrData = calculateLogZAtNewPoints(valuesOfNewParameters, true, -1, &logZAtSimulatedPointsUsingUncorrData);
    		bootstrapEstimators[iBoot] = calculateReweightedObservableValues(true, -1, &logZAtSimulatedPointsUsingUncorrData, &logZAtNewPointsUsingUncorrData, &histogramBootstrapEstimators[iBoot]);
    		smartGuessForLogZ = logZAtSimulatedPointsUsingUncorrData;
    	}
    	std::cout << "   ...done!\n";
    	restoreObservablesAfterReweighting(minimumOfEachObservable, &reweightedObservablesFromRawData, &bootstrapEstimators, &histogramBootstrapEstimators);
    	extractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, bootstrapEstimators);
        if(momentsReweighterHelper.reweightProbabilityDistribution)
            extractAndSetReweightedHistogramEstimators(histogramBootstrapEstimators);
    }else{
    	throw std::invalid_argument("Error method for some reason unknown! Aborting...");
    }

    std::cout << " ...reweighting of observables done!\n";
    std::cout << "==========================================================\n\n";
}


void MomentsReweighterAbstract::calculateNewPoints(){

	//TODO: improve! Here the easiest implementation -> new point values determined as (upper_bound-lower_bound)/num_points
	if(useSimulatedPointsAsNewPoints){
		valuesOfNewParameters = valuesOfSimulationParameters;
	}else{
		if(newRangesOfParameters.size() != newNumberOfPointsOfParameters.size() ||
	       newRangesOfParameters.size() != reweightingParameterNames.size())
			throw std::invalid_argument("Size of the vectors of new point parameters incorrect!");
		std::vector<std::vector<realFloat> > newPointValuesForSingleParameter(newRangesOfParameters.size());
		for(size_t i=0; i<newRangesOfParameters.size(); i++){
		    if(newRangesOfParameters[i].first == newRangesOfParameters[i].second)
		        newPointValuesForSingleParameter[i].push_back(newRangesOfParameters[i].first);
		    else {
		        if(newNumberOfPointsOfParameters[i] < 2)
		            throw std::invalid_argument("Number of new points must be at least 2, since boundaries are included!");
		        realFloat deltaPar = fabs(newRangesOfParameters[i].first - newRangesOfParameters[i].second)/(newNumberOfPointsOfParameters[i]-1);
		        for(unsigned int j=0; j<newNumberOfPointsOfParameters[i]; j++)
		            newPointValuesForSingleParameter[i].push_back(std::min(newRangesOfParameters[i].first, newRangesOfParameters[i].second) + j*deltaPar);
		    }
		}
		std::vector<realFloat> auxiliaryVector;
		valuesOfNewParameters.clear();
		writeNewPoints(valuesOfNewParameters, newPointValuesForSingleParameter, auxiliaryVector);
	}

	logZAtNewPoints.reserve(valuesOfNewParameters.size());
    momentsAtNewPoints = std::vector<std::vector<Moments> >(valuesOfNewParameters.size(), std::vector<Moments>(momentsReweighterHelper.numberOfObservablesGivenAsInput, Moments()));
    momentsEstimatorsAtNewPoints = std::vector<std::vector<MomentsEstimators> >(valuesOfNewParameters.size(),
    																			std::vector<MomentsEstimators>(momentsReweighterHelper.numberOfObservablesGivenAsInput, MomentsEstimators()));
    if(momentsReweighterHelper.reweightProbabilityDistribution){
        probabilityDistributionsAtNewBetas = std::vector<std::vector<Histogram> >(valuesOfNewParameters.size(), 
                                                                                  std::vector<Histogram>(momentsReweighterHelper.numberOfObservablesGivenAsInput,Histogram(momentsReweighterHelper.probabilityDistributionBinsize)));
        probabilityDistributionEstimatorsAtNewBetas = std::vector<std::vector<HistogramEstimator> >(valuesOfNewParameters.size(), 
                                                                                  std::vector<HistogramEstimator>(momentsReweighterHelper.numberOfObservablesGivenAsInput,HistogramEstimator(momentsReweighterHelper.probabilityDistributionBinsize)));                                                                    
    }
}


/*
 * This function is to calculate the value of the partition function at a new point
 * in the parameter space, knowing it at all simulated points. Then it is nothing but
 * eq.(8.37) of Barkema for a set of new parameters (and generalized to an arbitrary
 * number of reweighting parameters).
 * Notice that it will be also used to calculate the values of the partition function
 * at the simulated points at each step of the iterative procedure, namely to implement
 * eq.(8.36).
 *
 * NOTE: Eq.(8.37) is here implemented using logarithms.
 * NOTE: We implemented this function without checking the correct structure of any
 *       variable. This is because it is not a public method and will be called only
 *       by "developers". Thus memory issue can arise if one is not careful. For
 *       instance, logZAtSimulatedPoints must have the right amount of memory reserved
 *       before calling this function.
 */
std::vector<realFloat> MomentsReweighterAbstract::calculateLogZAtNewPoints(std::vector<std::vector<realFloat> > valuesOfParametersAtWhichLogZIsCalculated,
                                                                 bool useUncorrData, const int entryToBeLeftOut,
                                                                 std::vector<realFloat>* logZAtSimulationPointToBeUsed){

    SimulationDataContainer& simDataCont = (useUncorrData) ? momentsReweighterHelper.simulationUncorrDataContainer
                                                           : momentsReweighterHelper.simulationRawDataContainer;
    if(entryToBeLeftOut >= simDataCont[0][0].getNumberOfElements())
        throw std::logic_error("A not existing data entry has been asked to be excluded calculating logZ!");
    if(logZAtSimulationPointToBeUsed == NULL)
        logZAtSimulationPointToBeUsed = &logZAtSimulatedPoints;

    std::vector<realFloat> outputValuesOfLogZ(valuesOfParametersAtWhichLogZIsCalculated.size());
	realFloat logarithmOfDenominator = std::numeric_limits<double>::quiet_NaN(); //meaningless initial value, since variable will be initialized later.
	size_t numberOfReweightingParameters = reweightingParameterNames.size();
	size_t numberOfSimulationsDone = valuesOfSimulationParameters.size();
	for(size_t indexNewPoint = 0; indexNewPoint < valuesOfParametersAtWhichLogZIsCalculated.size(); indexNewPoint++){
        bool firstValue = true;
        for(size_t indexSimulation1 = 0; indexSimulation1 < numberOfSimulationsDone; indexSimulation1++){
            size_t numberConfigurations1 = simDataCont[indexSimulation1][0].getNumberOfElements();
			for(size_t indexConfiguration = 0; indexConfiguration < numberConfigurations1; indexConfiguration++){
                if((int)indexConfiguration == entryToBeLeftOut)
                    continue;
				for(size_t indexSimulation2 = 0; indexSimulation2 < numberOfSimulationsDone; indexSimulation2++){
                    size_t numberConfigurations2 = simDataCont[indexSimulation2][0].getNumberOfElements();
                    if(entryToBeLeftOut >= 0) numberConfigurations2--; //The number of conf has to be decreased if we leave one out!
					realFloat exponent = 0.0;
					for(size_t indexConjugatedQuantity = 0; indexConjugatedQuantity < numberOfReweightingParameters; indexConjugatedQuantity++){
						exponent += (valuesOfParametersAtWhichLogZIsCalculated[indexNewPoint][indexConjugatedQuantity]
						                     - valuesOfSimulationParameters[indexSimulation2][indexConjugatedQuantity])
                                    *simDataCont[indexSimulation1][indexConjugatedQuantity][indexConfiguration];
					}
                    realFloat newTerm = log((realFloat)numberConfigurations2) - (*logZAtSimulationPointToBeUsed)[indexSimulation2] + exponent;
					logarithmOfDenominator = (indexSimulation2 == 0) ? newTerm : logarithmic_sum(logarithmOfDenominator, newTerm);
				}
                outputValuesOfLogZ[indexNewPoint] = (indexSimulation1 == 0 && (indexConfiguration == 0 || firstValue)) ?
												 -logarithmOfDenominator :
				                                  logarithmic_sum(outputValuesOfLogZ[indexNewPoint], -logarithmOfDenominator);
                firstValue = false;
			}
		}
	}

	return outputValuesOfLogZ;
}

/*
 * This function is to calculate the value of the partition function at simulated points
 * in the parameter space. It is well known that this has to be done iteratively, i.e.
 * for instance according to eq.(8.36) of Barkema (here generalized to an arbitrary
 * number of reweighting parameters).
 *
 * NOTE: Eq.(8.36) is here implemented using logarithms.
 * NOTE: To avoid problems of over/under-flow one can use also the trick suggested
 *       around eq.(8.34) in the Barkema book. Nevertheless one has to understand if
 *       it is numerically worth looking for the maximum and the minimum of Z at
 *       each iteration. Todo: So far it is not implemented
 * NOTE: We implemented this function without checking the correct structure of any
 *       variable. This is because it is not a public method and will be called only
 *       by "developers". Thus memory issue can arise if one is not careful. For
 *       instance, logZAtSimulatedPoints must have the right amount of memory reserved
 *       before calling this function.
 */
std::vector<realFloat> MomentsReweighterAbstract::calculateLogZAtSimulatedPoints(bool useUncorrData, const int entryToBeLeftOut,
        															   std::vector<realFloat>* logZAtSimulationPointToStartFrom, bool printUserInfo){
    /*
     * Here we MUST initialize the values of the logZ to zero. Namely one should do something like
     *    logZAtSimulatedPoints.assign(valuesOfSimulationParameters.size(), 0.0);
     * but this is unnecessary since in the constructor we used the method resize() on the vector.
     *
     * NOTE: Since maybe in the configurationFile some values of logZ have been provided, here
     *       we have to skip those points for which logZ has not to be calculated. Nevertheless
     *       this means quite some overhead, so we split the code into two blocks (if no value
     *       of logZ is provided and if some is given).
     *
     * TODO: Benchmark in real life if these two blocks can be merged, i.e. how long takes the
     *       indices set up in the else here below.
     */
	using std::abs;

	if(entryToBeLeftOut >= momentsReweighterHelper.numberOfBinsToBeUsed[0]) //Here negative values mean do not leave out entry, so no check is done!
		throw std::out_of_range("Invalid entry to be left out in \"calculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry\" function.");

	std::vector<realFloat> resultLogZ(valuesOfSimulationParameters.size(), 0.0);
	if(logZAtSimulationPointToStartFrom != NULL)
		resultLogZ = *logZAtSimulationPointToStartFrom;

    if (printUserInfo){
    	std::cout << "==========================================================\n";
    	std::cout << " Calculating LogZ At Simulated Points (precision = " << precisionOfIterativeProcedureToCalculateLogZ << ")..." << std::endl;
    	std::cout.precision(NUM_DECIMAL_DIGITS);
    }
    if(logZAtSimulationPointToStartFrom != &logZAtSimulatedPoints || resultLogZ == std::vector<realFloat>(logZAtSimulatedPoints.size(), 0)){
    	realFloat residuumNew, residuumOld, valueResiduumForOutput=1;
    	int iter=0;
        std::vector<realFloat> newLogZ(valuesOfSimulationParameters.size());
        while(true){
        	iter++;
            newLogZ = calculateLogZAtNewPoints(valuesOfSimulationParameters, useUncorrData, entryToBeLeftOut, &resultLogZ);
            residuumOld=residuumNew=0.0;
            //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
            for(size_t indexSimulations = 0; indexSimulations < valuesOfSimulationParameters.size(); indexSimulations++){
                residuumNew += boost::math::expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations])
                           *boost::math::expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations]);
                //here one should put eq.(8.34)
            }
			if(printUserInfo && residuumNew < valueResiduumForOutput){
			  std::cout << "   Residuum = " << sqrt(residuumNew) << std::endl;
			  valueResiduumForOutput/=10.;
			}
            if(sqrt(residuumNew) > precisionOfIterativeProcedureToCalculateLogZ){
            	resultLogZ = newLogZ;
            	if(abs(residuumOld-residuumNew) < pow(10, -NUM_DECIMAL_DIGITS))
            		throw std::runtime_error("Iterative procedure to calculate logZ got stuck after " + std::to_string(iter) + " iterations with " + boost::lexical_cast<std::string>(residuumNew));
            }
            else
            	break;
	    }
        if (printUserInfo){
        	std::cout << " ...done in " <<  iter << " iterations!" << std::endl;
        	std::cout << "==========================================================\n\n";
        }
        return resultLogZ;
    }else{
        //Set up to skip some calculation on logZ
        std::vector<int> indicesOfParametersAtWhichLogZHasToBeCalculated;
        findIflogZHasToBeCalculated(logZAtSimulatedPoints, indicesOfParametersAtWhichLogZHasToBeCalculated);
        std::vector<std::vector<realFloat> > valuesOfParametersAtWhichLogZHasToBeCalculated;
        for(size_t i=0; i<indicesOfParametersAtWhichLogZHasToBeCalculated.size(); i++)
            valuesOfParametersAtWhichLogZHasToBeCalculated.push_back(valuesOfSimulationParameters[indicesOfParametersAtWhichLogZHasToBeCalculated[i]]);
        //Real calculation
        realFloat residuum;
        std::vector<realFloat> newLogZ;
        while(true){
            newLogZ = calculateLogZAtNewPoints(valuesOfParametersAtWhichLogZHasToBeCalculated);
            residuum = 0.0;
            //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
            for(size_t indexSimulations = 0; indexSimulations < valuesOfParametersAtWhichLogZHasToBeCalculated.size(); indexSimulations++){
                residuum += boost::math::expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]])
                           *boost::math::expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]]);
                //here one should put eq.(8.34)
            }
            if(sqrt(residuum) > precisionOfIterativeProcedureToCalculateLogZ){
            	for(size_t indexSimulations = 0; indexSimulations < valuesOfParametersAtWhichLogZHasToBeCalculated.size(); indexSimulations++){
                    logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]] = newLogZ[indexSimulations];
            	}
            }else{
            	break;
            }
        }
        if (printUserInfo){
        	std::cout << " ...done!" << std::endl;
        	std::cout << "==========================================================\n\n";
        }
        return logZAtSimulatedPoints;
    }
}

void MomentsReweighterAbstract::calculateAndSetLogZAtSimulatedPoints(){
	logZAtSimulatedPoints = calculateLogZAtSimulatedPoints(false, -1, &logZAtSimulatedPoints, true);
}

void MomentsReweighterAbstract::calculateAndSetLogZAtNewPoints(){
    logZAtNewPoints = calculateLogZAtNewPoints(valuesOfNewParameters);
}


/*
 * This function just calculates the value of the observables at the new points using a given
 * SimulationDataContainer object given as parameter. Here we do not estimate any error on the
 * value, but this function will be called several times in calculateAndGetReweightedObservables
 * with different SimulationDataContainer in order to evaluate the error.
 *
 * NOTE: From ALL the data files given in the SimulationDataContainer, the entry number "entryToBeLeftOut"
 *       is discarded in the calculation of the observables. This is to apply the Jackknife method later.
 *
 * NOTE: In this function, we consider understood that, in the SimulationDataContainer used in the calculation,
 *       the observables are "ready to be reweighted". This means that within such an object the logarithms
 *       of the original values must have been stored. Hence, in case of observables that can be negative,
 *       a global shift has to be done before taking the logarithms and an analog invert shift has to
 *       be done afterwards. All this procedure is carried out in the function calculateAndGetReweightedObservables
 *       and not here to avoid to repeat it doing the Jackknife (consider that this function will be called
 *       several times).
 *
 * NOTE: Here we reweight all the column of the data container beyond the conjugated quantities.
 *       This means that also the central moments per data which have in case been added are reweighted.
 *
 * REMARK: Here we sort of duplicate a piece of code of the function calculateLogZAtNewPoints, but
 *         it is done on the purpose to keep the calculation of logZ and the observables separated.
 * TODO: Refactor calculation of logarithmOfDenominator in an own function.
 */
std::vector<std::vector<realFloat> > MomentsReweighterAbstract::calculateReweightedObservableValues(bool useUncorrData,
                                                                                          const int entryToBeLeftOut,
                                                                                          std::vector<realFloat> *logZAtSimulationPointToBeUsed,
                                                                                          std::vector<realFloat> *logZAtNewPointsToBeUsed,
                                                                                          std::vector<std::vector<Histogram> > *histoToBeFilled){

    SimulationDataContainer& simDataCont = (useUncorrData) ? momentsReweighterHelper.simulationUncorrDataContainer
                                                           : momentsReweighterHelper.simulationRawDataContainer;
    if(entryToBeLeftOut >= simDataCont[0][0].getNumberOfElements())
        throw std::logic_error("A not existing data entry has been asked to be excluded reweighting observables!");
    if(logZAtSimulationPointToBeUsed == NULL)
        logZAtSimulationPointToBeUsed = &logZAtSimulatedPoints;
    if(logZAtNewPointsToBeUsed == NULL)
        logZAtNewPointsToBeUsed = &logZAtNewPoints;
    if(histoToBeFilled == NULL)
        histoToBeFilled = &probabilityDistributionsAtNewBetas;

    size_t numberOfReweightingParameters = reweightingParameterNames.size();
    size_t numberOfSimulationsDone = valuesOfSimulationParameters.size();
    size_t numberOfNewPoints = valuesOfNewParameters.size();
    std::vector<std::vector<realFloat> > outputValuesOfObservables(numberOfNewPoints, std::vector<realFloat>(momentsReweighterHelper.numberOfObservablesToBeReweighted));
    realFloat logarithmOfDenominator = std::numeric_limits<double>::quiet_NaN(); //meaningless initial value, since variable will be initialized later.
    std::vector<int> columnsInputObservables;
    if(momentsReweighterHelper.reweightProbabilityDistribution)
        columnsInputObservables=getColumnsToBeConsideredReweightingProbabilityDistribution();
    for(size_t indexNewPoint = 0; indexNewPoint < numberOfNewPoints; indexNewPoint++){
        bool firstValue = true;
        for(size_t indexSimulation1 = 0; indexSimulation1 < numberOfSimulationsDone; indexSimulation1++){
            size_t numberConfigurations1 = simDataCont[indexSimulation1][0].getNumberOfElements();
            for(size_t indexConfiguration = 0; indexConfiguration < numberConfigurations1; indexConfiguration++){
                if((int)indexConfiguration == entryToBeLeftOut)
                    continue;
                for(size_t indexSimulation2 = 0; indexSimulation2 < numberOfSimulationsDone; indexSimulation2++){
                    size_t numberConfigurations2 = simDataCont[indexSimulation2][0].getNumberOfElements();
                    if(entryToBeLeftOut >= 0) numberConfigurations2--; //The number of conf has to be decreased if we leave one out!
                    realFloat exponent = 0.0;
                    for(size_t indexConjugatedQuantity = 0; indexConjugatedQuantity < numberOfReweightingParameters; indexConjugatedQuantity++){
                        exponent += (valuesOfNewParameters[indexNewPoint][indexConjugatedQuantity]
                                    -valuesOfSimulationParameters[indexSimulation2][indexConjugatedQuantity])
                                    *simDataCont[indexSimulation1][indexConjugatedQuantity][indexConfiguration];
                    }
                    realFloat newTerm = log((realFloat)numberConfigurations2) - (*logZAtSimulationPointToBeUsed)[indexSimulation2] + exponent;
                    logarithmOfDenominator = (indexSimulation2 == 0) ? newTerm : logarithmic_sum(logarithmOfDenominator, newTerm);
                }
                for(int indexObservable=0; indexObservable<momentsReweighterHelper.numberOfObservablesToBeReweighted; indexObservable++){
                    realFloat newTerm = simDataCont[indexSimulation1][indexObservable+numberOfReweightingParameters][indexConfiguration] - logarithmOfDenominator;
                    outputValuesOfObservables[indexNewPoint][indexObservable] =
                            (indexSimulation1 == 0 && (indexConfiguration == 0 || firstValue)) ? newTerm :
                            logarithmic_sum(outputValuesOfObservables[indexNewPoint][indexObservable], newTerm);
                }

                /* 
                * In the following the probability distributions are getting reweighted. For that only take the logarithms of the heights and
                * not of the binsizes, how one might think, since the observables are also log(simDataCont).
                * We do that because for a binsize < 1 the log(Binsize) would be negative.
                * So to still get correct results we temporarily exponentiate the observables and use them for filling the Histogram correctly.
                * 
                * Here we also deal with the logarithms of the height which is why we have to use logarithmic_sum. 
                * As a condition whether we just set the height to the newHistoTerm or have to use the sum can't be that it's just the first iteration, 
                * because here we have multiple histograms with multiple bins. So also after a few iterations they can be empty. 
                * A better condition is just checking if the bin we want to fill is empty. Checking out a bin that doesn't exist will 
                * create a new entry of the map which is empty. But that is not a problem because 
                * immediately after checking a bin it will get filled.
                */
                if(momentsReweighterHelper.reweightProbabilityDistribution){
                    for(int indexInputObservable=0; indexInputObservable<momentsReweighterHelper.numberOfObservablesGivenAsInput; indexInputObservable++){
                        realFloat newHistoTerm = -logarithmOfDenominator;
                        realFloat tempRestoredObs=exp(simDataCont[indexSimulation1][columnsInputObservables[indexInputObservable]][indexConfiguration]);
                        if((*histoToBeFilled)[indexNewPoint][indexInputObservable][tempRestoredObs]==0)
                            (*histoToBeFilled)[indexNewPoint][indexInputObservable][tempRestoredObs] = newHistoTerm;
                        else{
                            (*histoToBeFilled)[indexNewPoint][indexInputObservable][tempRestoredObs] =
                            logarithmic_sum((*histoToBeFilled)[indexNewPoint][indexInputObservable][tempRestoredObs], newHistoTerm);
                        }
                    }
                    
                }
                
                firstValue = false;
            }
        }
        for(int indexObservable=0; indexObservable<momentsReweighterHelper.numberOfObservablesToBeReweighted; indexObservable++)
            outputValuesOfObservables[indexNewPoint][indexObservable] -= (*logZAtNewPointsToBeUsed)[indexNewPoint];
        if(momentsReweighterHelper.reweightProbabilityDistribution){
            for(int indexInputObservable=0; indexInputObservable<momentsReweighterHelper.numberOfObservablesGivenAsInput; indexInputObservable++){
                (*histoToBeFilled)[indexNewPoint][indexInputObservable] -= (*logZAtNewPointsToBeUsed)[indexNewPoint];
            }
        }
    }
    return outputValuesOfObservables;
}

/*
 * The preparation of the observables, as well as their restoration, has to be done also on
 * the uncorrelated data, since they are used to estimate the error (with the Jackknife method).
 */
void MomentsReweighterAbstract::prepareObservablesBeforeReweighting(std::vector<realFloat>& minimumOfEachObservable){
    const int numberOfReweightingParameters = (int)reweightingParameterNames.size();
    //Estimate of minimum of each observable through all files
    for(int indexSimulation=0; indexSimulation<momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
        for(int indexObservable=numberOfReweightingParameters; indexObservable<momentsReweighterHelper.simulationRawDataContainer[indexSimulation].getNumberOfDataSample(); indexObservable++){
            if(momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable].min() < minimumOfEachObservable[indexObservable-numberOfReweightingParameters])
                minimumOfEachObservable[indexObservable-numberOfReweightingParameters] = momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable].min();
        }
    }
    //Shift, if necessary, and logarithm
    for(int indexSimulation=0; indexSimulation<momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
        for(int indexObservable=numberOfReweightingParameters; indexObservable<momentsReweighterHelper.simulationRawDataContainer[indexSimulation].getNumberOfDataSample(); indexObservable++){
            if(minimumOfEachObservable[indexObservable-numberOfReweightingParameters] < 0){
                if(indexSimulation == 0)
                    std::cout << "  Observable number " << indexObservable-numberOfReweightingParameters << " shifted due to negative values!\n";
                momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable] -= 2*minimumOfEachObservable[indexObservable-numberOfReweightingParameters];
                momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable] -= 2*minimumOfEachObservable[indexObservable-numberOfReweightingParameters];
            }
            for(int indexData=0; indexData<momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable].getNumberOfElements(); indexData++){
                if(momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable][indexData] == 0.0){
                    std::ostringstream exceptionString;
                    exceptionString << "Zero-value for observable ";
                    exceptionString << indexObservable << " found in datafile " << indexSimulation;
                    exceptionString << " at trajectory number " << indexData << ". Impossible to take the logarithm!";
                    throw std::runtime_error(exceptionString.str());
                }
                momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable][indexData] = log(momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable][indexData]);
            }
            for(int indexData=0; indexData<momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable].getNumberOfElements(); indexData++)
                momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable][indexData] = log(momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable][indexData]);
        }
    }
}

void MomentsReweighterAbstract::restoreObservablesAfterReweighting(std::vector<realFloat> minimumOfEachObservable,
                                     std::vector<std::vector<realFloat> > *reweightedObservablesFromRawData,
                                     std::valarray<std::vector<std::vector<realFloat> > > *estimatorsForErrorCalculation,
									 std::valarray<std::vector<std::vector<Histogram> > > *reweightedHistogramEstimators){
    const int numberOfReweightingParameters = (int)reweightingParameterNames.size();
    //Exponential and shift, if necessary, BOTH on raw/uncorr data AND on values at new points (Jackknife partial predicitions)
    for(int indexObservable=0; indexObservable<momentsReweighterHelper.numberOfObservablesToBeReweighted; indexObservable++){
        for(int indexSimulation=0; indexSimulation<momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
        	for(int indexData=0; indexData<momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].getNumberOfElements(); indexData++)
                momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable][indexData] =
                        exp(momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable][indexData]);
        	for(int indexData=0; indexData<momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].getNumberOfElements(); indexData++)
                momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable][indexData] =
                        exp(momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable][indexData]);
        }
        for(size_t indexNewPoint=0; indexNewPoint<valuesOfNewParameters.size(); indexNewPoint++){
            if(estimatorsForErrorCalculation != NULL){
                for(size_t indexBin=0; indexBin<(*estimatorsForErrorCalculation).size(); indexBin++){
                    (*estimatorsForErrorCalculation)[indexBin][indexNewPoint][indexObservable] = exp((*estimatorsForErrorCalculation)[indexBin][indexNewPoint][indexObservable]);
                    if(minimumOfEachObservable[indexObservable] < 0)
                        (*estimatorsForErrorCalculation)[indexBin][indexNewPoint][indexObservable] += 2*minimumOfEachObservable[indexObservable];
                }
            }
            if(reweightedObservablesFromRawData != NULL){
                (*reweightedObservablesFromRawData)[indexNewPoint][indexObservable] = exp((*reweightedObservablesFromRawData)[indexNewPoint][indexObservable]);
                if(minimumOfEachObservable[indexObservable] < 0)
                    (*reweightedObservablesFromRawData)[indexNewPoint][indexObservable] += 2*minimumOfEachObservable[indexObservable];
            }
         }
         if(minimumOfEachObservable[indexObservable] < 0){
             std::cout << "  Observable number " << indexObservable << " restored!\n";
             for(int indexSimulation=0; indexSimulation<momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
                momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] += 2*minimumOfEachObservable[indexObservable];
                momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] += 2*minimumOfEachObservable[indexObservable];
             }
         }
     }

    /*
     * Restoring Histograms by exponentiating the heights and also - if nessecary - reshifting the whole distribution
     * ATTENTION: Here we need to only consider columns of first moments of observables, that are the only ones for
     *            which histograms are created. Therefore we create the columnsInputObservables vector, which is filled
     *            by getColumnsToBeConsideredReweightingProbabilityDistribution, which in turn considers the columns with
     *            the conjugated quantities. However, minimumOfEachObservable refers only to observables and that is why
     *            here we need to subtract reweightingParameterNames.size().
     */

    std::vector<int> columnsInputObservables;
    if(momentsReweighterHelper.reweightProbabilityDistribution){
        columnsInputObservables=getColumnsToBeConsideredReweightingProbabilityDistribution();
        for(size_t indexNewPoint=0; indexNewPoint<valuesOfNewParameters.size(); indexNewPoint++){
            for(int indexInputObservable=0; indexInputObservable<momentsReweighterHelper.numberOfObservablesGivenAsInput; indexInputObservable++){
                    probabilityDistributionsAtNewBetas[indexNewPoint][indexInputObservable].exponentiateHeights();
                    if(minimumOfEachObservable[columnsInputObservables[indexInputObservable]-reweightingParameterNames.size()] < 0)
                        probabilityDistributionsAtNewBetas[indexNewPoint][indexInputObservable].shift(2*minimumOfEachObservable[columnsInputObservables[indexInputObservable]-reweightingParameterNames.size()]);
                    probabilityDistributionsAtNewBetas[indexNewPoint][indexInputObservable].normalize();
            }
        }

        if(reweightedHistogramEstimators != NULL){
            for(size_t indexBin=0; indexBin<(*reweightedHistogramEstimators).size(); indexBin++){
                for(size_t indexNewPoint=0; indexNewPoint<valuesOfNewParameters.size(); indexNewPoint++){
                    for(int indexInputObservable=0; indexInputObservable<momentsReweighterHelper.numberOfObservablesGivenAsInput; indexInputObservable++){
                        (*reweightedHistogramEstimators)[indexBin][indexNewPoint][indexInputObservable].exponentiateHeights();
                        if(minimumOfEachObservable[columnsInputObservables[indexInputObservable]-reweightingParameterNames.size()] < 0)
                            (*reweightedHistogramEstimators)[indexBin][indexNewPoint][indexInputObservable].shift(2*minimumOfEachObservable[columnsInputObservables[indexInputObservable]-reweightingParameterNames.size()]);
                        (*reweightedHistogramEstimators)[indexBin][indexNewPoint][indexInputObservable].normalize();
                    }
                }
            }
        }
    }
}


SimulationDataContainer MomentsReweighterAbstract::getSimulationDataContainer(bool raw){
    return (raw == true) ? momentsReweighterHelper.simulationRawDataContainer
                         : momentsReweighterHelper.simulationUncorrDataContainer;
}

std::vector<int> MomentsReweighterAbstract::getColumnsToBeConsideredReweightingProbabilityDistribution(){
    std::vector<int> columnsToBeConsideredReweightingProbabilityDistribution;
    int numberOfReweightingParameters=reweightingParameterNames.size();
    std::vector<bool> isObservableUsingMultipleColumns=areInputObservablesUsingMultipleColumns(momentsReweighterHelper.columnsToBeReweightedUsingMultipleColumns,
                                                                                               momentsReweighterHelper.maximumMomentNeededOverall,
                                                                                               momentsReweighterHelper.numberOfObservablesGivenAsInput,
                                                                                               numberOfReweightingParameters);
    /*
     * Here we want to identify which "columns" indices in the SimulationDataContainer refer to first moments
     * of observables, which are then those to be used to reweight the probability distributions. Since the
     * first columns are dedicated to the conjugated quantities to the reweighting parameters, we need to do
     * a hard shift of the valid columns, to be then able to simply access the SimulationDataContainer.
     */
    int rightCol=0;
    columnsToBeConsideredReweightingProbabilityDistribution.push_back(rightCol+numberOfReweightingParameters);
    for(int i=1; i<momentsReweighterHelper.numberOfObservablesGivenAsInput; i++)
    {
        if(isObservableUsingMultipleColumns.at(i-1)){
            rightCol+=momentsReweighterHelper.maximumMomentNeededOverall-1 + momentsReweighterHelper.momentsToBeReweighted.size();
        }else{
            rightCol+=momentsReweighterHelper.momentsToBeReweighted.size();
        }
        columnsToBeConsideredReweightingProbabilityDistribution.push_back(rightCol+numberOfReweightingParameters);
    }
    return columnsToBeConsideredReweightingProbabilityDistribution;
}

std::vector<std::vector<Histogram> > MomentsReweighterAbstract::getReweightedProbabilityDistributions(){
    return probabilityDistributionsAtNewBetas;
}

std::vector<std::vector<HistogramEstimator> > MomentsReweighterAbstract::getReweightedProbabilityDistributionEstimators(){
    return probabilityDistributionEstimatorsAtNewBetas;
}

void MomentsReweighterAbstract::extractAndSetReweightedMomentsAndMomentsEstimators(const std::vector<std::vector<realFloat> >& reweightedObservablesFromRawData,
																				   const std::valarray<std::vector<std::vector<realFloat> > >& estimatorsForErrorsCalculation)
{
	const size_t numberOfObservablesGivenAsInput = momentsReweighterHelper.numberOfObservablesGivenAsInput;
	const size_t numberOfNeededMoments = momentsReweighterHelper.momentsToBeReweighted.size();
	for(size_t i=0; i<valuesOfNewParameters.size(); i++){
		int numberOfLastColumnThatHasBeenExtracted = 0;
		for(size_t j=0; j<numberOfObservablesGivenAsInput; j++){
			for(size_t k=0; k<numberOfNeededMoments; k++){
				if(momentsReweighterHelper.momentsToBeReweighted[k] == 1
				   && find(momentsReweighterHelper.columnsToBeReweightedUsingMultipleColumns.begin(),
						   momentsReweighterHelper.columnsToBeReweightedUsingMultipleColumns.end(), j+reweightingParameterNames.size())
						!= momentsReweighterHelper.columnsToBeReweightedUsingMultipleColumns.end()){
					//if momentsToBeReweighted[k]==1 insert as many columns as the maximum moment needed says!
					for(unsigned int h=0; h<momentsReweighterHelper.maximumMomentNeededOverall; h++)
						momentsAtNewPoints[i][j].insert(1, reweightedObservablesFromRawData[i][numberOfLastColumnThatHasBeenExtracted+h]);
					//The estimators are in the valarray in estimatorsForErrorsCalculation, that is the outermost index => temporary object needed
					std::valarray<realFloat> auxiliaryArray(estimatorsForErrorsCalculation.size());
					for(unsigned int h=0; h<momentsReweighterHelper.maximumMomentNeededOverall; h++){
						for(size_t l=0; l<estimatorsForErrorsCalculation.size(); l++)
							auxiliaryArray[l] = estimatorsForErrorsCalculation[l][i][numberOfLastColumnThatHasBeenExtracted+h];
						momentsEstimatorsAtNewPoints[i][j].insert(momentsReweighterHelper.momentsToBeReweighted[k], DataSample(auxiliaryArray));
					}
					numberOfLastColumnThatHasBeenExtracted += momentsReweighterHelper.maximumMomentNeededOverall;
				}else{
					momentsAtNewPoints[i][j].insert(momentsReweighterHelper.momentsToBeReweighted[k], reweightedObservablesFromRawData[i][numberOfLastColumnThatHasBeenExtracted]);
					std::valarray<realFloat> auxiliaryArray(estimatorsForErrorsCalculation.size());
					for(size_t h=0; h<estimatorsForErrorsCalculation.size(); h++)
						auxiliaryArray[h] = estimatorsForErrorsCalculation[h][i][numberOfLastColumnThatHasBeenExtracted];
					momentsEstimatorsAtNewPoints[i][j].insert(momentsReweighterHelper.momentsToBeReweighted[k], DataSample(auxiliaryArray));
					numberOfLastColumnThatHasBeenExtracted++;
				}
			}
		}
	}
}

void MomentsReweighterAbstract::extractAndSetReweightedHistogramEstimators(const std::valarray<std::vector<std::vector<Histogram> > >& histogramEstimatorsForErrorCalculation){ 
    const size_t numberOfNewPoints=valuesOfNewParameters.size();
    const size_t numberOfObservablesGivenAsInput=momentsReweighterHelper.numberOfObservablesGivenAsInput;
    size_t numberOfHeightsPerBins=histogramEstimatorsForErrorCalculation.size();
    for(size_t indexNewPoint=0; indexNewPoint<numberOfNewPoints; indexNewPoint++){
        for(size_t indexInputObservable=0; indexInputObservable<numberOfObservablesGivenAsInput; indexInputObservable++){  
            std::vector<double> middleOfBins=probabilityDistributionsAtNewBetas[indexNewPoint][indexInputObservable].getMiddleOfBins();
            for(double middleOfBin : middleOfBins){
                std::vector<double> heightsOfOneBin(numberOfHeightsPerBins);
                for(size_t indexEstimator=0; indexEstimator<numberOfHeightsPerBins; indexEstimator++){
                    double height=histogramEstimatorsForErrorCalculation[indexEstimator][indexNewPoint][indexInputObservable].getHeightOfSpecificBin(middleOfBin);
                    heightsOfOneBin[indexEstimator]=height;
                }
                probabilityDistributionEstimatorsAtNewBetas[indexNewPoint][indexInputObservable].insert(middleOfBin,heightsOfOneBin);
            }
        }
    }
}

/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

/*
 * In the following function, we decide to calculate logZ at those simulated points for which in
 * the constructor logZ has been not set to a non-zero value. This is ok, since the method 
 * calculateAndSetLogZAtSimulatedPoints should be called only in the constructor after
 * having set the available logZ values. In principle, it should be equivalent to select here
 * the points looking inside the parameters in simulationDataContainer and selecting those
 * for which logZ is not present. Actually, there is a subtle difference. Suppose that for some
 * reason calculateAndSetLogZAtSimulatedPoints is called after the constructor somewhere. Then
 * the values of logZ would have been already calculated and, since valuesOfSimulationParameters
 * are (must be) still the same, it makes sense not to calculate logZ again (as it would be if we
 * just look into simulationDataContainer parameters).
 */
static void findIflogZHasToBeCalculated(std::vector<realFloat> logZAtSimulatedPoints,
                                        std::vector<int> & indicesOfParametersAtWhichLogZHasToBeCalculated)
{
    indicesOfParametersAtWhichLogZHasToBeCalculated.clear();
    for(size_t i=0; i<logZAtSimulatedPoints.size(); i++){
        if (logZAtSimulatedPoints[i] == 0.0)
            indicesOfParametersAtWhichLogZHasToBeCalculated.push_back(i);
    }
}


static void writeNewPoints(std::vector<std::vector<realFloat> >&  valuesOfNewParameters,
                           std::vector<std::vector<realFloat> > newPointValuesForSingleParameter,
                           std::vector<realFloat> aux, int numberOfRow, int atInTheRow)
{
	if(aux.size() == newPointValuesForSingleParameter.size()){
		valuesOfNewParameters.push_back(aux);
		return;
	}

	for(size_t i=atInTheRow; i<newPointValuesForSingleParameter[numberOfRow].size(); i++){
		aux.push_back(newPointValuesForSingleParameter[numberOfRow][i]);
		writeNewPoints(valuesOfNewParameters, newPointValuesForSingleParameter, aux, numberOfRow+1, atInTheRow);
		aux.pop_back();
	}
}

static std::vector<bool> areInputObservablesUsingMultipleColumns(std::vector<unsigned int> inputObservablesUsingMultipleColumns,
                                                                 int maxMomentNeedeOverall, int numberOfObservablesGivenAsInput,
                                                                 int numberOfReweightingParameters){
    std::vector<bool> checkIfObservablesUseMultipleColumns;
    if(inputObservablesUsingMultipleColumns.empty()){
        for(int i=0; i<numberOfObservablesGivenAsInput; i++)
            checkIfObservablesUseMultipleColumns.push_back(false);
    } else {
        for(size_t i=0; i<inputObservablesUsingMultipleColumns.size(); i++)
            inputObservablesUsingMultipleColumns.at(i)-=numberOfReweightingParameters;
        for(unsigned int k=0; k<inputObservablesUsingMultipleColumns.at(0); k++)
            checkIfObservablesUseMultipleColumns.push_back(false);
        checkIfObservablesUseMultipleColumns.push_back(true);
        for(size_t k=1; k<inputObservablesUsingMultipleColumns.size(); k++){
            int obsBetweenObsUsingMultiCols=inputObservablesUsingMultipleColumns.at(k)-inputObservablesUsingMultipleColumns.at(k-1)-maxMomentNeedeOverall;
            for(int i=0; i<obsBetweenObsUsingMultiCols; i++)
                checkIfObservablesUseMultipleColumns.push_back(false);
            checkIfObservablesUseMultipleColumns.push_back(true);
        }
    }
    if((int)checkIfObservablesUseMultipleColumns.size()<numberOfObservablesGivenAsInput){
        for(size_t i=0; i< numberOfObservablesGivenAsInput-checkIfObservablesUseMultipleColumns.size(); i++)
            checkIfObservablesUseMultipleColumns.push_back(false);
    }
    return checkIfObservablesUseMultipleColumns;
}

/*
 * Here a tool to sum quantities using logarithms is developed.
 * Basically, eq.(8.12) of Barkema is implemented.
 */
static realFloat logarithmic_sum(realFloat logx1, realFloat logx2){
  return (logx1 >= logx2) ? logx1 + boost::math::log1p(exp(logx2-logx1)) :
                             logx2 + boost::math::log1p(exp(logx1-logx2));
}


