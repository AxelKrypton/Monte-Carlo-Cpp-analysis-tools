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

protected:
	std::vector<SimulationData> simulationDataSet;

};

/*
 * The following class is an extension of the previos one thought to facilitate the reweighting
 * of more complicated observables, like combinations of higher moments. Here we will use the
 * name CentralMomentsPerData meaning the power of the distance of such data from the mean value
 * of that kind of data. For example, including in the container the 3 central moment per data
 * of the second column, then, per each SimulationData object in the SimulationDataContainer,
 * one column (i.e. a DataSampleBasic object) will be added at the and containing (O2-mean(O2))^3
 * where O2 is each entry of the second column and mean(O2) is the mean of the second column.
 *
 * REMARK: The number of columns ranges from ZERO on.
 *
 */
class SimulationDataContainerWithCentralMomentsPerData : public SimulationDataContainer
{
public:
    SimulationDataContainerWithCentralMomentsPerData();
//    SimulationDataContainerWithCentralMomentsPerData(std::string configurationFile);
    SimulationDataContainerWithCentralMomentsPerData(std::string configurationFile,
                                                     std::vector<unsigned int> whichColumns,
                                                     std::vector<unsigned int> whichCentralMoments);
//    void insertCentralMoments(std::vector<unsigned int> whichObservables, std::vector<unsigned int> whichCentralMoments);
    std::vector<unsigned int> getWhichColumns();
    std::vector<unsigned int> getWhichCentralMoments();

private:
    std::vector<unsigned int> whichColumns;
    std::vector<unsigned int> whichCentralMoments;
};




#endif /* SIMULATIONCONTAINERDATA_HPP_ */
