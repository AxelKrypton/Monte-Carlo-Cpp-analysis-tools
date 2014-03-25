#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <vector>
#include <map>
#include "../dataAnalysisUtilities/dataSample.hpp"

class SimulationData
{
public:
	SimulationData(std::vector<double> simulationParametersIn, std::string filenameIn);
	SimulationData(std::map<std::string, double> simulationParametersIn, std::string filenameIn);
	//double& operator[](int index);

	int getNumberOfSimulationParameters();

	//std::vector<double> simulationParameters;
	std::map<std::string, double> simulationParameters;
	std::vector<DataSample> simulationRawData;
	std::string datafileName;
	int numberOfSimulationParameters;
private:
	void checkIndex(int index);
};

#endif /* SIMULATIONDATA_HPP_ */
