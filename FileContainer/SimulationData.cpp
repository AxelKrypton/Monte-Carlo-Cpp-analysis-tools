#include "SimulationData.hpp"

SimulationData::SimulationData(std::vector<double> simulationParametersIn, std::string filenameIn) :
	simulationParameters(simulationParametersIn), datafileName(filenameIn)
{
	numberOfSimulationParameters = simulationParametersIn.size();
	//todo: move to own fct.
	int replaceWithFilename = 1;
	for (int i = 0; i< numberOfSimulationParameters; i++)
	{
		//todo: add column to constructor
		// similar to:
		// int column = i+1;
		simulationRawData.push_back(DataSample(replaceWithFilename));
	}

}

void SimulationData::checkIndex(int index)
{
	if (index < 0 || index >= numberOfSimulationParameters )
		throw(std::invalid_argument("index must be greater than or equal to zero and smaller then the number of simulation points!"));
}

//todo: this should return the DataSample!
double& SimulationData::operator[](int index)
{
	checkIndex(index);
	return simulationParameters[index];
}

SimulationDataContainer::SimulationDataContainer()
{
	throw std::invalid_argument("SimulationData needs input file for construction!");
}

SimulationDataContainer::SimulationDataContainer(std::string informationFile)
{
	checkIfDatafileExists(informationFile);
	extractInformationFromFile(informationFile);
}

SimulationData& SimulationDataContainer::operator[](int index)
{
	checkIndex(index);
	return simulationData[index];
}

int SimulationDataContainer::getNumberOfDatafiles()
{
	return numberOfDatafiles;
}

int SimulationDataContainer::getNumberOfSimulationParameters()
{
	return numberOfSimulationParameters;
}

void SimulationDataContainer::checkIndex(int index)
{
	if (index <0 || index >= numberOfSimulationParameters )
		throw(std::invalid_argument("index must be greater than or equal to zero and smaller then the number of datafiles!"));
}

void SimulationDataContainer::extractInformationFromFile(std::string fileIn)
{
	numberOfDatafiles = 10;
	numberOfSimulationParameters = 5;
	std::string datafileName = "dummy.data";
	std::vector<double> parametersToGetOutOfFile(numberOfSimulationParameters);
	for (int i = 0; i< numberOfDatafiles; i++)
	{
		simulationData.push_back(SimulationData(parametersToGetOutOfFile, datafileName));
	}
}

void SimulationDataContainer::checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}
