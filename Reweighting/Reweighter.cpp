#include <algorithm>
#include "Reweighter.hpp"
#include "SimulationData.hpp"

static void extractNamesOfParametersFromSimulationData(SimulationData, std::vector<std::string>&);
static void checkCorrectnessOfReweightingConfigurationFile(SimulationDataContainer, std::vector<std::string>);
static void extractValuesOfSimulationParameters(SimulationDataContainer, std::vector<std::vector<double> >&);
static bool isLogZPresentInParameters(std::vector<std::string>);
static void writeNewPoints(std::vector<std::vector<double> >&, std::vector<std::vector<double> >, std::vector<double>, int=0, int=0);
static double logarithmic_sum(double, double);

/*****************************************************************************************/

Reweighter::Reweighter() {
	throw std::invalid_argument("Reweighter needs input file for construction!");
}


void Reweighter::generalInitialization()
{
	extractNamesOfParametersFromSimulationData(simulationDataContainer[0], reweightingParameterNames);
	checkCorrectnessOfReweightingConfigurationFile(simulationDataContainer, reweightingParameterNames);
	extractValuesOfSimulationParameters(simulationDataContainer, valuesOfSimulationParameters);
	if(isLogZPresentInParameters(reweightingParameterNames))
		throw std::logic_error("So far logZ cannot be passed as parameter in the configuration file!");
	if(precisionOfIterativeProcedureToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
	logZAtSimulatedPoints.reserve(valuesOfSimulationParameters.size());
}


Reweighter::Reweighter(std::string configurationFileIn, double precisionToCalculateLogZ)
 : configurationFile(configurationFileIn), simulationDataContainer(configurationFileIn),
   precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
{
	generalInitialization();
}


Reweighter::Reweighter(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		   std::vector<unsigned int>  newNumberOfPointsOfParametersIn, double precisionToCalculateLogZ)
 : configurationFile(configurationFileIn),  simulationDataContainer(configurationFileIn), newRangesOfParameters(newRangesOfParametersIn),
   newNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn), precisionOfIterativeProcedureToCalculateLogZ(precisionToCalculateLogZ)
{
	generalInitialization();
	calculateNewPoints();
}


std::vector<std::vector<double> > Reweighter::getValuesOfSimulationParameters(){
	return valuesOfSimulationParameters;
}


std::vector<std::vector<double> > Reweighter::getValuesOfNewParameters(){
	return valuesOfNewParameters;
}


int Reweighter::getNumberOfNewPoints(){
	return valuesOfNewParameters.size();
}


std::vector<double> Reweighter::getLogZAtSimulatedPoints(){
	return logZAtSimulatedPoints;
}


std::vector<double> Reweighter::getLogZAtNewPoints(){
	return logZAtNewPoints;
}


double Reweighter::getPrecisionToCalculateLogZ(){
	return precisionOfIterativeProcedureToCalculateLogZ;
}


void Reweighter::setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn){
	newRangesOfParameters = newRangesOfParametersIn;
	calculateNewPoints();
}


void Reweighter::setNewNumberOfPoints(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}


void Reweighter::setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		                               std::vector<unsigned int> newNumberOfPointsOfParametersIn)
{
	newRangesOfParameters = newRangesOfParametersIn;
	newNumberOfPointsOfParameters = newNumberOfPointsOfParametersIn;
	calculateNewPoints();
}

void Reweighter::setPrecisionToCalculateLogZ(double precisionToCalculateLogZ){
	if(precisionToCalculateLogZ <= 0.0)
		throw std::range_error("Precision smaller than or equal to zero is nonsense!");
	precisionOfIterativeProcedureToCalculateLogZ = precisionToCalculateLogZ;
}


void Reweighter::calculateNewPoints(){
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

/*=======================================================================================*/

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
std::vector<double> Reweighter::calculateLogZAtNewPoints(std::vector<std::vector<double> > valuesOfParametersAtWhichLogZIsCalculated){

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
void Reweighter::calculateLogZAtSimulatedPoints(){



	logZAtSimulatedPoints.assign(valuesOfSimulationParameters.size(), 0.0);
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

}













/*****************************************************************************************/

void extractNamesOfParametersFromSimulationData(SimulationData simData, std::vector<std::string>& parNames){
	std::map<std::string, double> auxMap;
	auxMap = simData.getSimulationParameters();
	parNames.clear();
	for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++)
		parNames.push_back(it->first);
}

static void checkCorrectnessOfReweightingConfigurationFile(SimulationDataContainer simDataCont, std::vector<std::string> parNames){
	std::vector<std::string> auxParNames;
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		if(i!=0){
			extractNamesOfParametersFromSimulationData(simDataCont[i], auxParNames);
			if(auxParNames != parNames)
				throw std::invalid_argument("Configuration file for Reweighting not valid. Use the same parameter names in each line!");
		}
		if((int)parNames.size() != simDataCont[i].getNumberOfDataSample())
			throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not the right number of columns!");
	}
}

void extractValuesOfSimulationParameters(SimulationDataContainer simDataCont, std::vector<std::vector<double> >& valuesOfSimPar){
	std::map<std::string, double> auxMap;
	std::vector<double> auxVector;
	valuesOfSimPar.clear();
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		auxMap = simDataCont[i].getSimulationParameters();
		for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++)
			auxVector.push_back(it->second);
		valuesOfSimPar.push_back(auxVector);
		auxVector.clear();
	}
}

static bool isLogZPresentInParameters(std::vector<std::string> rewPar){
	return std::find(rewPar.begin(), rewPar.end(), "logZ") != rewPar.end();
}

static void writeNewPoints(std::vector<std::vector<double> >&  valuesOfNewParameters,
		std::vector<std::vector<double> > newPointValuesForSingleParameter, std::vector<double> aux, int numberOfRow, int atInTheRow)
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


