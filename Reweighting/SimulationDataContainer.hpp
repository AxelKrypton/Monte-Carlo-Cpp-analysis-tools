#ifndef SIMULATIONCONTAINERDATA_HPP_
#define SIMULATIONCONTAINERDATA_HPP_

#include <stdexcept>
#include <fstream>
#include <valarray>

//todo: move class to own file!
//#include "../dataSample/dataSample.hpp"
#include "SimulationData.hpp"

class SimulationDataContainer
{
public:
	SimulationDataContainer();
	SimulationDataContainer(std::string informationFile);
	int getNumberOfDatafiles();
	int getNumberOfSimulationParameters();
	SimulationData& operator[](int index);

private:
	int numberOfDatafiles;
	int numberOfSimulationParameters;
	std::vector<SimulationData> simulationData;

	void checkIndex(int index);
	//todo: implement
	void extractInformationFromFile(std::string fileIn);
	//todo: this exists also in DataSample
	void checkIfDatafileExists(std::string filename);
};

#endif /* SIMULATIONCONTAINERDATA_HPP_ */
