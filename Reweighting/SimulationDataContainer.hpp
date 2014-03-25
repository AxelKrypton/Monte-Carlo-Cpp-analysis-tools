#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

#include <stdexcept>
#include <fstream>

#include "SimulationData.hpp"

class SimulationDataContainer
{
public:
	SimulationDataContainer();
	SimulationDataContainer(std::string configurationFile);
	int getNumberOfDatafiles();
	int getNumberOfSimulationParameters(int fileNumber);
	SimulationData& operator[](int index);

private:
	int numberOfSimulationParameters; //<-- this with map can be different from file to file (in principle)
	std::vector<SimulationData> simulationDataSet;

	void checkIndex(int index);
	//todo: implement
	void extractInformationFromFile(std::string fileIn);
	//todo: this exists also in DataSample
	void checkIfDatafileExists(std::string filename);
};

#endif /* SIMULATIONCONTAINERDATA_HPP_ */
