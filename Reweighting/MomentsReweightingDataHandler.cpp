#include <fstream>
#include <boost/filesystem.hpp>
#include "MomentsReweightingDataHandler.hpp"
#include "MomentsReweighter.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"

static void setNumberOfBinsToBeUsedAndEntriesToBeLeftOut(SimulationDataContainer, std::vector<int>, ErrorCalculationMethod, std::vector<int>&, std::vector<int>&);
static void printBinsizesActuallyUsed(SimulationDataContainer, std::vector<int>);

//static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> >);
//static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData, std::vector<std::string>&, const std::vector<std::string>&);
//static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer, std::vector<std::vector<double> >& , const std::vector<std::string>&);
//static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer, const std::vector<std::string>, int);
//static std::vector<int> extractValuesOfBinsizes(SimulationDataContainer, std::string);
//static std::vector<int> getNumberOfBinsToBeUsedAndEntriesToBeLeftOut(SimulationDataContainer, const std::vector<std::string>, ErrorCalculationMethod, std::vector<int>&);
//static std::pair<ErrorCalculationMethod, std::string> getErrorCalculationMethod(std::string);

/*****************************************************************************************/

/*
 * Initialization of the const static member of ReweighterAbstract class.
 */
std::string tmp[2] = {"logZ", "binsize"};
const std::vector<std::string> MomentsReweighterHelper::metaParameters(tmp, tmp+2);


/*
 * Here in this constructor we cannot initialize properly the simulationUncorrDataContainer object,
 * since its content depends on the error method adopted in the reweighting. Actually its content
 * is fixed for the jackknife but it is not for the bootstrap. So it makes sense to set it here
 * for the jackknife (especially because then for the ReweighterTest it has already been done) and
 * set it temporary to a copy of simulationRawDataContainer for the bootstrap (later in the Reweighter
 * we reset it when needed, see MomentsReweighterAbstract::calculateAndGetReweightedObservables method).
 *
 * TODO: Refactor this ctor that grow more and more in time...
 */
MomentsReweighterHelper::MomentsReweighterHelper(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformation)
	: simulationRawDataContainer(rawDataForReweightingAndMetainformation.rawData), simulationUncorrDataContainer(rawDataForReweightingAndMetainformation.rawData),
	  namesOfParametersIgnoringMetaParameters(rawDataForReweightingAndMetainformation.namesOfParametersIgnoringMetaParameters),
	  valuesOfSimulationParametersIgnoringMetaParameters(rawDataForReweightingAndMetainformation.valuesOfSimulationParametersIgnoringMetaParameters),
	  errorMethod(rawDataForReweightingAndMetainformation.errorMethod), bootstrapNumber(rawDataForReweightingAndMetainformation.bootstrapNumber)
{
	std::vector<int> entriesToBeCutFromRawData;
	setNumberOfBinsToBeUsedAndEntriesToBeLeftOut(simulationRawDataContainer, rawDataForReweightingAndMetainformation.binsizesToBeUsed, errorMethod,
												 numberOfBinsToBeUsed, entriesToBeCutFromRawData);
	//Evaluate central moments per data and append them to the raw data container
	std::vector<unsigned int> obsToBeRewUsingMultipleColumns = rawDataForReweightingAndMetainformation.columnsToBeReweightedUsingMultipleColumns;
	std::vector<unsigned int> momentsNeeded = rawDataForReweightingAndMetainformation.momentsToBeReweighted;
	unsigned int maxMomentNeeded = rawDataForReweightingAndMetainformation.maximumMomentNeededOverall;
	if(momentsNeeded.empty())
		throw std::logic_error("MomentsReweighterHelper asked to be built without any moment to be reweighted! Aborting...");
	std::sort(obsToBeRewUsingMultipleColumns.begin(), obsToBeRewUsingMultipleColumns.end());
	for(size_t i=1; i<obsToBeRewUsingMultipleColumns.size(); i++){
		if(obsToBeRewUsingMultipleColumns[i]-obsToBeRewUsingMultipleColumns[i-1] < maxMomentNeeded)
			throw std::invalid_argument("obsToBeRewUsingMultipleColumns contains columns too close (distance<" + std::to_string(maxMomentNeeded) + ")!");
	}

	//Here I set the number of "real" observables given as input (neglecting the multiple columns)
	numberOfObservablesGivenAsInput = simulationRawDataContainer[0].getNumberOfDataSample() - namesOfParametersIgnoringMetaParameters.size();
	numberOfObservablesGivenAsInput -= obsToBeRewUsingMultipleColumns.size()*(maxMomentNeeded-1); //neglect multiple columns (count one column only)
	simulationRawDataContainer = simulationRawDataContainer.buildAndGetMomentsPerData(momentsNeeded, namesOfParametersIgnoringMetaParameters.size(), obsToBeRewUsingMultipleColumns);
	numberOfObservablesToBeReweighted = simulationRawDataContainer[0].getNumberOfDataSample() - namesOfParametersIgnoringMetaParameters.size();

	std::cout << "numberOfObservablesGivenAsInput = " << numberOfObservablesGivenAsInput << "\n";
	std::cout << "numberOfObservablesToBeReweighted = " << numberOfObservablesToBeReweighted << "\n";

	if(errorMethod == jackknife)
		simulationUncorrDataContainer = simulationRawDataContainer.getUncorrelatedSimulationDataSet(numberOfBinsToBeUsed, jackknife);
	else
		simulationUncorrDataContainer = simulationRawDataContainer;

	//Refining on the raw data
	for(int i=0; i<simulationRawDataContainer.getNumberOfDatafiles(); i++){
		for(int j=0; j<simulationRawDataContainer[i].getNumberOfDataSample(); j++){
			simulationRawDataContainer[i][j] = simulationRawDataContainer[i][j].removeLastNElements(entriesToBeCutFromRawData[i]);
		}
	}
	//This has to be done after having cut the data in order to print the right information
	printBinsizesActuallyUsed(simulationRawDataContainer, numberOfBinsToBeUsed);
}


//MomentsReweighterHelper::MomentsReweighterHelper(std::string configurationFileIn,
//                                               std::vector<unsigned int> obsToBeRewUsingMultipleColumns,
//                                               std::string errorMethodIn)
//    : configurationFile(configurationFileIn), simulationRawDataContainer(configurationFileIn),
//      simulationUncorrDataContainer(simulationRawDataContainer), bootstrapNumber(NULL)
//{
//	std::pair<ErrorCalculationMethod, std::string> temporaryPair = getErrorCalculationMethod(errorMethodIn);
//	errorMethod = temporaryPair.first;
//	if(temporaryPair.second != "unset"){
//		bootstrapNumber=new int;
//		*bootstrapNumber = std::stoi( temporaryPair.second );
//	}
//	numberOfObservablesGivenAsInput = simulationRawDataContainer[0].getNumberOfDataSample() - getNamesOfParametersIgnoringMetaParameters().size();
//    checkCorrectnessOfConfigurationFileForReweighting(simulationRawDataContainer, MomentsReweighterHelper::metaParameters, numberOfObservablesGivenAsInput);
//
//    std::vector<int> entriesToBeCutFromRawData;
//    numberOfBinsToBeUsed = getNumberOfBinsToBeUsedAndEntriesToBeLeftOut(simulationRawDataContainer, MomentsReweighterHelper::metaParameters, errorMethod, entriesToBeCutFromRawData);
//    //Evaluate central moments per data and append them to the raw data container
//    //TODO: So far this is hard-coded, make it general and settable by user
//    std::vector<unsigned int> columnsForWhichMomentsMustBeInserted;
//    std::vector<unsigned int> momentsNeeded;
//    std::vector<bool> useMultipleColumnsForMoments;
//    momentsNeeded.push_back(2);
//    momentsNeeded.push_back(3);
//    momentsNeeded.push_back(4);
//    std::sort(obsToBeRewUsingMultipleColumns.begin(), obsToBeRewUsingMultipleColumns.end());
//    for(size_t i=1; i<obsToBeRewUsingMultipleColumns.size(); i++){
//        if(obsToBeRewUsingMultipleColumns[i]-obsToBeRewUsingMultipleColumns[i-1]<4)
//            throw std::invalid_argument("obsToBeRewUsingMultipleColumns contains columns too close (distance<4)!");
//    }
//    for(size_t i=getNamesOfParametersIgnoringMetaParameters().size(); (int)i<simulationRawDataContainer[0].getNumberOfDataSample();){
//        columnsForWhichMomentsMustBeInserted.push_back(i);
//        if(find(obsToBeRewUsingMultipleColumns.begin(), obsToBeRewUsingMultipleColumns.end(), (unsigned int)(i-getNamesOfParametersIgnoringMetaParameters().size())) != obsToBeRewUsingMultipleColumns.end()){
//            useMultipleColumnsForMoments.push_back(true);
//            i+=4;
//        }else{
//            useMultipleColumnsForMoments.push_back(false);
//            i++;
//        }
//
//    }
//    simulationRawDataContainer = simulationRawDataContainer.insertMomentsPerData(columnsForWhichMomentsMustBeInserted, momentsNeeded, useMultipleColumnsForMoments);
//    //Here I set the number of "real" observables given as input (neglecting the multiple columns)
//    numberOfObservablesGivenAsInput = (int)columnsForWhichMomentsMustBeInserted.size();
//    numberOfObservablesToBeReweighted = simulationRawDataContainer[0].getNumberOfDataSample() - getNamesOfParametersIgnoringMetaParameters().size();
//
//    if(errorMethod == jackknife)
//    	simulationUncorrDataContainer = simulationRawDataContainer.getUncorrelatedSimulationDataSet(numberOfBinsToBeUsed, jackknife);
//    else
//    	simulationUncorrDataContainer = simulationRawDataContainer;
//
//    //Refining on the raw data
//    for(int i=0; i<simulationRawDataContainer.getNumberOfDatafiles(); i++){
//        for(int j=0; j<simulationRawDataContainer[i].getNumberOfDataSample(); j++){
//            simulationRawDataContainer[i][j] = simulationRawDataContainer[i][j].removeLastNElements(entriesToBeCutFromRawData[i]);
//        }
//    }
//    //This has to be done after having cut the data in order to print the right information
//    printBinsizesActuallyUsed(simulationRawDataContainer, numberOfBinsToBeUsed);
//
//}
//
////Copy constructor needed since we have a raw pointer as member!
//MomentsReweighterHelper::MomentsReweighterHelper(const MomentsReweighterHelper& objectIn)
//   : numberOfBinsToBeUsed(objectIn.numberOfBinsToBeUsed),
//     numberOfObservablesGivenAsInput(objectIn.numberOfObservablesGivenAsInput),
//     numberOfObservablesToBeReweighted(objectIn.numberOfObservablesToBeReweighted),
//     configurationFile(objectIn.configurationFile),
//     simulationRawDataContainer(objectIn.simulationRawDataContainer),
//     simulationUncorrDataContainer(objectIn.simulationUncorrDataContainer),
//     errorMethod(objectIn.errorMethod)
//{
//	if(objectIn.bootstrapNumber == NULL)
//		bootstrapNumber = NULL;
//	else
//		bootstrapNumber = new int(*(objectIn.bootstrapNumber));
//}
//
////Equal operator needed since we have a raw pointer as member!
//MomentsReweighterHelper& MomentsReweighterHelper::operator=(const MomentsReweighterHelper& rhs){
//	// check for "self assignment" and do nothing in that case
//	if (this == &rhs) return *this;
//	else{
//		numberOfBinsToBeUsed = rhs.numberOfBinsToBeUsed;
//		numberOfObservablesGivenAsInput = rhs.numberOfObservablesGivenAsInput;
//		numberOfObservablesToBeReweighted = rhs.numberOfObservablesToBeReweighted;
//		configurationFile = rhs.configurationFile;
//		simulationRawDataContainer = rhs.simulationRawDataContainer;
//		simulationUncorrDataContainer = rhs.simulationUncorrDataContainer;
//		errorMethod = rhs.errorMethod;
//		if(bootstrapNumber != NULL) delete bootstrapNumber;
//		if(rhs.bootstrapNumber == NULL)
//				bootstrapNumber = NULL;
//			else
//				bootstrapNumber = new int(*(rhs.bootstrapNumber));
//	}
//	return *this;
//}

//std::vector<std::string> MomentsReweighterHelper::getNamesOfParametersIgnoringMetaParameters(){
//    std::vector<std::string> result;
//    extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simulationRawDataContainer[0], result,
//                                                                     MomentsReweighterHelper::metaParameters);
//    return result;
//}
//
//std::vector<std::vector<double> > MomentsReweighterHelper::getValuesOfSimulationParametersIgnoringMetaParameters(){
//    std::vector<std::vector<double> > result;
//    extractValuesOfSimulationParametersIgnoringMetaParameters(simulationRawDataContainer, result,
//                                                              MomentsReweighterHelper::metaParameters);
//    return result;
//}

////Here we do not return a vector but we give it as argument because it is supposed to be prepared!
//void MomentsReweighterHelper::extractAndSetProvidedValuesOfLogZAtSimulatedPoints(std::vector<double>& logZ){
//    if(logZ.size() == 0)
//        throw std::invalid_argument("logZ asked to be set but not allocated!");
//    for(size_t i=0; i<logZ.size(); i++){
//        if(logZ[i] != 0.0)
//            throw std::invalid_argument("logZ asked to be set but already with some non-zero value inside!");
//    }
//    std::map<std::string, double> auxMap;
//    for(int i=0; i<simulationRawDataContainer.getNumberOfDatafiles(); i++){
//        auxMap = simulationRawDataContainer[i].getSimulationParameters();
//        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
//            if(it->first == "logZ")
//                logZ[i] = it->second;
//        }
//    }
//}


//void MomentsReweighterHelper::writeNewConfigurationFileWithMetaparameters(MomentsReweighter reweighter, std::string newConfigFileName){
//    if(newConfigFileName == "")
//        newConfigFileName = "configFileWithLogZ";
//    std::ofstream outputFile;
//    outputFile.open(newConfigFileName.c_str(), std::ofstream::app);
//    if(!outputFile)
//        throw std::runtime_error("Something went wrong opening the file \"" + newConfigFileName + "\"!");
//    outputFile.precision(16);
//    outputFile << "\n\n#===================================================================================\n\n";
//    for(int i=0; i<reweighter.momentsReweighterHelper.simulationRawDataContainer.getNumberOfDatafiles(); i++){
//        outputFile << reweighter.momentsReweighterHelper.simulationRawDataContainer[i].getDatafileName() << "\t";
//        for(size_t j=0; j<reweighter.reweightingParameterNames.size(); j++){
//            outputFile << reweighter.reweightingParameterNames[j] << " " << reweighter.valuesOfSimulationParameters[i][j] << "\t";
//        }
//        for(size_t j=0; j<MomentsReweighterHelper::metaParameters.size(); j++){
//            outputFile << MomentsReweighterHelper::metaParameters[j] << " ";
//            if(MomentsReweighterHelper::metaParameters[j] == "logZ")
//                outputFile << reweighter.logZAtSimulatedPoints[i];
//            else if(MomentsReweighterHelper::metaParameters[j] == "binsize")
//                outputFile << reweighter.momentsReweighterHelper.simulationRawDataContainer[i][0].getNumberOfElements()/numberOfBinsToBeUsed[i];
//            else
//                throw std::runtime_error("Encountered unknown metaparameter writing new configuration file!");
//            outputFile << "\t";
//        }
//            outputFile << "\n";
//    }
//    outputFile << "\n#===================================================================================\n\n";
//    outputFile.close();
//}
//
//void MomentsReweighterHelper::writeNewPointsToFileWithLogZ(MomentsReweighter reweighter, std::string outputFileName){
//    if(boost::filesystem::exists(outputFileName))
//        throw std::invalid_argument("The file \"outputFileName\" already exists! It will not be overwritten, aborting...");
//    std::ofstream outputFile;
//    outputFile.open(outputFileName.c_str());
//    if(!outputFile)
//        throw std::runtime_error("Something went wrong opening the file \"" + outputFileName + "\"!");
//    outputFile.precision(16);
//    for(size_t i=0; i<reweighter.reweightingParameterNames.size(); i++)
//        outputFile << "# " << reweighter.reweightingParameterNames[i] << "\t";
//    outputFile << "logZ\n";
//    for(size_t i=0; i<reweighter.valuesOfNewParameters.size(); i++){
//        for(size_t j=0; j<reweighter.valuesOfNewParameters[i].size(); j++)
//            outputFile << reweighter.valuesOfNewParameters[i][j] << "\t";
//        outputFile << reweighter.logZAtNewPoints[i] << std::endl;
//    }
//    outputFile.close();
//}



/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/

//static void checkCorrectnessOfConfigurationFileForReweighting(SimulationDataContainer simDataCont,
//                                                              const std::vector<std::string> metaPar, int numObs){
//    std::vector<std::string> parNames, auxParNames;
//    std::vector<std::vector<double> > parValues;
//    extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simDataCont[0], parNames, metaPar);
//    int numberOfObservablesToBeReweighted = simDataCont[0].getNumberOfDataSample() - parNames.size();
//    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
//        if(i!=0){
//            extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(simDataCont[i], auxParNames, metaPar);
//            if(auxParNames != parNames)
//                throw std::invalid_argument("Configuration file for Reweighting not valid. Use the same parameter names in each line!");
//        }
//        //In the following two checks is also excluded the unlucky case in which numObs < 0.
//        if((int)parNames.size() > simDataCont[i].getNumberOfDataSample())
//            throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not enough columns!");
//        if(numberOfObservablesToBeReweighted != simDataCont[i].getNumberOfDataSample() - (int)parNames.size())
//            throw std::logic_error("Configuration file for Reweighting not valid. Number of observables in datafiles not coherent!");
//    }
//    extractValuesOfSimulationParametersIgnoringMetaParameters(simDataCont, parValues, metaPar);
//    for(size_t i=1; i<parValues.size(); i++){
//        if(isLastEntryPresentMoreThanOnce(parValues))
//            throw std::logic_error("Files with same reweighting parameters cannot be accumulated, yet!");
//    }
//    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
//        //In the following two checks is also excluded the unlucky case in which numObs < 0.
//        if((int)parNames.size() > simDataCont[i].getNumberOfDataSample())
//            throw std::logic_error("Configuration file for Reweighting not valid. At least one datafile has not enough columns!");
//        if(numObs != simDataCont[i].getNumberOfDataSample() - (int)parNames.size())
//            throw std::logic_error("Configuration file for Reweighting not valid. Number of observables in datafiles not coherent!");
//    }
//}


/*
 * NOTE: In the function getNumberOfBinsToBeUsed we calculate the number
 *       of bins using the given binsize. This is fine, but if
 *       we calculate again the binsize this could be wrong. For example,
 *       consider 1653 and 100 as binsize. This gives [1653/100]=16 bins
 *       but [1653/16]=103 as binsize! That's why we behave differently
 *       in Jackknife and bootstrap.
 */
static void setNumberOfBinsToBeUsedAndEntriesToBeLeftOut(SimulationDataContainer simDataCont, std::vector<int> binsizesToBeUsed, ErrorCalculationMethod errorMethod,
		 	          	  	  	  	  	  	  	  	  	 std::vector<int>& valuesOfNumberOfBins, std::vector<int>& entriesToBeLeftOut)
{
	valuesOfNumberOfBins.clear();

	if(binsizesToBeUsed.size() != (size_t)simDataCont.getNumberOfDatafiles())
        throw std::invalid_argument("Invalid size of binsizesToBeUsed vector in \"setNumberOfBinsToBeUsedAndEntriesToBeLeftOut\" function!");

    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++)
    	valuesOfNumberOfBins.push_back(simDataCont[i][0].getNumberOfElements()/binsizesToBeUsed[i]);

    if(errorMethod == jackknife){
    	valuesOfNumberOfBins = std::vector<int>(simDataCont.getNumberOfDatafiles(), *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end()));
    	entriesToBeLeftOut = simDataCont.getNumberOfEntriesLeftOut(valuesOfNumberOfBins);
    }else if(errorMethod == bootstrap){
    	entriesToBeLeftOut = simDataCont.getNumberOfEntriesLeftOut(binsizesToBeUsed);
    }else
    	throw std::runtime_error("Unknown errorMethod in \"setNumberOfBinsToBeUsedAndEntriesToBeLeftOut\" function!");
}

//static std::vector<int> getNumberOfBinsToBeUsedAndEntriesToBeLeftOut(SimulationDataContainer simDataCont,
//		 	          	  	  	  	  	  	  	  	  	  	  	     const std::vector<std::string> metaPar,
//		 	          	  	  	  	  	  	  	  	  	  	  	     ErrorCalculationMethod errorMethod,
//		 	          	  	  	  	  	  	  	  	  	  	  	     std::vector<int>& entriesToBeLeftOut){
//    std::vector<int> valuesOfBinsizes = extractValuesOfBinsizes(simDataCont, metaPar[1]);
//    std::vector<int> valuesOfNumberOfBins;
//    if(valuesOfBinsizes == std::vector<int>(valuesOfBinsizes.size(), 0))
//        throw std::runtime_error("In the configuration file at least one binsize must be provided!");
//
//    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
//        if(valuesOfBinsizes[i] != 0)
//            valuesOfNumberOfBins.push_back(simDataCont[i][0].getNumberOfElements()/valuesOfBinsizes[i]);
//        else{
//        	if (errorMethod == bootstrap){
//        		if(simDataCont[i].getNumberOfDataSample() < 10)
//        			throw std::invalid_argument("File \"" + simDataCont[i].getDatafileName() + "\" has less than 10 data and the binsize has not been specified! Aborting...");
//        		std::cout << "WARNING: Binsize not provided for file \"" << simDataCont[i].getDatafileName() << "\". Using 10 bins!\n";
//        		valuesOfNumberOfBins.push_back(10);
//        	}
//        }
//    }
//
//    if(errorMethod == jackknife){
//    	std::vector<int> vectorToBeReturned(simDataCont.getNumberOfDatafiles(), *min_element(valuesOfNumberOfBins.begin(), valuesOfNumberOfBins.end()));
//    	entriesToBeLeftOut = simDataCont.getNumberOfEntriesLeftOut(vectorToBeReturned);
//    	return vectorToBeReturned;
//    }else{ // errorMethod == bootstrap
//    	entriesToBeLeftOut = simDataCont.getNumberOfEntriesLeftOut(valuesOfBinsizes);
//    	return valuesOfNumberOfBins;
//    }
//}


/*
 * In the following function, we go through the configuration file and we check on each line
 * whether binsize is provided. If it is given  we check that its value makes sense and we store it,
 * otherwise we manually store zero to take trace that for such a file the binsize was not given.
 */
//static std::vector<int> extractValuesOfBinsizes(SimulationDataContainer simDataCont, std::string labelOfBinsize){
//    std::map<std::string, double> auxMap;
//    std::vector<int> auxVector;
//    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
//        bool found=false;
//        auxMap = simDataCont[i].getSimulationParameters();
//        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
//            if(it->first == labelOfBinsize){
//                if(it->second <= 0)
//                    throw std::invalid_argument("At least one non positive binsize has been provided!");
//                else{
//                    auxVector.push_back((int)it->second);
//                    found=true;
//                }
//                break;
//            }
//        }
//        if(found == false)
//            auxVector.push_back(0);
//    }
//    return auxVector;
//}


//static void extractNamesOfParametersFromSimulationDataIgnoringMetaParameters(SimulationData simData,
//                                                                             std::vector<std::string>& parNames,
//                                                                             const std::vector<std::string>& metaParameters){
//    std::map<std::string, double> auxMap;
//    auxMap = simData.getSimulationParameters();
//    parNames.clear();
//    for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
//        if(find(metaParameters.begin(), metaParameters.end(), it->first) == metaParameters.end())
//            parNames.push_back(it->first);
//    }
//}
//
//static void extractValuesOfSimulationParametersIgnoringMetaParameters(SimulationDataContainer simDataCont,
//                                                                      std::vector<std::vector<double> >& valuesOfSimPar,
//                                                                      const std::vector<std::string>& metaParameters){
//    std::map<std::string, double> auxMap;
//    std::vector<double> auxVector;
//    valuesOfSimPar.clear();
//    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
//        auxMap = simDataCont[i].getSimulationParameters();
//        for(std::map<std::string, double>::iterator it=auxMap.begin(); it!=auxMap.end(); it++){
//            if(find(metaParameters.begin(), metaParameters.end(), it->first) == metaParameters.end())
//                auxVector.push_back(it->second);
//        }
//        valuesOfSimPar.push_back(auxVector);
//        auxVector.clear();
//    }
//}

/*
 * ATTENTION: The access operator [] of map, even if used only to retrieve the value, modifies the
 *            map inserting a new default initialized element, when the asked element is not found.
 *            Here we use this fact to print zero binsizes for files for which binsize has not been
 *            given, but then at the end of the function, the object SimulationDataContainer will
 *            be modified and this modification has not to be propagated throughout the code. Hence
 *            here we pass ON PURPOSE simDataCont by value and it has NOT to be changed to a const
 *            reference or worse to a reference.
 */
static void printBinsizesActuallyUsed(SimulationDataContainer simDataCont, std::vector<int> numberOfBinsToBeUsed){
    size_t maxLengthDataFilename = simDataCont[0].getDatafileName().length();
    for(int i=1; i<simDataCont.getNumberOfDatafiles(); i++){
        if(simDataCont[i].getDatafileName().length() > maxLengthDataFilename)
            maxLengthDataFilename = simDataCont[1].getDatafileName().length();
    }
    std::cout << "\n";
    for(size_t i=0; i<maxLengthDataFilename+50; i++)
        std::cout << "=";
    std::cout << "\n";

    for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
        std::cout << simDataCont[i].getDatafileName();
        std::cout << "   Given binsize = " << simDataCont[i].getSimulationParameters()["binsize"];
        std::cout << "    Used binsize = " << simDataCont[i][0].getNumberOfElements()/numberOfBinsToBeUsed[i] << "\n";
    }

    for(size_t i=0; i<maxLengthDataFilename+50; i++)
        std::cout << "=";
    std::cout << "\n\n";
}


//static bool isLastEntryPresentMoreThanOnce(std::vector<std::vector<double> > parValues){
//    return (parValues.size() > 1) && (std::find(parValues.begin(), parValues.end()-1, parValues.back()) != parValues.end()-1);
//}



//static std::pair<ErrorCalculationMethod, std::string> getErrorCalculationMethod(std::string errorMethodIn)
//{
//	std::vector<std::string> resultOfSplit;
//	boost::split(resultOfSplit, errorMethodIn, boost::is_any_of(" _-,."));
//	if(resultOfSplit.size() > 2){
//		throw std::invalid_argument("The error method \"" + errorMethodIn + "\" contains more than once any of the char \" _-,.\"! Aborting...");
//	}
//	errorMethodIn=resultOfSplit[0];
//	boost::algorithm::to_lower(errorMethodIn);
//	std::map<std::string, ErrorCalculationMethod> m;
//	m["jackknife"] = jackknife;
//	m["jack"] = jackknife;
//	m["bootstrap"] = bootstrap;
//	m["boot"] = bootstrap;
//
//	ErrorCalculationMethod errorMethodOut = m[errorMethodIn];
//	if(errorMethodOut) {
//		if(errorMethodOut == jackknife){
//			if(resultOfSplit.size() == 2)
//				throw std::invalid_argument("The Jackknife error method does not require any further information!! Aborting...");
//			return std::pair<ErrorCalculationMethod, std::string>(errorMethodOut, "unset");
//		}else if(errorMethodOut == bootstrap){
//			return (resultOfSplit.size() == 2) ? std::pair<ErrorCalculationMethod, std::string>(errorMethodOut, resultOfSplit[1])
//					                           : std::pair<ErrorCalculationMethod, std::string>(errorMethodOut, "100");
//		}else{
//			throw std::logic_error("Something magic seemed to be happened here, since this case should never be entered!! Aborting...");
//		}
//	} else {
//			throw std::invalid_argument("The error method \"" + errorMethodIn + "\" is not valid! Aborting...");
//	}
//}
