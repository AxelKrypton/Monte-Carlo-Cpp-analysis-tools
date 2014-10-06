#include <fstream>
#include <boost/filesystem.hpp>
#include "ReweightingDataHandler.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"

static std::vector<int> extractValuesOfBinsizes(SimulationDataContainer, std::string);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> >);

/*****************************************************************************************/

ReweightingDataHandler::ReweightingDataHandler() {
    throw std::invalid_argument("ReweightingDataHandler needs input file for construction!");
}

ReweightingDataHandler::ReweightingDataHandler(std::string configurationFileIn)
    : configurationFile(configurationFileIn), simulationDataContainer(configurationFileIn)
{
    checkCorrectnessOfConfigurationFileForReweighting();
    calculateAndSetNumberOfBinsToBeUsed();
    //Perform binning on the data
    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        for(int j=0; j<simulationDataContainer[i].getNumberOfDataSample(); j++){
            simulationDataContainer[i][j] = BinnedDataSampleFromNumberOfBins(simulationDataContainer[i][j], numberOfBinsToBeUsed, false, false);
        }
    }
    std::cout.precision(16);
    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        for(int j=0; j<simulationDataContainer[i].getNumberOfDataSample(); j++){
            std::cout << "sim[" << i << "][" << j << "] = " << simulationDataContainer[i][j].getNumberOfElements() << "    mean = " << simulationDataContainer[i][j].sum()/simulationDataContainer[i][j].getNumberOfElements() <<"\n";
//        for(int k=0; k<simulationDataContainer[i][j].getNumberOfElements(); k++)
//            std::cout << simulationDataContainer[i][j][k] << "\n";
        }
    }

}

void ReweightingDataHandler::checkCorrectnessOfConfigurationFileForReweighting(){
    std::vector<std::string> parNames, auxParNames;
    std::vector<std::vector<double> > parValues;
    Reweighter::extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simulationDataContainer[0], parNames);
    int numberOfObservablesToBeReweighted = simulationDataContainer[0].getNumberOfDataSample() - parNames.size();
    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        if(i!=0){
            Reweighter::extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simulationDataContainer[i], auxParNames);
            if(auxParNames != parNames)
                throw std::invalid_argument("Configuration file for Reweighting not valid. Use the same parameter names in each line!");
        }
        //In the following two checks is also excluded the unlucky case in which numObs < 0.
        if((int)parNames.size() > simulationDataContainer[i].getNumberOfDataSample())
            throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not enough columns!");
        if(numberOfObservablesToBeReweighted != simulationDataContainer[i].getNumberOfDataSample() - (int)parNames.size())
            throw std::logic_error("Configuration file for Reweighting not valid. Number of observables in datafiles not coherent!");
    }
    Reweighter::extractValuesOfSimulationParametersIgnoringMetaParameters(simulationDataContainer, parValues);
    for(size_t i=0; i<parValues.size(); i++){
        for(size_t j=0; j<parValues[i].size(); j++)
            std::cout << parValues[i][j] << " ";
        std::cout << "\n";
    }
    for(size_t i=1; i<parValues.size(); i++){
        if(isLastEntryPresentMoreThanOnce(parValues))
            throw std::logic_error("Files with same reweighting parameters cannot be accumulated, yet!");
    }
}

void ReweightingDataHandler::calculateAndSetNumberOfBinsToBeUsed(){
    std::vector<int> valuesOfBinsizes = extractValuesOfBinsizes(simulationDataContainer, Reweighter::metaParameters[1]);
    std::vector<int> valuesOfNumberOfBins;
    if(valuesOfBinsizes == std::vector<int>(valuesOfBinsizes.size(), 0))
        throw std::runtime_error("In the configuration file at least one binsize must be provided!");
    for(size_t i=0; i<valuesOfBinsizes.size(); i++)
        std::cout << "binsizes[" << i << "] = " << valuesOfBinsizes[i] << "\n";

    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        if(valuesOfBinsizes[i] != 0)
            valuesOfNumberOfBins.push_back(simulationDataContainer[i][0].getNumberOfElements()/valuesOfBinsizes[i]);
    }
    std::cout << "numBins = " << *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end()) << "\n";
    numberOfBinsToBeUsed = *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end());
}


SimulationDataContainer ReweightingDataHandler::getDataForReweighter(){
    return simulationDataContainer;
}


void ReweightingDataHandler::writeNewConfigurationFileWithMetaparameters(Reweighter reweighter, std::string newConfigFileName){
    if(newConfigFileName == "")
        newConfigFileName = "configFileWithLogZ";
    std::ofstream outputFile;
    outputFile.open(newConfigFileName.c_str(), std::ofstream::app);
    if(!outputFile)
        throw std::runtime_error("Something went wrong opening the file \"" + newConfigFileName + "\"!");
    outputFile.precision(16);
    outputFile << "\n\n#===================================================================================\n\n";
    for(int i=0; i<reweighter.simulationDataContainer.getNumberOfDatafiles(); i++){
        outputFile << reweighter.simulationDataContainer[i].getDatafileName() << "\t";
        for(size_t j=0; j<reweighter.reweightingParameterNames.size(); j++){
            outputFile << reweighter.reweightingParameterNames[j] << " " << reweighter.valuesOfSimulationParameters[i][j] << "\t";
        }
        for(size_t j=0; j<Reweighter::metaParameters.size(); j++){
            outputFile << Reweighter::metaParameters[j] << " ";
            if(Reweighter::metaParameters[j] == "logZ")
                outputFile << reweighter.logZAtSimulatedPoints[i];
            else if(Reweighter::metaParameters[j] == "binsize")
                outputFile << reweighter.simulationDataContainer[i][0].getNumberOfElements()/numberOfBinsToBeUsed;
            else
                throw std::runtime_error("Encountered unknown metaparameter writing new configuration file!");
            outputFile << "\t";
        }
            outputFile << "\n";
    }
    outputFile << "\n#===================================================================================\n\n";
    outputFile.close();
}

void ReweightingDataHandler::writeNewPointsToFileWithLogZ(Reweighter reweighter, std::string outputFileName){
    if(boost::filesystem::exists(outputFileName))
        throw std::invalid_argument("The file \"outputFileName\" already exists! It will not be overwritten, aborting...");
    std::ofstream outputFile;
    outputFile.open(outputFileName.c_str());
    if(!outputFile)
        throw std::runtime_error("Something went wrong opening the file \"" + outputFileName + "\"!");
    outputFile.precision(16);
    for(size_t i=0; i<reweighter.reweightingParameterNames.size(); i++)
        outputFile << "# " << reweighter.reweightingParameterNames[i] << "\t";
    outputFile << "logZ\n";
    for(size_t i=0; i<reweighter.valuesOfNewParameters.size(); i++){
        for(size_t j=0; j<reweighter.valuesOfNewParameters[i].size(); j++)
            outputFile << reweighter.valuesOfNewParameters[i][j] << "\t";
        outputFile << reweighter.logZAtNewPoints[i] << std::endl;
    }
    outputFile.close();
}



/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

/*
 * In the following function, we go through the configuration file and we check on each line
 * whether binsize is provided. If it is given  we check that its value makes sense and we store it,
 * otherwise we manually store zero to take trace that for such a file the binsize was not given.
 */
static std::vector<int> extractValuesOfBinsizes(SimulationDataContainer simDataCont, std::string labelOfBinsize){
    std::map<std::string, double> auxMap;
    std::vector<int> auxVector;
    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        bool found=false;
        auxMap = simDataCont[i].getSimulationParameters();
        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
            if(it->first == labelOfBinsize){
                if(it->second <= 0)
                    throw std::invalid_argument("At least one non positive binsize has been provided!");
                else{
                    auxVector.push_back((int)it->second);
                    found=true;
                }
                break;
            }
        }
        if(found == false)
            auxVector.push_back(0);
    }
    return auxVector;
}

static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> > parValues){
    return (parValues.size() > 1) && (std::find(parValues.begin(), parValues.end()-1, parValues.back()) != parValues.end()-1);
}
