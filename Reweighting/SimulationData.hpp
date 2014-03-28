#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <vector>
#include <map>
#include "../dataAnalysisUtilities/dataSample.hpp"

class SimulationData
{
public:
	SimulationData();
	SimulationData(std::map<std::string, double> simulationParametersIn, std::string filenameIn);

	int getNumberOfSimulationParameters();
	int getNumberOfDataSample();
	double getParameterValue(std::string parameterName);
	DataSample& operator[](int index);

private:
	std::map<std::string, double> simulationParameters;
	std::string datafileName;
	std::vector<DataSample> simulationRawData;
};

#endif /* SIMULATIONDATA_HPP_ */
