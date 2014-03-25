#include "SimulationDataContainer.hpp"

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
	return simulationDataSet[index];
}

int SimulationDataContainer::getNumberOfDatafiles()
{
	return simulationDataSet.size();
}

int SimulationDataContainer::getNumberOfSimulationParameters(int fileNumber)
{
	return 0;
	//return simulationDataSet[fileNumber].Getxxx;
}

void SimulationDataContainer::checkIndex(int index)
{
	if (index <0 || index >= numberOfSimulationParameters )
		throw(std::invalid_argument("index must be greater than or equal to zero and smaller then the number of datafiles!"));
}

void SimulationDataContainer::extractInformationFromFile(std::string fileIn)
{
	std::string datafileName = "dummy.data";
	std::vector<double> parametersToGetOutOfFile(5);
	for (int i = 0; i<10; i++)
	{
		simulationDataSet.push_back(SimulationData(parametersToGetOutOfFile, datafileName));
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
