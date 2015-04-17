#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>

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
static DataSample getMomentUsingMultipleColumns(const int, std::vector<DataSample>);
static DataSampleBasic getOneDataPerBinFromDataSample(DataSampleBasic, const int);
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


SimulationDataContainer SimulationDataContainer::getUncorrelatedSimulationDataSet(std::vector<int> numberOfBinsToBeUsed, ErrorCalculationMethod errorMethod)
{
	SimulationDataContainer uncorrelatedSimulationDataSet(*this); //default copy ctor should be enough
	if(errorMethod == jackknife){
		for(int i=0; i<uncorrelatedSimulationDataSet.getNumberOfDatafiles(); i++){
			int tmpSizeOfDataSample = simulationDataSet[i][0].getNumberOfElements();
			for(int j=0; j<uncorrelatedSimulationDataSet[i].getNumberOfDataSample(); j++){
				//Pick one data every binsize (the first of every bin)
				uncorrelatedSimulationDataSet[i][j] = simulationDataSet[i][j].sampleSlice(0, numberOfBinsToBeUsed[i], tmpSizeOfDataSample/numberOfBinsToBeUsed[i]);
			}
		}
	}else if(errorMethod == bootstrap){
		for(int i=0; i<uncorrelatedSimulationDataSet.getNumberOfDatafiles(); i++){
			for(int j=0; j<uncorrelatedSimulationDataSet[i].getNumberOfDataSample(); j++){
				//Pick one random data every binsize
				uncorrelatedSimulationDataSet[i][j] = getOneDataPerBinFromDataSample(simulationDataSet[i][j], numberOfBinsToBeUsed[i]);
			}
		}
	}else{
		throw std::logic_error("Invalid error method! Aborting...");
	}
	return uncorrelatedSimulationDataSet;
}

//parameterToBeUsed can be numberOfBins or binsize, but the function is the same because the formula does not change!
std::vector<int> SimulationDataContainer::getNumberOfEntriesLeftOut(std::vector<int> parameterToBeUsed)
{
	std::vector<int> entriesLeftOut;
	for(int i=0; i<getNumberOfDatafiles(); i++){
		int tmpSizeOfDataSample = simulationDataSet[i][0].getNumberOfElements();
		entriesLeftOut.push_back(tmpSizeOfDataSample - tmpSizeOfDataSample/(parameterToBeUsed[i])*(parameterToBeUsed[i]));
	}
	return entriesLeftOut;
}


SimulationDataContainer SimulationDataContainer::insertMomentsPerData(std::vector<unsigned int> whichColumns,
                                                                      std::vector<unsigned int> whichMoments,
                                                                      std::vector<bool> useMultipleColumnsForMoments)
{
    SimulationDataContainer newSimDataCont(*this);

    if(whichColumns.size() != useMultipleColumnsForMoments.size())
        throw std::invalid_argument("Wrong call to insertMomentsPerData: invalid directives for columns!");

    for(int i=0; i<newSimDataCont.getNumberOfDatafiles(); i++){
        for(size_t j=0; j<whichColumns.size(); j++){
            if(useMultipleColumnsForMoments[j]==false){
                if((int)whichColumns[j] >= simulationDataSet[i].getNumberOfDataSample())
                    throw std::out_of_range("Columns specified not valid to add central moments!");
                for(size_t k=0; k<whichMoments.size(); k++){
                    DataSample temporarySample = newSimDataCont.simulationDataSet[i][whichColumns[j]];
                    newSimDataCont.simulationDataSet[i].appendNewColumnOfData(temporarySample ^ (int)whichMoments[k]);
                }
            }else{
                unsigned int maxMoment = *max_element(whichMoments.begin(), whichMoments.end());
                if(int(whichColumns[j]+maxMoment-1) >= simulationDataSet[i].getNumberOfDataSample())
                    throw std::out_of_range("Columns specified not valid to add central moments using multipleColumns!");
                std::vector<DataSample> temporarySamples;
                for(size_t h=0; h<maxMoment; h++)
                    temporarySamples.push_back(simulationDataSet[i][whichColumns[j]+h]);
                for(size_t k=0; k<whichMoments.size(); k++)
                    newSimDataCont.simulationDataSet[i].appendNewColumnOfData(getMomentUsingMultipleColumns(whichMoments[k],
                                                                                                            temporarySamples));
                //Replace the column whichColumns[j] by the mean of the given columns
                newSimDataCont.simulationDataSet[i][whichColumns[j]] = getMomentUsingMultipleColumns(1, temporarySamples);
            }
        }
    }

//    for(int i=0; i<newSimDataCont[0].getNumberOfDataSample(); i++){
//        for(int j=0; j<newSimDataCont[0][i].getNumberOfElements(); j++)
//            std::cout << "s[0][" << i << "][" << j << "] = " << newSimDataCont[0][i][j] << "\n";
//    }
//    std::cout << "\n\n";

    //Delete columns used to get higher moments (leaving the first)
    for(int i=0; i<newSimDataCont.getNumberOfDatafiles(); i++){
        for(size_t j=0; j<whichColumns.size(); j++){
            if(useMultipleColumnsForMoments[j]==true){
                unsigned int maxMoment = *max_element(whichMoments.begin(), whichMoments.end());
                for(size_t k=1; k<maxMoment; k++)
                    //Here it seems that I delete always the same columns but indeed
                    //it is ok because when deleting the other are shifted to the left
                    newSimDataCont.simulationDataSet[i].deleteColumnOfData(whichColumns[j]+1);
            }
        }
    }

//    for(int i=0; i<newSimDataCont[0].getNumberOfDataSample(); i++){
//        for(int j=0; j<newSimDataCont[0][i].getNumberOfElements(); j++)
//            std::cout << "s[0][" << i << "][" << j << "] = " << newSimDataCont[0][i][j] << "\n";
//    }

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


static DataSample getMomentUsingMultipleColumns(const int moment, std::vector<DataSample> tempSamples){
    if((int)tempSamples.size() < moment)
        throw std::invalid_argument("Too few columns given to estimate the desired moment!");

    //TODO: Implement the following in a general way with recursive functions
    if(tempSamples.size() == 4){
        if(moment == 1)
            return (tempSamples[0] + tempSamples[1] + tempSamples[2] + tempSamples[3])/4.;
        else if(moment == 2)
            return ((tempSamples[0]*tempSamples[1]) + (tempSamples[0]*tempSamples[2]) +
                    (tempSamples[0]*tempSamples[3]) + (tempSamples[1]*tempSamples[2]) +
                    (tempSamples[1]*tempSamples[3]) + (tempSamples[2]*tempSamples[3]))/6.;
        else if(moment == 3)
            return ((tempSamples[0]*tempSamples[1]*tempSamples[2]) + (tempSamples[0]*tempSamples[1]*tempSamples[3]) +
                    (tempSamples[0]*tempSamples[2]*tempSamples[3]) + (tempSamples[1]*tempSamples[2]*tempSamples[3]))/4.;
        else
            return tempSamples[0]*tempSamples[1]*tempSamples[2]*tempSamples[3];
    }else{
        throw std::invalid_argument("Calculation of moments using multiple columns not yet implemented in the asked case!");
    }
}


static DataSampleBasic getOneDataPerBinFromDataSample(DataSampleBasic dataSetIn, const int numberOfBins){
	std::cout << "num bins = " << numberOfBins << "\n";
	DataSampleBasic dataSetOut(numberOfBins);
	int binSize = dataSetIn.getNumberOfElements() / numberOfBins;
	// construct a trivial random generator engine from a time-based seed:
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	std::uniform_int_distribution<int> distribution(0, binSize-1); //bounded could be extracted, that's why binSize-1
	//Get random entry per bin
	for(int i=0; i<numberOfBins; i++){
		dataSetOut[i] = dataSetIn[i*binSize + distribution(generator)];
	}
	return dataSetOut;
}


