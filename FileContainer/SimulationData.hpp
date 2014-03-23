#ifndef SIMULATIONDATA_HPP_
#define SIMULATIONDATA_HPP_

#include <stdexcept>
#include <fstream>
#include <valarray>

#include "../dataSample/dataSample.hpp"

class SimulationMetaData
{
public:
	SimulationMetaData(std::vector<double> simulationParametersIn);
	double& operator[](int index);

	std::vector<double> simulationParameters;
	std::string datafileName;
	int numberOfSimulationParameters;
private:
	void checkIndex(int index);
};

class SimulationData
{
public:
	SimulationData();
	SimulationData(std::string informationFile);
	int getNumberOfDatafiles();
	int getNumberOfSimulationParameters();
	SimulationMetaData& operator[](int index);

private:
	int numberOfDatafiles;
	int numberOfSimulationParameters;
	std::vector<SimulationMetaData> simulationMetaData;
	std::vector<DataSample> simulationRawData;

	void checkIndex(int index);
	//todo: implement
	//todo: refactor into at least two functions(extract + member init)
	void extractInformationFromFile(std::string fileIn);
	//todo: this exists also in DataSample
	void checkIfDatafileExists(std::string filename);
};

#endif /* SIMULATIONDATA_HPP_ */
