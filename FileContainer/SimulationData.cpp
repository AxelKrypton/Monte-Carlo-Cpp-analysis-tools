#include "SimulationData.hpp"

SimulationMetaData::SimulationMetaData(std::vector<double> simulationParametersIn) :
	simulationParameters(simulationParametersIn)
{
	numberOfSimulationParameters = simulationParametersIn.size();
	datafileName = "dummy.data";
}

void SimulationMetaData::checkIndex(int index)
{
	if (index <0 || index >= numberOfSimulationParameters )
		throw(std::invalid_argument("index must be greater than or equal to zero and smaller then the number of simulation points!"));
}

double& SimulationMetaData::operator[](int index)
{
	checkIndex(index);
	return simulationParameters[index];
}

SimulationData::SimulationData()
{
	throw std::invalid_argument("SimulationData needs input file for construction!");
}

SimulationData::SimulationData(std::string informationFile)
{
	checkIfDatafileExists(informationFile);
	extractInformationFromFile(informationFile);
}

//perhaps this should give the DataSample!
SimulationMetaData& SimulationData::operator[](int index)
{
	checkIndex(index);
	return simulationMetaData[index];
}

int SimulationData::getNumberOfDatafiles()
{
	return numberOfDatafiles;
}

int SimulationData::getNumberOfSimulationParameters()
{
	return numberOfSimulationParameters;
}

void SimulationData::checkIndex(int index)
{
	if (index <0 || index >= numberOfSimulationParameters )
		throw(std::invalid_argument("index must be greater than or equal to zero and smaller then the number of datafiles!"));
}

void SimulationData::extractInformationFromFile(std::string fileIn)
{
	numberOfDatafiles = 10;
	numberOfSimulationParameters = 5;
	//todo: add filename
	std::vector<double> parametersToGetOutOfFile(numberOfSimulationParameters);
	for (int i = 0; i< numberOfDatafiles; i++)
	{
		simulationMetaData.push_back(SimulationMetaData(parametersToGetOutOfFile));
	}
	//todo: move to own fct.
	for (int i = 0; i< numberOfDatafiles; i++)
	{
		std::string filename = simulationMetaData[i].datafileName;
		int replaceWithFilename = 1;
		simulationRawData.push_back(DataSample(replaceWithFilename));
	}
}

void SimulationData::checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}
