/*
 *
 *  Copyright (c)
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

#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <vector>
#include <map>
#include "../dataAnalysisUtilities/DataSampleBasic.hpp"

class SimulationData
{
public:
	SimulationData();
	SimulationData(std::map<std::string, realFloat> simulationParametersIn, std::string filenameIn, const int offset = 0);

	int getNumberOfSimulationParameters();
	int getNumberOfDataSample();
	realFloat getParameterValue(std::string parameterName);
	std::map<std::string, realFloat> getSimulationParameters();
    std::string getDatafileName();
	DataSampleBasic& operator[](int index);
    void appendNewColumnOfData(DataSampleBasic sampleIn);
    void deleteColumnOfData(const int whichColumn);

private:
	std::map<std::string, realFloat> simulationParameters;
	std::string datafileName;
	std::vector<DataSampleBasic> simulationRawData;
};

#endif /* SIMULATIONDATA_HPP_ */
