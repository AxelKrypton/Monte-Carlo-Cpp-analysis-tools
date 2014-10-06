#include <fstream>
#include <boost/filesystem.hpp>
#include "ReweightingDataHandler.hpp"
#include "Reweighter.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"

static std::vector<int> extractValuesOfBinsizes(SimulationDataContainer, std::string);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> >);
static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData, std::vector<std::string>&, const std::vector<std::string>&);
static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer, std::vector<std::vector<double> >& , const std::vector<std::string>&);
static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer, const std::vector<std::string>, int);
static int getNumberOfBinsToBeUsed(SimulationDataContainer, const std::vector<std::string>);

/*****************************************************************************************/

/*
 * Initialization of the const static member of ReweighterAbstract class.
 */
std::string tmp[2] = {"logZ", "binsize"};
const std::vector<std::string> ReweightingDataHandler::metaParameters(tmp, tmp+2);

ReweightingDataHandler::ReweightingDataHandler() {
    throw std::invalid_argument("ReweightingDataHandler needs input file for construction!");
}

ReweightingDataHandler::ReweightingDataHandler(std::string configurationFileIn)
    : configurationFile(configurationFileIn), simulationDataContainer(configurationFileIn)
{
    numberOfObservablesToBeReweighted = simulationDataContainer[0].getNumberOfDataSample() - getNamesOfParametersIgnoringMetaParameters().size();
    checkCorrectnessOfConfigurationFileForReweighting(simulationDataContainer, ReweightingDataHandler::metaParameters, numberOfObservablesToBeReweighted);
    numberOfBinsToBeUsed = getNumberOfBinsToBeUsed(simulationDataContainer, ReweightingDataHandler::metaParameters);
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

std::vector<std::string> ReweightingDataHandler::getNamesOfParametersIgnoringMetaParameters(){
    std::vector<std::string> result;
    extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simulationDataContainer[0], result,
                                                                     ReweightingDataHandler::metaParameters);
    return result;
}

std::vector<std::vector<double> > ReweightingDataHandler::getValuesOfSimulationParametersIgnoringMetaParameters(){
    std::vector<std::vector<double> > result;
    extractValuesOfSimulationParametersIgnoringMetaParameters(simulationDataContainer, result,
                                                              ReweightingDataHandler::metaParameters);
    return result;
}

//Here we do not return a vector but we give it as argument because it is supposed to be prepared!
void ReweightingDataHandler::extractAndSetProvidedValuesOfLogZAtSimulatedPoints(std::vector<double>& logZ){
    if(logZ.size() == 0)
        throw std::invalid_argument("logZ asked to be set but not allocated!");
    for(size_t i=0; i<logZ.size(); i++){
        if(logZ[i] != 0.0)
            throw std::invalid_argument("logZ asked to be set but already with some non-zero value inside!");
    }
    std::map<std::string, double> auxMap;
    for(int i=0; i<simulationDataContainer.getNumberOfDatafiles(); i++){
        auxMap = simulationDataContainer[i].getSimulationParameters();
        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
            if(it->first == "logZ")
                logZ[i] = it->second;
        }
    }
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
    for(int i=0; i<reweighter.reweightingDataHandler.simulationDataContainer.getNumberOfDatafiles(); i++){
        outputFile << reweighter.reweightingDataHandler.simulationDataContainer[i].getDatafileName() << "\t";
        for(size_t j=0; j<reweighter.reweightingParameterNames.size(); j++){
            outputFile << reweighter.reweightingParameterNames[j] << " " << reweighter.valuesOfSimulationParameters[i][j] << "\t";
        }
        for(size_t j=0; j<ReweightingDataHandler::metaParameters.size(); j++){
            outputFile << ReweightingDataHandler::metaParameters[j] << " ";
            if(ReweightingDataHandler::metaParameters[j] == "logZ")
                outputFile << reweighter.logZAtSimulatedPoints[i];
            else if(ReweightingDataHandler::metaParameters[j] == "binsize")
                outputFile << reweighter.reweightingDataHandler.simulationDataContainer[i][0].getNumberOfElements()/numberOfBinsToBeUsed;
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

static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer simDataCont,
                                                              const std::vector<std::string> metaPar, int numObs){
    std::vector<std::string> parNames, auxParNames;
    std::vector<std::vector<double> > parValues;
    extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simDataCont[0], parNames, metaPar);
    int numberOfObservablesToBeReweighted = simDataCont[0].getNumberOfDataSample() - parNames.size();
    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        if(i!=0){
            extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simDataCont[i], auxParNames, metaPar);
            if(auxParNames != parNames)
                throw std::invalid_argument("Configuration file for Reweighting not valid. Use the same parameter names in each line!");
        }
        //In the following two checks is also excluded the unlucky case in which numObs < 0.
        if((int)parNames.size() > simDataCont[i].getNumberOfDataSample())
            throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not enough columns!");
        if(numberOfObservablesToBeReweighted != simDataCont[i].getNumberOfDataSample() - (int)parNames.size())
            throw std::logic_error("Configuration file for Reweighting not valid. Number of observables in datafiles not coherent!");
    }
    extractValuesOfSimulationParametersIgnoringMetaParameters(simDataCont, parValues, metaPar);
    for(size_t i=0; i<parValues.size(); i++){
        for(size_t j=0; j<parValues[i].size(); j++)
            std::cout << parValues[i][j] << " ";
        std::cout << "\n";
    }
    for(size_t i=1; i<parValues.size(); i++){
        if(isLastEntryPresentMoreThanOnce(parValues))
            throw std::logic_error("Files with same reweighting parameters cannot be accumulated, yet!");
    }
    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        //In the following two checks is also excluded the unlucky case in which numObs < 0.
        if((int)parNames.size() > simDataCont[i].getNumberOfDataSample())
            throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not enough columns!");
        if(numObs != simDataCont[i].getNumberOfDataSample() - (int)parNames.size())
            throw std::logic_error("Configuration file for Reweighting not valid. Number of observables in datafiles not coherent!");
    }
}


static int getNumberOfBinsToBeUsed(SimulationDataContainer simDataCont, const std::vector<std::string> metaPar){
    std::vector<int> valuesOfBinsizes = extractValuesOfBinsizes(simDataCont, metaPar[1]);
    std::vector<int> valuesOfNumberOfBins;
    if(valuesOfBinsizes == std::vector<int>(valuesOfBinsizes.size(), 0))
        throw std::runtime_error("In the configuration file at least one binsize must be provided!");
    for(size_t i=0; i<valuesOfBinsizes.size(); i++)
        std::cout << "binsizes[" << i << "] = " << valuesOfBinsizes[i] << "\n";

    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        if(valuesOfBinsizes[i] != 0)
            valuesOfNumberOfBins.push_back(simDataCont[i][0].getNumberOfElements()/valuesOfBinsizes[i]);
    }
    std::cout << "numBins = " << *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end()) << "\n";
    return *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end());
}


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


static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData simData,
                                                                             std::vector<std::string>& parNames,
                                                                             const std::vector<std::string>& metaParameters){
    std::map<std::string, double> auxMap;
    auxMap = simData.getSimulationParameters();
    parNames.clear();
    for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
        if(find(metaParameters.begin(), metaParameters.end(), it->first) == metaParameters.end())
            parNames.push_back(it->first);
    }
}

static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer simDataCont,
                                                                      std::vector<std::vector<double> >& valuesOfSimPar,
                                                                      const std::vector<std::string>& metaParameters){
    std::map<std::string, double> auxMap;
    std::vector<double> auxVector;
    valuesOfSimPar.clear();
    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        auxMap = simDataCont[i].getSimulationParameters();
        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
            if(find(metaParameters.begin(), metaParameters.end(), it->first) == metaParameters.end())
                auxVector.push_back(it->second);
        }
        valuesOfSimPar.push_back(auxVector);
        auxVector.clear();
    }
}


static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> > parValues){
    return (parValues.size() > 1) && (std::find(parValues.begin(), parValues.end()-1, parValues.back()) != parValues.end()-1);
}
