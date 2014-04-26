#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <vector>
#include <map>
#include "../dataAnalysisUtilities/DataSampleBasic.hpp"

class SimulationData
{
public:
	SimulationData();
	SimulationData(std::map<std::string, double> simulationParametersIn, std::string filenameIn, const int offset = 0);

	int getNumberOfSimulationParameters();
	int getNumberOfDataSample();
	double getParameterValue(std::string parameterName);
	DataSampleBasic& operator[](int index);

private:
	std::map<std::string, double> simulationParameters;
	std::string datafileName;
	std::vector<DataSampleBasic> simulationRawData;
};

#endif /* SIMULATIONDATA_HPP_ */
