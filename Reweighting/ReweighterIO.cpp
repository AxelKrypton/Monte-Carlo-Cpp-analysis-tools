#include <regex>
#include "ReweighterIO.hpp"
#include "MomentsReweightingDataHandler.hpp"

static std::vector<std::string> getNamesOfParametersIgnoringMetaParameters(SimulationData);
static std::vector<std::vector<double> > getValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer);
static std::vector<Binsizes> getValuesOfSpecifiedBinsizes(SimulationDataContainer);
static std::vector<double> getValuesOfSpecifiedLogZ(SimulationDataContainer);
static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData, std::vector<std::string>&, const std::vector<std::string>&);
static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer, std::vector<std::vector<double> >& , const std::vector<std::string>&);
static void extractValuesOfSpecifiedBinsizes(SimulationDataContainer, std::vector<Binsizes>& , const std::string&);
static void extractValuesOfSpecifiedLogZ(SimulationDataContainer, std::vector<double>& , const std::string&);
static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> >);
static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer, const std::vector<std::string>, int);
static bool isLabelMatchingAnyMetaParameter(const std::string&, const std::vector<std::string>&);


/*****************************************************************************************/


ReweighterIO::ReweighterIO(LqcdReweightingParameters parameters) : readFromFileDataContainer(parameters.getInputfile()), bootstrapNumber(nullptr)
{
	isMeanKnownToBeZero = parameters.getIsMeanKnownToBeZero();
	namesOfParametersIgnoringMetaParameters = getNamesOfParametersIgnoringMetaParameters(readFromFileDataContainer[0]);
	valuesOfSimulationParametersIgnoringMetaParameters = getValuesOfSimulationParametersIgnoringMetaParameters(readFromFileDataContainer);
	valuesOfSpecifiedBinsizes = getValuesOfSpecifiedBinsizes(readFromFileDataContainer);
	valuesOfSpecifiedLogZ = getValuesOfSpecifiedLogZ(readFromFileDataContainer);
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

static std::vector<Binsizes> getValuesOfSpecifiedBinsizes(SimulationDataContainer simDataCont){
	std::vector<Binsizes> result;
	extractValuesOfSpecifiedBinsizes(simDataCont, result, MomentsReweightingDataHandler::metaParameters[1]);
	return result;
}

static std::vector<double> getValuesOfSpecifiedLogZ(SimulationDataContainer simDataCont){
	std::vector<double> result;
	extractValuesOfSpecifiedLogZ(simDataCont, result, MomentsReweightingDataHandler::metaParameters[0]);
	return result;
}

static bool isLabelMatchingAnyMetaParameter(const std::string& label, const std::vector<std::string>& metaParameters){
	for(auto metaParameter: metaParameters){
		if(std::regex_match(label, std::regex(metaParameter + "(.*)")))
			return true;
	}
	return false;
}

static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData simData,
                                                                             std::vector<std::string>& parNames,
                                                                             const std::vector<std::string>& metaParameters){
    std::map<std::string, double> auxMap;
    auxMap = simData.getSimulationParameters();
    parNames.clear();
    for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
        if(!isLabelMatchingAnyMetaParameter(it->first, metaParameters))
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
        	if(!isLabelMatchingAnyMetaParameter(it->first, metaParameters))
                auxVector.push_back(it->second);
        }
        valuesOfSimPar.push_back(auxVector);
        auxVector.clear();
    }
}

/*
 * In the following function, we go through the configuration file and we check on each line
 * whether binsize is provided. If it is given  we check that its value makes sense and we store it,
 * otherwise we manually store zero to take trace that for such a file the binsize was not given.
 */
static void extractValuesOfSpecifiedBinsizes(SimulationDataContainer simDataCont, std::vector<Binsizes>& valuesOfBinsizes, const std::string& binsizeLabel){
	std::map<std::string, double> auxMap;
	valuesOfBinsizes = std::vector<Binsizes>(simDataCont.getNumberOfDatafiles(), Binsizes());
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		auxMap = simDataCont[i].getSimulationParameters();
		for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
			if((it->first) == binsizeLabel){
				if(it->second <= 0)
					throw std::invalid_argument("At least one non positive binsize has been provided!");
				else
					valuesOfBinsizes[i].setDefaultValue((int)it->second);
			}else if((it->first).find(binsizeLabel) != std::string::npos){ //This means that  binsizeLabel is contained in it->first
				if(it->second <= 0)
					throw std::invalid_argument("At least one non positive binsize has been provided!");
				else{
					std::string lastPartOfString = (it->first).substr((it->first).find(binsizeLabel) + binsizeLabel.length());
					if(*(lastPartOfString.begin()) == '-')
						throw std::invalid_argument("Binsize for \"negative moment\" specified in configuration file!");
					valuesOfBinsizes[i][std::stoi(lastPartOfString)] = (int)it->second; //std::stoi throws an exception if no conversion can be done!
				}
			}
		}
	}
	//If no binsize was provided throw an exception!
	bool areAllBinsizesEmpty = true;
	for(size_t i=0; i<valuesOfBinsizes.size(); i++)
		areAllBinsizesEmpty &= valuesOfBinsizes[i].empty();
	if(areAllBinsizesEmpty)
		throw std::runtime_error("In the configuration file at least one binsize must be provided!");
}

/*
 * Again we have to take trace for the logZ not provided. Since logZ
 * can take any real values, when not provided we set it to 'nan'.
 */
static void extractValuesOfSpecifiedLogZ(SimulationDataContainer simDataCont, std::vector<double>& valuesOfLogZ, const std::string& logZLabel){
	std::map<std::string, double> auxMap;
	valuesOfLogZ = std::vector<double>(simDataCont.getNumberOfDatafiles(), NAN);
	for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
		auxMap = simDataCont[i].getSimulationParameters();
		for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
			if((it->first) == logZLabel)
				valuesOfLogZ[i] = it->second;
		}
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
