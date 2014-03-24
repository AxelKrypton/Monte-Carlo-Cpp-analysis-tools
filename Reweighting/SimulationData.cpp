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


