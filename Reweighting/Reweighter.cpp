#include <algorithm>
#include "Reweighter.hpp"
#include "SimulationData.hpp"

static void extractNamesOfParametersFromSimulationData(SimulationData, std::vector<std::string>&);
static void checkCorrectnessOfReweightingConfigurationFile(SimulationDataContainer, std::vector<std::string>);
static void extractValuesOfSimulationParameters(SimulationDataContainer, std::vector<std::vector<double> >&);
static bool isLogZPresentInParameters(std::vector<std::string>);
static void writeNewPoints(std::vector<std::vector<double> >&, std::vector<std::vector<double> >, std::vector<double>, int=0, int=0);

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
}


Reweighter::Reweighter(std::string configurationFileIn) : configurationFile(configurationFileIn),
															 simulationDataContainer(configurationFileIn)
{
	generalInitialization();
}


Reweighter::Reweighter(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		   std::vector<unsigned int>  newNumberOfPointsOfParametersIn)  : configurationFile(configurationFileIn),
															      simulationDataContainer(configurationFileIn),
															  	  newRangesOfParameters(newRangesOfParametersIn),
															  	  newNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn)
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


double Reweighter::getNumberOfNewPoints(){
	return valuesOfNewParameters.size();
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
	std::vector<double> aux;
	writeNewPoints(valuesOfNewParameters, newPointValuesForSingleParameter, aux);
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




