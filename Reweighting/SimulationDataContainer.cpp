#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "SimulationDataContainer.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"


static void checkIfDatafileExists(std::string);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::string>);
static bool isLastMapPresentMoreThanOnce(std::vector<std::map<std::string, double> >);
static bool mapCompare(const std::map<std::string, double>&, const std::map<std::string, double>&);
static bool isAnyMapEmpty(std::vector<std::map<std::string, double> >);
static void extractInformationFromFile(std::string fileIn,
                                       std::vector<std::string>& dataFilenames,
                                       std::vector<std::map<std::string, double> >& dataParameters);
/*****************************************************************************************/

SimulationDataContainer::SimulationDataContainer()
{
	throw std::invalid_argument("SimulationDataContainer needs input file for construction!");
}

SimulationDataContainer::SimulationDataContainer(std::string informationFile)
{
	checkIfDatafileExists(informationFile);
	std::vector<std::string> dataFilenames;
	std::vector<std::map<std::string, double> > dataParameters;
	extractInformationFromFile(informationFile, dataFilenames, dataParameters);
	if(isAnyMapEmpty(dataParameters))
		throw std::logic_error("In given file \"" + informationFile + "\", no parameters for at least one simulation!");
	for (size_t i = 0; i < dataFilenames.size(); i++)
		simulationDataSet.push_back(SimulationData(dataParameters[i], dataFilenames[i]));
}

SimulationData& SimulationDataContainer::operator[](int index)
{
	return simulationDataSet.at(index);
}

int SimulationDataContainer::getNumberOfDatafiles()
{
	return simulationDataSet.size();
}

int SimulationDataContainer::getNumberOfSimulationParameters(int fileNumber)
{
	return simulationDataSet[fileNumber].getNumberOfSimulationParameters();
}

std::pair<SimulationDataContainer, std::vector<int> >
SimulationDataContainer::getBinnedSimulationDataSetAndNumbersOfEntriesLeftOut(int numberOfBinsToBeUsed){
    SimulationDataContainer binnedSimulationDataSet(*this); //default copy ctor should be enough
    std::vector<int> entriesLeftOut;
    //Here we make no check on the datafile, since they already were done in SimulationData ctor
    for(int i=0; i<binnedSimulationDataSet.getNumberOfDatafiles(); i++){
        for(int j=0; j<binnedSimulationDataSet[i].getNumberOfDataSample(); j++){
            binnedSimulationDataSet[i][j] = BinnedDataSampleFromNumberOfBins(simulationDataSet[i][j],
                                                                             numberOfBinsToBeUsed, false, false);
        }
        int tmpSizeOfDataSample = simulationDataSet[i][0].getNumberOfElements();
        entriesLeftOut.push_back(tmpSizeOfDataSample - tmpSizeOfDataSample/numberOfBinsToBeUsed*numberOfBinsToBeUsed);
    }
    return std::make_pair(binnedSimulationDataSet, entriesLeftOut);
}


SimulationDataContainer SimulationDataContainer::insertCentralMomentsPerData(std::vector<unsigned int> whichColumns,
                                                                             std::vector<unsigned int> whichCentralMoments)
{
    SimulationDataContainer newSimDataCont(*this);

    for(int i=0; i<newSimDataCont.getNumberOfDatafiles(); i++){
        for(size_t j=0; j<whichColumns.size(); j++){
            if((int)whichColumns[j] >= newSimDataCont.simulationDataSet[i].getNumberOfDataSample())
                throw std::out_of_range("Columns specified not valid to add central moments!");
            for(size_t k=0; k<whichCentralMoments.size(); k++){
                DataSample temporarySample = newSimDataCont.simulationDataSet[i][whichColumns[j]];
                newSimDataCont.simulationDataSet[i].appendNewColumnOfData(temporarySample ^ (int)whichCentralMoments[k]);
                /*newSimDataCont.simulationDataSet[i].appendNewColumnOfData((temporarySample -
		  temporarySample.getNthMoment(1)) ^ (int)whichCentralMoments[k]);*/
            }
        }
    }

    return newSimDataCont;
}

/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

static void extractInformationFromFile(std::string fileIn,
    std::vector<std::string>& dataFilenames, std::vector<std::map<std::string, double> >& dataParameters)
{
	std::ifstream infile(fileIn.c_str());
	if(!infile.is_open())
		throw std::invalid_argument("Given file \"" + fileIn + "\" failed to be opened!");

	std::string line;
	while (std::getline(infile, line))	{
		//ignore lines beginning by # since they are comments for gnuplot and empty lines
		if(line.find_first_not_of("#\t ") < line.find_first_of("#")){
			line = line.substr(0, line.find("#", 0)); //remove everything after '#' in each line

			std::stringstream ss(line);
			double auxForParameterValue;
			std::string auxForParameterName, auxForFilename;
			std::map<std::string, double> auxMap;

			(ss >> auxForFilename) ? dataFilenames.push_back(auxForFilename)
					               : throw std::runtime_error("Error reading datafile \"" + fileIn + "\"");

			if(isLastEntryPresentMoreThanOnce(dataFilenames)){
					throw std::invalid_argument("Configuration file \"" + fileIn + "\" contains the datafile \"" + dataFilenames.back() + "\" more than once!");
			}

			while(ss.good() && (ss.str().substr(ss.tellg())).find_first_not_of(" \t\n") != std::string::npos){
				if(ss >> auxForParameterName >> auxForParameterValue)
					auxMap[auxForParameterName] = auxForParameterValue;
				else
					throw std::runtime_error("Error reading datafile \"" + fileIn + "\"");
			}

			dataParameters.push_back(auxMap);
			if(isLastMapPresentMoreThanOnce(dataParameters))
				throw std::logic_error("Files with same parameters cannot be accumulated, yet!");
		}
	}
	if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
	{
		throw std::runtime_error("Error reading datafile");
	}
	infile.close();
}

//todo: use stat of sys/stat.h to check existence and access to the file
static void checkIfDatafileExists(std::string filename){
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}

static bool isLastEntryPresentMoreThanOnce(std::vector<std::string> filenames){
	return (filenames.size() > 1) && (std::find(filenames.begin(), filenames.end()-1, filenames.back()) != filenames.end()-1);
}

static bool isLastMapPresentMoreThanOnce(std::vector<std::map<std::string, double> > parameters){
	for(size_t i = 0; i<parameters.size()-1; i++){
		if(mapCompare(parameters[i], parameters.back()))
			return true;
	}
	return false;
}

static bool mapCompare (const std::map<std::string, double>& lhs, const std::map<std::string, double>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

static bool isAnyMapEmpty(std::vector<std::map<std::string, double> > dataParameters){
	for(size_t i=0; i<dataParameters.size(); i++){
		if(dataParameters[i].size() == 0)
			return true;
	}
	return false;
}

