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
    std::pair<SimulationDataContainer, std::vector<int> > getUncorrelatedSimulationDataSetAndNumbersOfEntriesLeftOut(int numberOfBinsToBeUsed);
    SimulationData& operator[](int index);
    SimulationDataContainer insertMomentsPerData(std::vector<unsigned int> whichColumns,
                                                        std::vector<unsigned int> whichCentralMoments);

protected:
	std::vector<SimulationData> simulationDataSet;

};

/*
 * In the above class the method insertMomentsPerData is thought to facilitate the reweighting
 * of more complicated observables, like combinations of higher moments. We will use the
 * name MomentsPerData meaning the power of such data. For example, including in the container
 * the 3rd moment per data of the second column, then, per each SimulationData object in
 * the SimulationDataContainer, one column (i.e. a DataSampleBasic object) containing
 * (O2)^3 will be appended (where O2 is each entry of the second column).
 *
 * REMARK: The number of columns ranges from ZERO on.
 */




#endif /* SIMULATIONCONTAINERDATA_HPP_ */
