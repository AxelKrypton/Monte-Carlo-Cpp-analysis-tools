#include <fstream>
#include <boost/filesystem.hpp>
#include "MomentsReweighterHelper.hpp"
#include "MomentsReweighter.hpp"
#include "SimulationData.hpp"
#include "../dataAnalysisUtilities/binnedDataSample.hpp"

static void setNumberOfBinsToBeUsedAndEntriesToBeLeftOut(SimulationDataContainer, std::vector<int>, ErrorCalculationMethod, std::vector<int>&, std::vector<int>&);
static void printBinsizesActuallyUsed(SimulationDataContainer, std::vector<int>);

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
	  columnsToBeReweightedUsingMultipleColumns(rawDataForReweightingAndMetainformation.columnsToBeReweightedUsingMultipleColumns),
	  momentsToBeReweighted(rawDataForReweightingAndMetainformation.momentsToBeReweighted),
	  maximumMomentNeededOverall(rawDataForReweightingAndMetainformation.maximumMomentNeededOverall),
	  errorMethod(rawDataForReweightingAndMetainformation.errorMethod), bootstrapNumber(rawDataForReweightingAndMetainformation.bootstrapNumber)
{
	std::vector<int> entriesToBeCutFromRawData;
	setNumberOfBinsToBeUsedAndEntriesToBeLeftOut(simulationRawDataContainer, rawDataForReweightingAndMetainformation.binsizesToBeUsed, errorMethod,
												 numberOfBinsToBeUsed, entriesToBeCutFromRawData);
	//Evaluate central moments per data and append them to the raw data container
	if(momentsToBeReweighted.empty())
		throw std::logic_error("MomentsReweighterHelper asked to be built without any moment to be reweighted! Aborting...");
	if(!columnsToBeReweightedUsingMultipleColumns.empty()){
		std::sort(columnsToBeReweightedUsingMultipleColumns.begin(), columnsToBeReweightedUsingMultipleColumns.end());
		for(size_t i=1; i<columnsToBeReweightedUsingMultipleColumns.size(); i++){
			if(columnsToBeReweightedUsingMultipleColumns[i]-columnsToBeReweightedUsingMultipleColumns[i-1] < maximumMomentNeededOverall)
				throw std::invalid_argument("obsToBeRewUsingMultipleColumns contains columns too close (distance<" + std::to_string(maximumMomentNeededOverall) + ")!");
		}
		unsigned int numberOfColumnsWithObservables = simulationRawDataContainer[0].getNumberOfDataSample() - namesOfParametersIgnoringMetaParameters.size();
		if(numberOfColumnsWithObservables - columnsToBeReweightedUsingMultipleColumns.back() + 1 < maximumMomentNeededOverall)
			throw std::invalid_argument("Not enough columns to be used for single observable (in col. " + std::to_string(columnsToBeReweightedUsingMultipleColumns.back()) + ")!");
	}

	//Here I set the number of "real" observables given as input (neglecting the multiple columns)
	numberOfObservablesGivenAsInput = simulationRawDataContainer[0].getNumberOfDataSample() - namesOfParametersIgnoringMetaParameters.size();

	std::cout << "Obs. given as input counting multiple = " << numberOfObservablesGivenAsInput << "\n";

	numberOfObservablesGivenAsInput -= columnsToBeReweightedUsingMultipleColumns.size()*(maximumMomentNeededOverall-1); //neglect multiple columns (count one column only)

	std::cout << "Obs. given as input NOT counting multiple = " << numberOfObservablesGivenAsInput << "\n";

	simulationRawDataContainer = simulationRawDataContainer.buildAndGetMomentsPerData(momentsToBeReweighted, namesOfParametersIgnoringMetaParameters.size(),
																					  columnsToBeReweightedUsingMultipleColumns, maximumMomentNeededOverall);
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


/*****************************************************************************************/
/******************************* STATIC FUNCTIONS ****************************************/
/*****************************************************************************************/


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

