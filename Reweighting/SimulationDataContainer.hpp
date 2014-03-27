#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

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
	std::vector<SimulationData> simulationDataSet;

	void extractInformationFromFile(std::string fileIn,
	    std::vector<std::string>& DataFilename, std::vector<std::map<std::string, double> >& dataParameters);
};

#endif /* SIMULATIONCONTAINERDATA_HPP_ */
