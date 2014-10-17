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
    SimulationDataContainer insertCentralMomentsPerData(std::vector<unsigned int> whichColumns,
                                                        std::vector<unsigned int> whichCentralMoments);

protected:
	std::vector<SimulationData> simulationDataSet;

};

/*
 * In the above class the method insertCentralMomentsPerData is thought to facilitate the reweighting
 * of more complicated observables, like combinations of higher moments. We will use the
 * name CentralMomentsPerData meaning the power of the distance of such data from the mean value
 * of that kind of data. For example, including in the container the 3rd central moment per data
 * of the second column, then, per each SimulationData object in the SimulationDataContainer,
 * one column (i.e. a DataSampleBasic object) containing (O2-mean(O2))^3 will be appended
 * (where O2 is each entry of the second column and mean(O2) is the mean of the second column).
 *
 * REMARK: The number of columns ranges from ZERO on.
 */




#endif /* SIMULATIONCONTAINERDATA_HPP_ */
