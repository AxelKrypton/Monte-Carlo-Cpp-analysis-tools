#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

#include "SimulationData.hpp"

class SimulationDataContainer
{
public:
	SimulationDataContainer();
    explicit SimulationDataContainer(std::string configurationFile);
	int getNumberOfDatafiles();
	int getNumberOfSimulationParameters(int fileNumber);
    std::pair<SimulationDataContainer, std::vector<int> > getBinnedSimulationDataSetAndNumbersOfEntriesLeftOut(int numberOfBinsToBeUsed);
	SimulationData& operator[](int index);

private:
	std::vector<SimulationData> simulationDataSet;

};

#endif /* SIMULATIONCONTAINERDATA_HPP_ */
