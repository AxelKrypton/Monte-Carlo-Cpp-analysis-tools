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

static void writeNewPoints(std::vector<std::vector<realFloat> >&, std::vector<std::vector<realFloat> >, std::vector<realFloat>, int=0, int=0);
static void findIflogZHasToBeCalculated(std::vector<realFloat>, std::vector<int>&);
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
		if(isnan(logZAtSimulatedPoints[i]))
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
		std::cout << "   Calculating the Jackknife estimators... \n";
		for(size_t i=0; i<numberOfBinsUsedToBinData; i++){
			std::vector<realFloat> logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry =
					calculateLogZAtSimulatedPoints(true, i, &smartGuessForLogZ);
			std::vector<realFloat> logZAtNewPointsUsingUncorrDataLeavingOutOneEntry =
					calculateLogZAtNewPoints(valuesOfNewParameters, true, i, &logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry);
			jackknifeEstimators[i] =
					calculateReweightedObservableValues(true, i, &logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry, &logZAtNewPointsUsingUncorrDataLeavingOutOneEntry);
			smartGuessForLogZ = logZAtSimulatedPointsUsingUncorrDataLeavingOutOneEntry;
		}
		std::cout << "   ...done!\n";
		restoreObservablesAfterReweighting(minimumOfEachObservable, &reweightedObservablesFromRawData, &jackknifeEstimators);
		extractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, jackknifeEstimators);
    }else if(momentsReweighterHelper.errorMethod == bootstrap){
    	if(momentsReweighterHelper.bootstrapNumber == NULL)
    		throw std::logic_error("In \"calculateAndGetReweightedObservables\" bootstrapNumber unset in the bootstrap case!! Aborting...");
    	std::valarray<std::vector<std::vector<realFloat> > >
    	        	            bootstrapEstimators(std::vector<std::vector<realFloat> >(numberOfNewPoints,
    	        	                                                                  std::vector<realFloat>(numberOfObservablesToBeReweighted)),
    	        	            				    *(momentsReweighterHelper.bootstrapNumber));
    	std::cout << "   Calculating the Bootstrap estimators... \n";
		// construct a trivial random generator engine from a time-based seed:
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
    	for(int iBoot=0; iBoot<(*(momentsReweighterHelper.bootstrapNumber)); iBoot++){
    		momentsReweighterHelper.simulationUncorrDataContainer =
    				momentsReweighterHelper.simulationRawDataContainer.getUncorrelatedSimulationDataSet(momentsReweighterHelper.numberOfBinsToBeUsed, bootstrap, &generator);
    		std::vector<realFloat> logZAtSimulatedPointsUsingUncorrData = calculateLogZAtSimulatedPoints(true, -1, &smartGuessForLogZ);
    		std::vector<realFloat> logZAtNewPointsUsingUncorrData = calculateLogZAtNewPoints(valuesOfNewParameters, true, -1, &logZAtSimulatedPointsUsingUncorrData);
    		bootstrapEstimators[iBoot] = calculateReweightedObservableValues(true, -1, &logZAtSimulatedPointsUsingUncorrData, &logZAtNewPointsUsingUncorrData);
    		smartGuessForLogZ = logZAtSimulatedPointsUsingUncorrData;
    	}
    	std::cout << "   ...done!\n";
    	restoreObservablesAfterReweighting(minimumOfEachObservable, &reweightedObservablesFromRawData, &bootstrapEstimators);
    	extractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, bootstrapEstimators);
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
			if(newNumberOfPointsOfParameters[i] < 2)
				throw std::invalid_argument("Number of new points must be at least 2, since boundaries are included!");
			realFloat deltaPar = fabs(newRangesOfParameters[i].first - newRangesOfParameters[i].second)/(newNumberOfPointsOfParameters[i]-1);
			for(unsigned int j=0; j<newNumberOfPointsOfParameters[i]; j++)
				newPointValuesForSingleParameter[i].push_back(std::min(newRangesOfParameters[i].first, newRangesOfParameters[i].second) + j*deltaPar);
		}
		std::vector<realFloat> auxiliaryVector;
		valuesOfNewParameters.clear();
		writeNewPoints(valuesOfNewParameters, newPointValuesForSingleParameter, auxiliaryVector);
	}

	logZAtNewPoints.reserve(valuesOfNewParameters.size());
    momentsAtNewPoints = std::vector<std::vector<Moments> >(valuesOfNewParameters.size(), std::vector<Moments>(momentsReweighterHelper.numberOfObservablesGivenAsInput, Moments()));
    momentsEstimatorsAtNewPoints = std::vector<std::vector<MomentsEstimators> >(valuesOfNewParameters.size(),
    																			std::vector<MomentsEstimators>(momentsReweighterHelper.numberOfObservablesGivenAsInput, MomentsEstimators()));
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
	realFloat logarithmOfDenominator;
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
	if(entryToBeLeftOut >= momentsReweighterHelper.numberOfBinsToBeUsed[0]) //Here negative values mean do not leave out entry, so no check is done!
		throw std::out_of_range("Invalid entry to be left out in \"calculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry\" function.");

	std::vector<realFloat> resultLogZ(valuesOfSimulationParameters.size(), 0.0);
	if(logZAtSimulationPointToStartFrom != NULL)
		resultLogZ = *logZAtSimulationPointToStartFrom;

    if (printUserInfo){
    	std::cout << "==========================================================\n";
    	std::cout << " Calculating LogZ At Simulated Points (precision = " << precisionOfIterativeProcedureToCalculateLogZ << ")..." << std::endl;
    }
    if(logZAtSimulationPointToStartFrom != &logZAtSimulatedPoints || resultLogZ == std::vector<realFloat>(logZAtSimulatedPoints.size(), 0)){
    	realFloat residuum, valueResiduumForOutput=1;
        std::vector<realFloat> newLogZ(valuesOfSimulationParameters.size());
        while(true){
            newLogZ = calculateLogZAtNewPoints(valuesOfSimulationParameters, useUncorrData, entryToBeLeftOut, &resultLogZ);
            residuum=0.0;
            //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
            for(size_t indexSimulations = 0; indexSimulations < valuesOfSimulationParameters.size(); indexSimulations++){
                residuum += expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations])
                           *expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations]);
                //here one should put eq.(8.34)
            }
			if(printUserInfo && residuum < valueResiduumForOutput){
			  std::cout << "   Residuum = " << sqrt(residuum) << std::endl;
			  valueResiduumForOutput/=10.;
			}
            if(sqrt(residuum) > precisionOfIterativeProcedureToCalculateLogZ)
            	resultLogZ = newLogZ;
            else
            	break;
	    }
        if (printUserInfo){
        	std::cout << " ...done!" << std::endl;
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
                residuum += expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]])
                           *expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]]);
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
                                                                                          std::vector<realFloat> *logZAtNewPointsToBeUsed){

    SimulationDataContainer& simDataCont = (useUncorrData) ? momentsReweighterHelper.simulationUncorrDataContainer
                                                           : momentsReweighterHelper.simulationRawDataContainer;
    if(entryToBeLeftOut >= simDataCont[0][0].getNumberOfElements())
        throw std::logic_error("A not existing data entry has been asked to be excluded reweighting observables!");
    if(logZAtSimulationPointToBeUsed == NULL)
        logZAtSimulationPointToBeUsed = &logZAtSimulatedPoints;
    if(logZAtNewPointsToBeUsed == NULL)
        logZAtNewPointsToBeUsed = &logZAtNewPoints;

    size_t numberOfReweightingParameters = reweightingParameterNames.size();
    size_t numberOfSimulationsDone = valuesOfSimulationParameters.size();
    size_t numberOfNewPoints = valuesOfNewParameters.size();
    std::vector<std::vector<realFloat> > outputValuesOfObservables(numberOfNewPoints, std::vector<realFloat>(momentsReweighterHelper.numberOfObservablesToBeReweighted));
    realFloat logarithmOfDenominator;
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
                firstValue = false;
            }
        }
        for(int indexObservable=0; indexObservable<momentsReweighterHelper.numberOfObservablesToBeReweighted; indexObservable++)
            outputValuesOfObservables[indexNewPoint][indexObservable] -= (*logZAtNewPointsToBeUsed)[indexNewPoint];
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
            }
            momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable] = momentsReweighterHelper.simulationRawDataContainer[indexSimulation][indexObservable].applyFunction(log);
            momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable] = momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][indexObservable].applyFunction(log);
        }
    }
}

void MomentsReweighterAbstract::restoreObservablesAfterReweighting(std::vector<realFloat> minimumOfEachObservable,
                                     std::vector<std::vector<realFloat> > *reweightedObservablesFromRawData,
                                     std::valarray<std::vector<std::vector<realFloat> > > *estimatorsForErrorCalculation){
    const int numberOfReweightingParameters = (int)reweightingParameterNames.size();
    //Exponential and shift, if necessary, BOTH on raw/uncorr data AND on values at new points (Jackknife partial predicitions)
    for(int indexObservable=0; indexObservable<momentsReweighterHelper.numberOfObservablesToBeReweighted; indexObservable++){
        for(int indexSimulation=0; indexSimulation<momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
            momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] =
                    momentsReweighterHelper.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].applyFunction(exp);
            momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] =
                    momentsReweighterHelper.simulationUncorrDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].applyFunction(exp);
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

}


SimulationDataContainer MomentsReweighterAbstract::getSimulationDataContainer(bool raw){
    return (raw == true) ? momentsReweighterHelper.simulationRawDataContainer
                         : momentsReweighterHelper.simulationUncorrDataContainer;
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


/*
 * Here a tool to sum quantities using logarithms is developed.
 * Basically, eq.(8.12) of Barkema is implemented.
 */
static realFloat logarithmic_sum(realFloat logx1, realFloat logx2){
  return (logx1 >= logx2) ? logx1 + log1p(exp(logx2-logx1)) :
                             logx2 + log1p(exp(logx1-logx2));
}




