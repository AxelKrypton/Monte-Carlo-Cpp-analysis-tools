#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <vector>
#include "../dataAnalysisUtilities/dataSample.hpp"

class SimulationData
{
public:
	SimulationData(std::vector<double> simulationParametersIn, std::string filenameIn);
	double& operator[](int index);

	std::vector<double> simulationParameters;
	std::vector<DataSample> simulationRawData;
	std::string datafileName;
	int numberOfSimulationParameters;
private:
	void checkIndex(int index);
};

#endif /* SIMULATIONDATA_HPP_ */
