#include "ReweighterIO.hpp"
#include "MomentsReweightingDataHandler.hpp"

static std::vector<std::string> getNamesOfParametersIgnoringMetaParameters(SimulationData);
static std::vector<std::vector<double> > getValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer);
static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData, std::vector<std::string>&, const std::vector<std::string>&);
static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer, std::vector<std::vector<double> >& , const std::vector<std::string>&);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> >);
static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer, const std::vector<std::string>, int);


/*****************************************************************************************/


ReweighterIO::ReweighterIO(LqcdReweightingParameters parameters) : readFromFileDataContainer(parameters.getInputfile()), bootstrapNumber(nullptr)
{
	namesOfParametersIgnoringMetaParameters = getNamesOfParametersIgnoringMetaParameters(readFromFileDataContainer[0]);
	valuesOfSimulationParametersIgnoringMetaParameters = getValuesOfSimulationParametersIgnoringMetaParameters(readFromFileDataContainer);
	int numberOfObservablesGivenAsInput = readFromFileDataContainer[0].getNumberOfDataSample() - namesOfParametersIgnoringMetaParameters.size();
	checkCorrectnessOfConfigurationFileForReweighting(readFromFileDataContainer, MomentsReweightingDataHandler::metaParameters, numberOfObservablesGivenAsInput);
	//Set error information
	if(parameters.getUseJackknifeAsErrorMethod())
		errorMethod = jackknife;
	else if (parameters.getUseBootstrapAsErrorMethod()){
		errorMethod = bootstrap;
		bootstrapNumber = std::unique_ptr<int>(new int(parameters.getNumberOfBootstrapResample()));
	}else
		throw std::runtime_error("Error method unknown! This exception should never be thrown! Please investigate...");
}




/******************************************** STATIC FUNCTIONS *************************************************/


static std::vector<std::string> getNamesOfParametersIgnoringMetaParameters(SimulationData simData){
    std::vector<std::string> result;
    extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simData, result, MomentsReweightingDataHandler::metaParameters);
    return result;
}

static std::vector<std::vector<double> > getValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer simDataCont){
    std::vector<std::vector<double> > result;
    extractValuesOfSimulationParametersIgnoringMetaParameters(simDataCont, result, MomentsReweightingDataHandler::metaParameters);
    return result;
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

static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> > parValues){
    return (parValues.size() > 1) && (std::find(parValues.begin(), parValues.end()-1, parValues.back()) != parValues.end()-1);
}
