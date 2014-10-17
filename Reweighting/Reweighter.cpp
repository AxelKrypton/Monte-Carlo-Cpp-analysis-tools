#include <algorithm>
#include <limits>
#include <sstream>
#include "Reweighter.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"
#include "../dataAnalysisUtilities/jackknifeAnalysis.hpp"

static void writeNewPoints(std::vector<std::vector<double> >&, std::vector<std::vector<double> >, std::vector<double>, int=0, int=0);
static void findIflogZHasToBeCalculated(std::vector<double>, std::vector<int>&);
//static void assignMeanValuesToEstimateAndError(std::vector<std::vector<double> >, std::vector<std::vector<EstimateAndError> >&);
static double logarithmic_sum(double, double);

/*****************************************************************************************/

/*
 * The following exception in the default constructor is never thrown because in the
 * initialization list (that is not explicitly given) there is an implicit call to
 * the default constructor of the SimulationDataContainer class that throws an std::invalid_argument
 * exception.
 */
ReweighterAbstract::ReweighterAbstract() {
    throw std::invalid_argument("Reweighter needs SimulationDataContainer object for construction!");
}


ReweighterAbstract::ReweighterAbstract(std::string configurationFileIn, double precisionToCalculateLogZ)
 : reweightingDataHandler(configurationFileIn), observablesAtNewPoints(),
   precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
{
	generalInitialization();
}


ReweighterAbstract::ReweighterAbstract(std::string configurationFileIn,
                                       std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                                       std::vector<unsigned int>  newNumberOfPointsOfParametersIn,
                                       double precisionToCalculateLogZ)
 : reweightingDataHandler(configurationFileIn), observablesAtNewPoints(),
   newRangesOfParameters(newRangesOfParametersIn), newNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn),
   precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
{
	generalInitialization();
	calculateNewPoints();
}


std::vector<std::vector<double> > ReweighterAbstract::getValuesOfSimulationParameters(){
	return valuesOfSimulationParameters;
}


std::vector<std::vector<double> > ReweighterAbstract::getValuesOfNewParameters(){
	return valuesOfNewParameters;
}


int ReweighterAbstract::getNumberOfNewPoints(){
	return valuesOfNewParameters.size();
}


std::vector<double> ReweighterAbstract::getLogZAtSimulatedPoints(){
	return logZAtSimulatedPoints;
}


std::vector<double> ReweighterAbstract::getLogZAtNewPoints(){
    if(newRangesOfParameters.size() == 0 || newNumberOfPointsOfParameters.size() == 0)
        throw std::logic_error("Values of logZ at new points cannot be retrieved without setting before the new points!");
	return logZAtNewPoints;
}


double ReweighterAbstract::getPrecisionToCalculateLogZ(){
	return precisionOfIterativeProcedureToCalculateLogZ;
}


void ReweighterAbstract::setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn){
	newRangesOfParameters = newRangesOfParametersIn;
	calculateNewPoints();
}


void ReweighterAbstract::setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}


void ReweighterAbstract::setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		                               std::vector<unsigned int> newNumberOfPointsOfParametersIn)
{
	newRangesOfParameters = newRangesOfParametersIn;
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}

void ReweighterAbstract::setPrecisionToCalculateLogZ(double precisionToCalculateLogZ){
	if(precisionToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
	precisionOfIterativeProcedureToCalculateLogZ = precisionToCalculateLogZ;
}

/*
 * In the following function the reweighting of the observables is performed. The error on the
 * reweighted quantities is estimated using the Jackknife approach, i.e. calculating here
 * the partial predictions leaving out one data from ALL data file (the first from all, the
 * second from all, etc.).
 *
 * NOTE: In order to reweight, the use of logarithms is highly encouraged. Hence we have to
 *       be sure that each observable is positive before taking the logarithm. This is achieved
 *       shifting them by twice the minimum, if some negative value occurs.
 */
std::vector<std::vector<EstimateAndError> > ReweighterAbstract::calculateAndGetReweightedObservables(){
    std::cout << "==========================================================\n";
    std::cout << " Starting reweighting of observables...\n";
    size_t numberOfNewPoints = valuesOfNewParameters.size();
    size_t numberOfBinsUsedToBinData = reweightingDataHandler.numberOfBinsToBeUsed;
    size_t numberOfObservables = reweightingDataHandler.numberOfObservablesToBeReweighted;
    std::vector<double> minimumOfEachObservable(numberOfObservables, std::numeric_limits<double>::max());
    prepareObservablesBeforeReweighting(minimumOfEachObservable);
//    assignMeanValuesToEstimateAndError(calculateReweightedObservableValues(), observablesAtNewPoints);

    std::valarray<std::vector<std::vector<double> > >
            jackknifeEstimators(std::vector<std::vector<double> >(numberOfNewPoints,
                                                        std::vector<double>(numberOfObservables)),
                                numberOfBinsUsedToBinData);
    std::cout << "   Calculating the Jackknife estimators... \n";
    for(size_t i=0; i<numberOfBinsUsedToBinData; i++){
        std::vector<double> logZAtSimulatedPointsLeavingOutOneEntry =
                calculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(i);
        std::vector<double> logZAtNewPointsLeavingOutOneEntry =
                calculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(i, logZAtSimulatedPointsLeavingOutOneEntry);
        jackknifeEstimators[i] =
                calculateReweightedObservableValues(true, i, &logZAtSimulatedPointsLeavingOutOneEntry,
                                                             &logZAtNewPointsLeavingOutOneEntry);
    }
    std::cout << "   ...done!\n";
    restoreObservablesAfterReweighting(minimumOfEachObservable, &jackknifeEstimators);
    for(size_t i=0; i<numberOfNewPoints; i++){
        for(size_t j=0; j<numberOfObservables; j++){
            std::valarray<double> jackknifeEstimatorsPerPointAndObs(numberOfBinsUsedToBinData);
            for(size_t k=0; k<numberOfBinsUsedToBinData; k++)
                jackknifeEstimatorsPerPointAndObs[k] = jackknifeEstimators[k][i][j];
            observablesAtNewPoints[i][j].estimate = calculateJacknifeEstimate(DataSample(jackknifeEstimatorsPerPointAndObs));
            observablesAtNewPoints[i][j].error = calculateJacknifeError(DataSample(jackknifeEstimatorsPerPointAndObs));
        }
    }
    std::cout << " ...reweighting of observables done!\n";
    std::cout << "==========================================================\n";
    return observablesAtNewPoints;
}

/*****************************************************************************************/
/************************** PROTECTED OR PRIVATE METHODS *********************************/
/*****************************************************************************************/

void ReweighterAbstract::generalInitialization()
{
    reweightingParameterNames = reweightingDataHandler.getNamesOfParametersIgnoringMetaParameters();
    valuesOfSimulationParameters = reweightingDataHandler.getValuesOfSimulationParametersIgnoringMetaParameters();
    if(precisionOfIterativeProcedureToCalculateLogZ <= 0.0)
        throw std::range_error("Precision smaller than or equal to zero is nonsense!");
    logZAtSimulatedPoints.resize(valuesOfSimulationParameters.size(), 0.0);
    reweightingDataHandler.extractAndSetProvidedValuesOfLogZAtSimulatedPoints(logZAtSimulatedPoints);
}

void ReweighterAbstract::calculateNewPoints(){
	//todo: improve! Here the easiest implementation -> new point values determined as (upper_bound-lower_bound)/num_points
	if(newRangesOfParameters.size() != newNumberOfPointsOfParameters.size() ||
       newRangesOfParameters.size() != reweightingParameterNames.size())
		throw std::invalid_argument("Size of the vectors of new point parameters incorrect!");
	std::vector<std::vector<double> > newPointValuesForSingleParameter(newRangesOfParameters.size());
	for(size_t i=0; i<newRangesOfParameters.size(); i++){
		if(newNumberOfPointsOfParameters[i] < 2)
			throw std::invalid_argument("Number of new points must be at least 2, since boundaries are included!");
		double deltaPar = fabs(newRangesOfParameters[i].first - newRangesOfParameters[i].second)/(newNumberOfPointsOfParameters[i]-1);
		for(unsigned int j=0; j<newNumberOfPointsOfParameters[i]; j++)
			newPointValuesForSingleParameter[i].push_back(std::min(newRangesOfParameters[i].first, newRangesOfParameters[i].second) + j*deltaPar);
	}
	std::vector<double> auxiliaryVector;
	valuesOfNewParameters.clear();
	writeNewPoints(valuesOfNewParameters, newPointValuesForSingleParameter, auxiliaryVector);
	logZAtNewPoints.reserve(valuesOfNewParameters.size());
    observablesAtNewPoints = std::vector<std::vector<EstimateAndError> >(valuesOfNewParameters.size(),
                             std::vector<EstimateAndError>(reweightingDataHandler.numberOfObservablesToBeReweighted, EstimateAndError()));
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
std::vector<double> ReweighterAbstract::calculateLogZAtNewPoints(std::vector<std::vector<double> > valuesOfParametersAtWhichLogZIsCalculated,
                                                                 bool useBinnedData, const int entryToBeLeftOut,
                                                                 std::vector<double>* logZAtSimulationPointToBeUsed){

    SimulationDataContainer& simDataCont = (useBinnedData) ? reweightingDataHandler.simulationBinnedDataContainer
                                                           : reweightingDataHandler.simulationRawDataContainer;
    if(entryToBeLeftOut >= simDataCont[0][0].getNumberOfElements())
        throw std::logic_error("A not existing data entry has been asked to be excluded calculating logZ!");
    if(logZAtSimulationPointToBeUsed == NULL)
        logZAtSimulationPointToBeUsed = &logZAtSimulatedPoints;

	std::vector<double> outputValuesOfLogZ(valuesOfParametersAtWhichLogZIsCalculated.size());
	double logarithmOfDenominator;
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
					double exponent = 0.0;
					for(size_t indexConjugatedQuantity = 0; indexConjugatedQuantity < numberOfReweightingParameters; indexConjugatedQuantity++){
						exponent += (valuesOfParametersAtWhichLogZIsCalculated[indexNewPoint][indexConjugatedQuantity]
						                     - valuesOfSimulationParameters[indexSimulation2][indexConjugatedQuantity])
                                    *simDataCont[indexSimulation1][indexConjugatedQuantity][indexConfiguration];
					}
                    double newTerm = log((double)numberConfigurations2) - (*logZAtSimulationPointToBeUsed)[indexSimulation2] + exponent;
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
void ReweighterAbstract::calculateAndSetLogZAtSimulatedPoints(){
    /*
     * Here we MUST initialize the values of the logZ to zero. Namely one should do something like
     *    logZAtSimulatedPoints.assign(valuesOfSimulationParameters.size(), 0.0);
     * but this is unnecessary since in the constructor we used the method resize() on the vector.
     *
     * NOTE: Since maybe in the configurationFile some values of logZ have been provided, here
     *       we have to skip those points for which logZ has not to bee calculated. Nevertheless
     *       this means quite some overhead, so we split the code into two blocks (if no value
     *       of logZ is provided and if some is given).
     *
     * TODO: Benchmark in real life if these two blocks can be merged, i.e. how long does the
     *       indices set up in the else here below.
     */
    if(logZAtSimulatedPoints == std::vector<double>(logZAtSimulatedPoints.size(), 0)){

        double residuum;
        std::vector<double> newLogZ(valuesOfSimulationParameters.size());
        do{
            newLogZ = calculateLogZAtNewPoints(valuesOfSimulationParameters);
            residuum = 0.0;
            //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
            for(size_t indexSimulations = 0; indexSimulations < valuesOfSimulationParameters.size(); indexSimulations++){
                residuum += expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indexSimulations])
                           *expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indexSimulations]);
                //here one should put eq.(8.34)
            }
            logZAtSimulatedPoints = newLogZ;
        }while(sqrt(residuum) > precisionOfIterativeProcedureToCalculateLogZ);

    }else{

        //Set up to skip some calculation on logZ
        std::vector<int> indicesOfParametersAtWhichLogZHasToBeCalculated;
        findIflogZHasToBeCalculated(logZAtSimulatedPoints, indicesOfParametersAtWhichLogZHasToBeCalculated);
        std::vector<std::vector<double> > valuesOfParametersAtWhichLogZHasToBeCalculated;
        for(size_t i=0; i<indicesOfParametersAtWhichLogZHasToBeCalculated.size(); i++)
            valuesOfParametersAtWhichLogZHasToBeCalculated.push_back(valuesOfSimulationParameters[indicesOfParametersAtWhichLogZHasToBeCalculated[i]]);
        //Real calculation
        double residuum;
        std::vector<double> newLogZ;
        do{
            newLogZ = calculateLogZAtNewPoints(valuesOfParametersAtWhichLogZHasToBeCalculated);
            residuum = 0.0;
            //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
            for(size_t indexSimulations = 0; indexSimulations < valuesOfParametersAtWhichLogZHasToBeCalculated.size(); indexSimulations++){
                residuum += expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]])
                           *expm1(newLogZ[indexSimulations] - logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]]);
                //here one should put eq.(8.34)
                logZAtSimulatedPoints[indicesOfParametersAtWhichLogZHasToBeCalculated[indexSimulations]] = newLogZ[indexSimulations];
            }
        }while(sqrt(residuum) > precisionOfIterativeProcedureToCalculateLogZ);

    }
}


void ReweighterAbstract::calculateAndSetLogZAtNewPoints(){
    logZAtNewPoints = calculateLogZAtNewPoints(valuesOfNewParameters);
}

std::vector<double> ReweighterAbstract::calculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(const int entryToBeLeftOut,
                                                                                                     std::vector<double> logZAtSimulationPointToBeUsed){
    return calculateLogZAtNewPoints(valuesOfNewParameters, true, entryToBeLeftOut, &logZAtSimulationPointToBeUsed);
}

/*
 * Here we implement a dedicated function to calculate logZ at simulated points leaving out one of the binned data.
 * We are aware that we duplicate somehouw the function calculateAndSetLogZAtSimulatedPoints, but we postpone
 * this to a future refactoring.
 *
 * TODO: Refactor this function making it part of \"calculateAndSetLogZAtSimulatedPoints\" that will have
 *       to be modified (separate setting and calculation, bool useBinnedData as parameter, ecc.)
 */
std::vector<double> ReweighterAbstract::calculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(const int entryToBeLeftOut){

    if(entryToBeLeftOut < 0 || entryToBeLeftOut >= reweightingDataHandler.numberOfBinsToBeUsed)
        throw std::out_of_range("Invalid entry to be left out in \"calculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry\" function.");
    double residuum;
    std::vector<double> newLogZ(valuesOfSimulationParameters.size());
    std::vector<double> resultLogZ(valuesOfSimulationParameters.size(), 0.0);
    do{
        newLogZ = calculateLogZAtNewPoints(valuesOfSimulationParameters, true, entryToBeLeftOut, &resultLogZ);
        residuum = 0.0;
        //todo: think if it is worth to make logZAt___Points valarray instead of vector to use valarray functionalities here.
        for(size_t indexSimulations = 0; indexSimulations < valuesOfSimulationParameters.size(); indexSimulations++){
            residuum += expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations])
                       *expm1(newLogZ[indexSimulations] - resultLogZ[indexSimulations]);
            //here one should put eq.(8.34)
        }
        resultLogZ = newLogZ;
    }while(sqrt(residuum) > precisionOfIterativeProcedureToCalculateLogZ);

    return resultLogZ;
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
 * REMARK: Here we sort of duplicate a piece of code of the function calculateLogZAtNewPoints, but
 *         it is done on the purpose to keep the calculation of logZ and the observables separated.
 * TODO: Refactor calculation of logarithmOfDenominator in an own function.
 */
std::vector<std::vector<double> > ReweighterAbstract::calculateReweightedObservableValues(bool useBinnedData,
                                                                                          const int entryToBeLeftOut,
                                                                                          std::vector<double> *logZAtSimulationPointToBeUsed,
                                                                                          std::vector<double> *logZAtNewPointsToBeUsed){

    SimulationDataContainer& simDataCont = (useBinnedData) ? reweightingDataHandler.simulationBinnedDataContainer
                                                           : reweightingDataHandler.simulationRawDataContainer;
    if(entryToBeLeftOut >= simDataCont[0][0].getNumberOfElements())
        throw std::logic_error("A not existing data entry has been asked to be excluded reweighting observables!");
    if(logZAtSimulationPointToBeUsed == NULL)
        logZAtSimulationPointToBeUsed = &logZAtSimulatedPoints;
    if(logZAtNewPointsToBeUsed == NULL)
        logZAtNewPointsToBeUsed = &logZAtNewPoints;

    size_t numberOfReweightingParameters = reweightingParameterNames.size();
    size_t numberOfSimulationsDone = valuesOfSimulationParameters.size();
    size_t numberOfNewPoints = valuesOfNewParameters.size();
    std::vector<std::vector<double> > outputValuesOfObservables(numberOfNewPoints, std::vector<double>(reweightingDataHandler.numberOfObservablesToBeReweighted));
    double logarithmOfDenominator;
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
                    double exponent = 0.0;
                    for(size_t indexConjugatedQuantity = 0; indexConjugatedQuantity < numberOfReweightingParameters; indexConjugatedQuantity++){
                        exponent += (valuesOfNewParameters[indexNewPoint][indexConjugatedQuantity]
                                    -valuesOfSimulationParameters[indexSimulation2][indexConjugatedQuantity])
                                    *simDataCont[indexSimulation1][indexConjugatedQuantity][indexConfiguration];
                    }
                    double newTerm = log((double)numberConfigurations2) - (*logZAtSimulationPointToBeUsed)[indexSimulation2] + exponent;
                    logarithmOfDenominator = (indexSimulation2 == 0) ? newTerm : logarithmic_sum(logarithmOfDenominator, newTerm);
                }
                for(int indexObservable=0; indexObservable<reweightingDataHandler.numberOfObservablesToBeReweighted; indexObservable++){
                    double newTerm = simDataCont[indexSimulation1][indexObservable+numberOfReweightingParameters][indexConfiguration] - logarithmOfDenominator;
                    outputValuesOfObservables[indexNewPoint][indexObservable] =
                            (indexSimulation1 == 0 && (indexConfiguration == 0 || firstValue)) ? newTerm :
                            logarithmic_sum(outputValuesOfObservables[indexNewPoint][indexObservable], newTerm);
                    firstValue = false;
                }
            }
        }
        for(int indexObservable=0; indexObservable<reweightingDataHandler.numberOfObservablesToBeReweighted; indexObservable++)
            outputValuesOfObservables[indexNewPoint][indexObservable] -= (*logZAtNewPointsToBeUsed)[indexNewPoint];
    }

    return outputValuesOfObservables;
}

/*
 * The preparation of the observables, as well as their restoration, has to be done also on
 * the binned data, since they are used to estimate the error (with the Jackknife method).
 * One could calculate the minimum also of the binned data, but since the goal is to have
 * positive quantities before taking the logarithm, it is enough to use the minimum of the
 * raw data that will be in general smaller or equal to the minimum of the binned data.
 */
void ReweighterAbstract::prepareObservablesBeforeReweighting(std::vector<double>& minimumOfEachObservable){
    const int numberOfReweightingParameters = (int)reweightingParameterNames.size();
    //Estimate of minimum of each observable through all files
    for(int indexSimulation=0; indexSimulation<reweightingDataHandler.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
        for(int indexObservable=numberOfReweightingParameters; indexObservable<reweightingDataHandler.simulationRawDataContainer[indexSimulation].getNumberOfDataSample(); indexObservable++){
            if(reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable].min() < minimumOfEachObservable[indexObservable-numberOfReweightingParameters])
                minimumOfEachObservable[indexObservable-numberOfReweightingParameters] = reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable].min();
        }
    }
    //Shift, if necessary, and logarithm
    for(int indexSimulation=0; indexSimulation<reweightingDataHandler.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
        for(int indexObservable=numberOfReweightingParameters; indexObservable<reweightingDataHandler.simulationRawDataContainer[indexSimulation].getNumberOfDataSample(); indexObservable++){
            if(minimumOfEachObservable[indexObservable-numberOfReweightingParameters] < 0){
                if(indexSimulation == 0)
                    std::cout << "  Observable number " << indexObservable-numberOfReweightingParameters << " shifted due to negative values!\n";
                reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable] -= 2*minimumOfEachObservable[indexObservable-numberOfReweightingParameters];
                reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][indexObservable] -= 2*minimumOfEachObservable[indexObservable-numberOfReweightingParameters];
            }
            for(int indexData=0; indexData<reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable].getNumberOfElements(); indexData++){
                if(reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable][indexData] == 0.0){
                    std::ostringstream exceptionString("Zero-value for observable ");
                    exceptionString << indexObservable << " found in datafile " << indexSimulation;
                    exceptionString << "at trajectory number " << indexData << ". Impossible to take the logarithm!";
                    throw std::runtime_error(exceptionString.str());
                }
            }
            reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable] = reweightingDataHandler.simulationRawDataContainer[indexSimulation][indexObservable].applyFunction(log);
            reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][indexObservable] = reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][indexObservable].applyFunction(log);
        }
    }
}

void ReweighterAbstract::restoreObservablesAfterReweighting(std::vector<double> minimumOfEachObservable,
                                  std::valarray<std::vector<std::vector<double> > > *jackknifePartialPred){
    const int numberOfReweightingParameters = (int)reweightingParameterNames.size();
    //Exponential and shift, if necessary, BOTH on raw/binned data AND on values at new points (Jackknife partial predicitions)
    for(int indexObservable=0; indexObservable<reweightingDataHandler.numberOfObservablesToBeReweighted; indexObservable++){
        for(int indexSimulation=0; indexSimulation<reweightingDataHandler.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
            reweightingDataHandler.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] =
                    reweightingDataHandler.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].applyFunction(exp);
            reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] =
                    reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable].applyFunction(exp);
        }
        for(size_t indexNewPoint=0; indexNewPoint<valuesOfNewParameters.size(); indexNewPoint++){
//            observablesAtNewPoints[indexNewPoint][indexObservable].estimate = exp(observablesAtNewPoints[indexNewPoint][indexObservable].estimate);
            if(jackknifePartialPred != NULL){
                for(size_t indexBin=0; indexBin<(*jackknifePartialPred).size(); indexBin++){
                    (*jackknifePartialPred)[indexBin][indexNewPoint][indexObservable] = exp((*jackknifePartialPred)[indexBin][indexNewPoint][indexObservable]);
                    if(minimumOfEachObservable[indexObservable-numberOfReweightingParameters] < 0)
                        (*jackknifePartialPred)[indexBin][indexNewPoint][indexObservable] += 2*minimumOfEachObservable[indexObservable-numberOfReweightingParameters];
                }
            }
         }
         if(minimumOfEachObservable[indexObservable] < 0){
             std::cout << "  Observable number " << indexObservable << " restored!\n";
             for(int indexSimulation=0; indexSimulation<reweightingDataHandler.simulationRawDataContainer.getNumberOfDatafiles(); indexSimulation++){
                reweightingDataHandler.simulationRawDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] += 2*minimumOfEachObservable[indexObservable];
                reweightingDataHandler.simulationBinnedDataContainer[indexSimulation][numberOfReweightingParameters+indexObservable] += 2*minimumOfEachObservable[indexObservable];
             }
         }
     }

}


SimulationDataContainer ReweighterAbstract::getSimulationDataContainer(bool raw){
    return (raw == true) ? reweightingDataHandler.simulationRawDataContainer
                         : reweightingDataHandler.simulationBinnedDataContainer;
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
static void findIflogZHasToBeCalculated(std::vector<double> logZAtSimulatedPoints,
                                        std::vector<int> & indicesOfParametersAtWhichLogZHasToBeCalculated)
{
    indicesOfParametersAtWhichLogZHasToBeCalculated.clear();
    for(size_t i=0; i<logZAtSimulatedPoints.size(); i++){
        if (logZAtSimulatedPoints[i] == 0.0)
            indicesOfParametersAtWhichLogZHasToBeCalculated.push_back(i);
    }
}


static void writeNewPoints(std::vector<std::vector<double> >&  valuesOfNewParameters,
                           std::vector<std::vector<double> > newPointValuesForSingleParameter,
                           std::vector<double> aux, int numberOfRow, int atInTheRow)
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
static double logarithmic_sum(double logx1, double logx2){
  return (logx1 >= logx2) ? logx1 + log1p(exp(logx2-logx1)) :
                             logx2 + log1p(exp(logx1-logx2));
}


