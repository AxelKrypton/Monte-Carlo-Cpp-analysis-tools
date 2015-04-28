#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

class SimulationDataContainer
{
public:
	SimulationDataContainer();
    explicit SimulationDataContainer(std::string configurationFile);
	int getNumberOfDatafiles();
	int getNumberOfSimulationParameters(int fileNumber);
    SimulationDataContainer getUncorrelatedSimulationDataSet(std::vector<int> numberOfBinsToBeUsed, ErrorCalculationMethod errorMethod, std::default_random_engine *generator = NULL);
    std::vector<int> getNumberOfEntriesLeftOut(std::vector<int> parameterToBeUsed);
    SimulationData& operator[](int index);
    SimulationDataContainer insertMomentsPerData(std::vector<unsigned int> whichColumns,
                                                 std::vector<unsigned int> whichMoments,
                                                 std::vector<bool> useMultipleColumnsForMoments = std::vector<bool>(5, false));

private:
	std::vector<SimulationData> simulationDataSet;

};

/*
 * In the above class the method insertMomentsPerData is thought to facilitate the reweighting
 * of more complicated observables, like combinations of higher moments. We will use the
 * name MomentsPerData meaning the power of such data. For example, including in the container
 * the 3rd moment per data of the second column, then, per each SimulationData object in
 * the SimulationDataContainer, one column (i.e. a DataSampleBasic object) containing
 * (O2)^3 will be appended (where O2 is each entry of the second column).
 * The argument useMultipleColumnsForCentralMoments is to decide how to build the higher moments.
 * Its size must be equal to that of whichColumns and its entries are referred in the obvious way
 * to the columns. If any entry is false then the central moments are build in the standard way,
 * otherwise the columns starting from that specified are used to build the higher moments; as
 * many as the highest required moment says. Notice that in case of using several columns, these
 * columns will be at the end substituted by one containing the mean line by line.
 *
 * Let's make an example -->  insertMomentsPerData({2,6},{2,3,4},{true, false});
 * this call means:
 *  - using the four columns (four because the higher required moment is 4) starting from the number
 *    2, build the moments 2,3,4 that means:
 *
 *     SECOND MOMENT = (col2 * col3 + col2 * col4 + col2 * col5 +  col3 * col4 + col3 * col5 + col4 * col5) / 6.
 *      THIRD MOMENT = (col2 * col3 * col4 + col3 * col4 * col5 +  col4 * col5 * col2 + col5 * col2 * col3) / 4.
 *     FOURTH MOMENT =  col2 * col3 * col4 * col5
 *
 *  - using the column number 6 build moments in the standard way
 *  - replace columns 2,3,4,5 with one containing the mean of them
 *
 * REMARK: The number of columns ranges from ZERO on.
 */




#endif /* SIMULATIONCONTAINERDATA_HPP_ */
