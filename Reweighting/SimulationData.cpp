#include <fstream>
#include <sstream>

#include "SimulationData.hpp"


static void checkIfDatafileExists(std::string);
static void throwInvalidColumnOfFile(std::string, const int);
static void checkIfDatafileWasGood(std::vector<DataSampleBasic>, std::string);

/*****************************************************************************************/

SimulationData::SimulationData()
{
	throw std::invalid_argument("SimulationData needs input file for construction!");
}

SimulationData::SimulationData(std::map<std::string, double> simulationParametersIn, std::string filenameIn, const int offset) :
	simulationParameters(simulationParametersIn), datafileName(filenameIn)
{
	if(simulationParametersIn.size() == 0)
		throw std::logic_error("SimulationData object built without any simulation parameter!");
	else{
		checkIfDatafileExists(filenameIn);
		for(int i=0; ; i++){
			bool charInLine;
			try{
				simulationRawData.push_back(DataSampleBasic(filenameIn, i+1, offset, &charInLine));
				/*
				 * Note: if in a column are present only invalid data, the above push_back throw an exception
				 *       and the rest of the try is skipped. Furthermore one cannot rely on the value of charInLine
				 *       because the DataSample constructor throw an exception before setting it. Nevertheless FileReader
				 *       distinguish between a column of only invalid data (std::invalid_argument thrown) or a column of
				 *       empty data, i.e. a not existing column (std::out_of_range thrown). We use this in the two catch blocks.
				 */
				if(charInLine)
					throwInvalidColumnOfFile(filenameIn, i+1);
			}catch (std::out_of_range exceptionThrown){
				break;
			}catch(std::invalid_argument exceptionThrown){
				throwInvalidColumnOfFile(filenameIn, i+1);
			}
		}
	}

	checkIfDatafileWasGood(simulationRawData, filenameIn);
}


int SimulationData::getNumberOfSimulationParameters() {
	return simulationParameters.size();
}


int SimulationData::getNumberOfDataSample(){
	return simulationRawData.size();
}


std::map<std::string, double> SimulationData::getSimulationParameters(){
	return simulationParameters;
}


double SimulationData::getParameterValue(std::string parameterName)
{
	//This would be one line with c++11 with .at()
	if(simulationParameters.find(parameterName) == simulationParameters.end())
		throw std::out_of_range("The given parameter \"" + parameterName + "\" has not been found!");
	return simulationParameters[parameterName];
}


std::string SimulationData::getDatafileName(){
    return datafileName;
}


DataSampleBasic& SimulationData::operator[](int index)
{
	return simulationRawData.at(index);
}

/*****************************************************************************************/

//todo: use stat of sys/stat.h to check existence and access to the file
void checkIfDatafileExists(std::string filename){
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}

static void throwInvalidColumnOfFile(std::string filenameIn, const int columnNumber){
	std::ostringstream tmp;
	tmp << "Invalid column of data encountered in file \"" + filenameIn + "\" at column " << columnNumber << ".";
	throw std::invalid_argument(tmp.str());
}

/*
 * Here we check if the data read from the file are in the proper form for reweighting. This means
 * that all lines must have had the same amount of columns, namely each created DataSample should
 * have the same amount of elements.
 */
static void checkIfDatafileWasGood(std::vector<DataSampleBasic> rawData, std::string fileIn){
	if(rawData.size() == 0)
		throw std::invalid_argument("Datafile \"" + fileIn + "\" contains only lines to be ignored or not numeric data!!");
	for(size_t i=1; i< rawData.size(); i++){
		if(rawData[i].getNumberOfElements() != rawData[0].getNumberOfElements())
			throw std::logic_error("Datafile \"" + fileIn + "\" had not the right structure for reweighting!");
	}
}

