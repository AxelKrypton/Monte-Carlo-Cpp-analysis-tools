/*
 *
 *  Copyright (c) 2014-2015,2019-2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

#include <random>

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
    SimulationDataContainer buildAndGetMomentsPerData(std::vector<unsigned int> whichMoments, unsigned int ignoreFirstNColumns,
    												  std::vector<unsigned int> columnsForWhichMultipleColumnsForMomentsAreUsed = std::vector<unsigned int>(),
    												  unsigned int maximumMomentNeededOverall = 0);

private:
	std::vector<SimulationData> simulationDataSet;

};

/*
 *  ATTENTION: The number of columns ranges from ZERO on.
 *
 * In the above class the method buildAndGetMomentsPerData is thought to facilitate the reweighting
 * of more complicated quantities, like combinations of higher moments. We will use the
 * name MomentsPerData meaning the power of such data. For example, including in the container
 * the 3rd moment per data of the second column, then, per each SimulationData object in
 * the SimulationDataContainer, one column (i.e. a DataSampleBasic object) containing
 * (O2)^3 will be appended (where O2 is each entry of the second column).
 *
 * The argument columnsForWhichMultipleColumnsForMomentsAreUsed is to decide how to build the
 * higher moments.Its entries are referred to the number of the columns. For the entries of this
 * vector the columns starting from that specified are used to build the higher moments; as
 * many as the highest required moment says (see further below for an example).
 *
 * The method works as follows: per each column of each file (except the first ignoreFirstNColumns),
 * the moments specified in  whichMoments are build and appended at the end. After all the original
 * columns (again except the first ignoreFirstNColumns) are deleted.
 *
 * Let's make an example -->  buildAndGetMomentsPerData({1,2,3,4}, 1, {2});
 * this call means:
 *  - using the four columns (four because the higher required moment is 4) starting from the number
 *    2, build the moments 1,2,3,4 that means append at the end the four columns (needed for first moment
 *    in reweighting) and then:
 *
 *     SECOND MOMENT = (col2 * col3 + col2 * col4 + col2 * col5 +  col3 * col4 + col3 * col5 + col4 * col5) / 6.
 *      THIRD MOMENT = (col2 * col3 * col4 + col3 * col4 * col5 +  col4 * col5 * col2 + col5 * col2 * col3) / 4.
 *     FOURTH MOMENT =  col2 * col3 * col4 * col5
 *
 *  - using all the other columns except the first (ignoreFirstNColumns==1), build moments in the standard way
 */




#endif /* SIMULATIONCONTAINERDATA_HPP_ */
