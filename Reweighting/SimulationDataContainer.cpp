/*
 *
 *  Copyright (c) 2014-2015,2020 Alessandro Sciarra
 *  Copyright (c) 2015 Christopher Pinke
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

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>


#include "SimulationDataContainer.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"


static void checkIfDatafileExists(std::string);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::string>);
static bool isLastMapPresentMoreThanOnce(std::vector<std::map<std::string, realFloat> >);
static bool mapCompare(const std::map<std::string, realFloat>&, const std::map<std::string, realFloat>&);
static bool isAnyMapEmpty(std::vector<std::map<std::string, realFloat> >);
static void extractInformationFromFile(std::string fileIn,
                                       std::vector<std::string>& dataFilenames,
                                       std::vector<std::map<std::string, realFloat> >& dataParameters);
static DataSample getMomentUsingMultipleColumns(const int, std::vector<DataSample>);
static DataSampleBasic getRandomlyOneDataPerBinFromDataSample(DataSampleBasic, const int, bool, std::default_random_engine* = NULL);
/*****************************************************************************************/

SimulationDataContainer::SimulationDataContainer()
{
	throw std::invalid_argument("SimulationDataContainer needs input file for construction!");
}

SimulationDataContainer::SimulationDataContainer(std::string informationFile)
{
	checkIfDatafileExists(informationFile);
	std::vector<std::string> dataFilenames;
	std::vector<std::map<std::string, realFloat> > dataParameters;
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


SimulationDataContainer SimulationDataContainer::getUncorrelatedSimulationDataSet(std::vector<int> numberOfBinsToBeUsed, ErrorCalculationMethod errorMethod,
																				  std::default_random_engine *generator)
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
		if( generator == NULL )
			throw std::invalid_argument("Function \"getUncorrelatedSimulationDataSet\" called without random generator needed for the bootstrap case! Aborting...");
		for(int i=0; i<uncorrelatedSimulationDataSet.getNumberOfDatafiles(); i++){
			for(int j=0; j<uncorrelatedSimulationDataSet[i].getNumberOfDataSample(); j++){
				//Bin all columns as the first was binned!
				if(j==0)
					uncorrelatedSimulationDataSet[i][j] = getRandomlyOneDataPerBinFromDataSample(simulationDataSet[i][j], numberOfBinsToBeUsed[i], false, generator);
				else
					uncorrelatedSimulationDataSet[i][j] = getRandomlyOneDataPerBinFromDataSample(simulationDataSet[i][j], numberOfBinsToBeUsed[i], true);
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

/*
 * Here the strategy is to insert at the end the moments for all columns and then delete the original columns
 */
SimulationDataContainer SimulationDataContainer::buildAndGetMomentsPerData(std::vector<unsigned int> whichMoments, unsigned int ignoreFirstNColumns,
																		   std::vector<unsigned int> columnsForWhichMultipleColumnsForMomentsAreUsed,
																		   unsigned int maximumMomentNeededOverall)
{
    SimulationDataContainer newSimDataCont(*this);
    for(size_t i=0; i<columnsForWhichMultipleColumnsForMomentsAreUsed.size(); i++){
    	if(columnsForWhichMultipleColumnsForMomentsAreUsed[i] < ignoreFirstNColumns)
    		throw std::logic_error("In \"buildAndGetMomentsPerData\" asked to use multiple columns in building moments of an ignored column!");
    }

    for(size_t i=0; i<simulationDataSet.size(); i++){
    	if((int)ignoreFirstNColumns >= simulationDataSet[i].getNumberOfDataSample())
    		throw std::logic_error("In \"buildAndGetMomentsPerData\" asked to ignore all columns or more!");
        for(int j=ignoreFirstNColumns; j<simulationDataSet[i].getNumberOfDataSample(); /*increment in cases below*/){
            if(find(columnsForWhichMultipleColumnsForMomentsAreUsed.begin(), columnsForWhichMultipleColumnsForMomentsAreUsed.end(), j) == columnsForWhichMultipleColumnsForMomentsAreUsed.end()){
                for(size_t k=0; k<whichMoments.size(); k++){
                    DataSample temporarySample = newSimDataCont.simulationDataSet[i][j];
                    newSimDataCont.simulationDataSet[i].appendNewColumnOfData(temporarySample ^ (int)whichMoments[k]);
                }
                j++;
            }else{
                unsigned int maxMoment = (maximumMomentNeededOverall ==0 ) ? *max_element(whichMoments.begin(), whichMoments.end()) : maximumMomentNeededOverall;
                if(int(j+maxMoment-1) >= simulationDataSet[i].getNumberOfDataSample())
                    throw std::out_of_range("Columns specified not valid to add specified moments using multipleColumns!");
                std::vector<DataSample> temporarySamples;
                for(size_t h=0; h<maxMoment; h++)
                    temporarySamples.push_back(simulationDataSet[i][j+h]);
                for(size_t k=0; k<whichMoments.size(); k++){
                	//If the moments is the first we have to copy all the columns at the end
                	if(whichMoments[k] == 1){
                		for(size_t h=0; h<temporarySamples.size(); h++)
                			newSimDataCont.simulationDataSet[i].appendNewColumnOfData(temporarySamples[h]);
                	}else
                		newSimDataCont.simulationDataSet[i].appendNewColumnOfData(getMomentUsingMultipleColumns(whichMoments[k], temporarySamples));
                }
                j+=maxMoment;
            }
        }
    }
    //Delete all original columns (use this object to recover original number of columns)
    for(size_t i=0; i<simulationDataSet.size(); i++){
        for(int j=ignoreFirstNColumns; j<simulationDataSet[i].getNumberOfDataSample(); j++){
			//Here it seems that I delete always the same columns but indeed
			//it is ok because when deleting the other are shifted to the left
        	newSimDataCont.simulationDataSet[i].deleteColumnOfData(ignoreFirstNColumns);
        }
    }
    return newSimDataCont;
}

/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

static void extractInformationFromFile(std::string fileIn,
    std::vector<std::string>& dataFilenames, std::vector<std::map<std::string, realFloat> >& dataParameters)
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
			realFloat auxForParameterValue;
			std::string auxForParameterName, auxForFilename;
			std::map<std::string, realFloat> auxMap;

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

static bool isLastMapPresentMoreThanOnce(std::vector<std::map<std::string, realFloat> > parameters){
	for(size_t i = 0; i<parameters.size()-1; i++){
		if(mapCompare(parameters[i], parameters.back()))
			return true;
	}
	return false;
}

static bool mapCompare (const std::map<std::string, realFloat>& lhs, const std::map<std::string, realFloat>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

static bool isAnyMapEmpty(std::vector<std::map<std::string, realFloat> > dataParameters){
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
            return (tempSamples[0] + tempSamples[1] + tempSamples[2] + tempSamples[3])/(realFloat)4.0;
        else if(moment == 2)
            return ((tempSamples[0]*tempSamples[1]) + (tempSamples[0]*tempSamples[2]) +
                    (tempSamples[0]*tempSamples[3]) + (tempSamples[1]*tempSamples[2]) +
                    (tempSamples[1]*tempSamples[3]) + (tempSamples[2]*tempSamples[3]))/(realFloat)6.0;
        else if(moment == 3)
            return ((tempSamples[0]*tempSamples[1]*tempSamples[2]) + (tempSamples[0]*tempSamples[1]*tempSamples[3]) +
                    (tempSamples[0]*tempSamples[2]*tempSamples[3]) + (tempSamples[1]*tempSamples[2]*tempSamples[3]))/(realFloat)4.0;
        else
            return tempSamples[0]*tempSamples[1]*tempSamples[2]*tempSamples[3];
    }else{
        throw std::invalid_argument("Calculation of moments using multiple columns not yet implemented in the asked case!");
    }
}

static DataSampleBasic getRandomlyOneDataPerBinFromDataSample(DataSampleBasic dataSetIn, const int numberOfBins, bool sameIndecesAsLastTime, std::default_random_engine* generator){
	static bool firstTimeThisFunctionIsCalled = true;
	static int dimensionDataSetLastCall;
	static int numberOfBinsLastCall;
	static std::vector<int> indecesUsedLastCall;
	if(firstTimeThisFunctionIsCalled){
		dimensionDataSetLastCall = dataSetIn.getNumberOfElements();
		numberOfBinsLastCall = numberOfBins;
		firstTimeThisFunctionIsCalled = false;
	}

	DataSampleBasic dataSetOut(numberOfBins);
	if(sameIndecesAsLastTime){
		if(dataSetIn.getNumberOfElements() != dimensionDataSetLastCall || numberOfBins != numberOfBinsLastCall || indecesUsedLastCall.size() == 0)
			throw std::invalid_argument("Invalid call to \"getRandomlyOneDataPerBinFromDataSample\" with sameIndecesAsLastTime==true! Aborting...");
		if(indecesUsedLastCall.size() != (size_t)numberOfBins)
			throw std::runtime_error("In function \"getRandomlyOneDataPerBinFromDataSample\" indecesUsedLastCall is not empty but has wrong size!! Investigate...");
		for(int i=0; i<numberOfBins; i++){
			dataSetOut[i] = dataSetIn[indecesUsedLastCall[i]];
		}
	}else{
		int binSize = dataSetIn.getNumberOfElements() / numberOfBins;
		std::uniform_int_distribution<int> distribution(0, binSize-1); //bounded could be extracted, that's why binSize-1
		//Get random entry per bin
		indecesUsedLastCall.clear();
		for(int i=0; i<numberOfBins; i++){
			indecesUsedLastCall.push_back(i*binSize + distribution(*generator));
			dataSetOut[i] = dataSetIn[indecesUsedLastCall[i]];
		}
		//Set variables for future calls
		dimensionDataSetLastCall = dataSetIn.getNumberOfElements();
		numberOfBinsLastCall = numberOfBins;
	}

	return dataSetOut;
}


