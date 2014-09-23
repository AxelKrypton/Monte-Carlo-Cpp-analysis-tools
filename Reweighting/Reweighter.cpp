#include <algorithm>
#include <fstream>
#include <boost/filesystem.hpp>
#include "Reweighter.hpp"
#include "SimulationData.hpp"

static void extractNamesOfParametersFromSimulationDataIgnoringLogZ(SimulationData, std::vector<std::string>&);
static void checkCorrectnessOfReweightingConfigurationFile(SimulationDataContainer, std::vector<std::string>);
static void extractValuesOfSimulationParametersIgnoringLogZ(SimulationDataContainer, std::vector<std::vector<double> >&);
static void extractAndSetProvidedValuesOfLogZAtSimulatedPoints(SimulationDataContainer, std::vector<double> &);
static void writeNewPoints(std::vector<std::vector<double> >&, std::vector<std::vector<double> >, std::vector<double>, int=0, int=0);
static void findIflogZHasToBeCalculated(std::vector<double>, std::vector<int>&);
static double logarithmic_sum(double, double);

/*****************************************************************************************/

/*
 * The following exception in the default constructor is never thrown because in the
 * initialization list (that is not explicitly given) there is an implicit call to
 * the default constructor of the SimulationDataContainer class that throws an std::invalid_argument
 * exception.
 */
ReweighterAbstract::ReweighterAbstract() {
    throw std::invalid_argument("Reweighter needs input file for construction!");
}


void ReweighterAbstract::generalInitialization()
{
    extractNamesOfParametersFromSimulationDataIgnoringLogZ(simulationDataContainer[0], reweightingParameterNames);
	checkCorrectnessOfReweightingConfigurationFile(simulationDataContainer, reweightingParameterNames);
    extractValuesOfSimulationParametersIgnoringLogZ(simulationDataContainer, valuesOfSimulationParameters);
	if(precisionOfIterativeProcedureToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
    logZAtSimulatedPoints.resize(valuesOfSimulationParameters.size(), 0.0);
    extractAndSetProvidedValuesOfLogZAtSimulatedPoints(simulationDataContainer, logZAtSimulatedPoints);
}


ReweighterAbstract::ReweighterAbstract(std::string configurationFileIn, double precisionToCalculateLogZ)
 : configurationFile(configurationFileIn), simulationDataContainer(configurationFileIn),
   precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
{
	generalInitialization();
}


ReweighterAbstract::ReweighterAbstract(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		   std::vector<unsigned int>  newNumberOfPointsOfParametersIn, double precisionToCalculateLogZ)
 : configurationFile(configurationFileIn),  simulationDataContainer(configurationFileIn), newRangesOfParameters(newRangesOfParametersIn),
   newNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn), precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
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

void ReweighterAbstract::writeNewConfigurationFileWithLogZ(std::string newConfigFileName){
    if(newConfigFileName == "")
        newConfigFileName = configurationFile + "_new";
    std::ofstream outputFile;
    outputFile.open(newConfigFileName.c_str(), std::ofstream::app);
    if(!outputFile)
        throw std::runtime_error("Something went wrong opening the file \"" + newConfigFileName + "\"!");
    outputFile.precision(16);
    outputFile << "\n\n#===================================================================================\n\n";
    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        outputFile << simulationDataContainer[i].getDatafileName() << "\t";
        for(size_t j=0; j<reweightingParameterNames.size(); j++){
            outputFile << reweightingParameterNames[j] << " " << valuesOfSimulationParameters[i][j] << "\t";
        }
        outputFile << "logZ " << logZAtSimulatedPoints[i] << std::endl;
    }
    outputFile << "\n#===================================================================================\n\n";
    outputFile.close();
}

void ReweighterAbstract::writeNewPointsToFileWithLogZ(std::string outputFileName){
    if(boost::filesystem::exists(outputFileName))
        throw std::invalid_argument("The file \"outputFileName\" already exists! It will not be overwritten, aborting...");
    std::ofstream outputFile;
    outputFile.open(outputFileName.c_str());
    if(!outputFile)
        throw std::runtime_error("Something went wrong opening the file \"" + outputFileName + "\"!");
    outputFile.precision(16);
    for(size_t i=0; i<reweightingParameterNames.size(); i++)
        outputFile << "# " << reweightingParameterNames[i] << "\t";
    outputFile << "logZ\n";
    for(size_t i=0; i<valuesOfNewParameters.size(); i++){
        for(size_t j=0; j<valuesOfNewParameters[i].size(); j++)
            outputFile << valuesOfNewParameters[i][j] << "\t";
        outputFile << logZAtNewPoints[i] << std::endl;
    }
    outputFile.close();
}

/*****************************************************************************************/
/************************** PROTECTED OR PRIVATE METHODS *********************************/
/*****************************************************************************************/

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
std::vector<double> ReweighterAbstract::calculateLogZAtNewPoints(std::vector<std::vector<double> > valuesOfParametersAtWhichLogZIsCalculated){

	std::vector<double> outputValuesOfLogZ(valuesOfParametersAtWhichLogZIsCalculated.size());
	double logarithmOfDenominator;
	size_t numberOfReweightingParameters = reweightingParameterNames.size();
	size_t numberOfSimulationsDone = valuesOfSimulationParameters.size();
	for(size_t indexNewPoint = 0; indexNewPoint < valuesOfParametersAtWhichLogZIsCalculated.size(); indexNewPoint++){
		for(size_t indexSimulation1 = 0; indexSimulation1 < numberOfSimulationsDone; indexSimulation1++){
			size_t numberConfigurations1 = simulationDataContainer[indexSimulation1][0].getNumberOfElements();
			for(size_t indexConfiguration = 0; indexConfiguration < numberConfigurations1; indexConfiguration++){
				for(size_t indexSimulation2 = 0; indexSimulation2 < numberOfSimulationsDone; indexSimulation2++){
					size_t numberConfigurations2 = simulationDataContainer[indexSimulation2][0].getNumberOfElements();
					double exponent = 0.0;
					for(size_t indexConjugatedQuantity = 0; indexConjugatedQuantity < numberOfReweightingParameters; indexConjugatedQuantity++){
						exponent += (valuesOfParametersAtWhichLogZIsCalculated[indexNewPoint][indexConjugatedQuantity]
						                     - valuesOfSimulationParameters[indexSimulation2][indexConjugatedQuantity])
								    *simulationDataContainer[indexSimulation1][indexConjugatedQuantity][indexConfiguration];
					}
					double newTerm = log((double)numberConfigurations2) - logZAtSimulatedPoints[indexSimulation2] + exponent;
					logarithmOfDenominator = (indexSimulation2 == 0) ? newTerm : logarithmic_sum(logarithmOfDenominator, newTerm);
				}
				outputValuesOfLogZ[indexNewPoint] = (indexSimulation1 == 0 && indexConfiguration == 0) ?
												 -logarithmOfDenominator :
				                                  logarithmic_sum(outputValuesOfLogZ[indexNewPoint], -logarithmOfDenominator);
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











/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

void extractNamesOfParametersFromSimulationDataIgnoringLogZ(SimulationData simData, std::vector<std::string>& parNames){
	std::map<std::string, double> auxMap;
	auxMap = simData.getSimulationParameters();
	parNames.clear();
    for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
        if(it->first != "logZ")
            parNames.push_back(it->first);
    }
}

static void checkCorrectnessOfReweightingConfigurationFile(SimulationDataContainer simDataCont, std::vector<std::string> parNames){
	std::vector<std::string> auxParNames;
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		if(i!=0){
            extractNamesOfParametersFromSimulationDataIgnoringLogZ(simDataCont[i], auxParNames);
			if(auxParNames != parNames)
				throw std::invalid_argument("Configuration file for Reweighting not valid. Use the same parameter names in each line!");
		}
		if((int)parNames.size() != simDataCont[i].getNumberOfDataSample())
			throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not the right number of columns!");
	}
}

void extractValuesOfSimulationParametersIgnoringLogZ(SimulationDataContainer simDataCont, std::vector<std::vector<double> >& valuesOfSimPar){
	std::map<std::string, double> auxMap;
	std::vector<double> auxVector;
	valuesOfSimPar.clear();
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		auxMap = simDataCont[i].getSimulationParameters();
        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
            if(it->first != "logZ")
                auxVector.push_back(it->second);
        }
		valuesOfSimPar.push_back(auxVector);
		auxVector.clear();
	}
}

static void extractAndSetProvidedValuesOfLogZAtSimulatedPoints(SimulationDataContainer simDataCont, std::vector<double>& logZ){
    std::map<std::string, double> auxMap;
    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        auxMap = simDataCont[i].getSimulationParameters();
        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
            if(it->first == "logZ")
                logZ[i] = it->second;
        }
    }
}


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


