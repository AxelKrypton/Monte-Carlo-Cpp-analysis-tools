#ifndef REWEIGHTER_HPP_
#define REWEIGHTER_HPP_

#include "SimulationDataContainer.hpp"
#include "ReweighterIO.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

struct RawDataForReweightingAndMetainformation {
	//Information coming from the configuration file
	SimulationDataContainer rawData;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters;
    std::vector<std::vector<realFloat> > valuesOfSimulationParametersIgnoringMetaParameters;
    std::vector<realFloat> valuesOfSpecifiedLogZ;
    //Information coming from the user
    std::vector<std::pair<realFloat, realFloat> >  newRangesOfParameters;
    std::vector<unsigned int>  newNumberOfPointsOfParameters;
    bool useSimulatedPointsAsNewPoints;
    bool isMeanKnownToBeZero;
    realFloat precisionToCalculateLogZ;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    ErrorCalculationMethod errorMethod;
    std::shared_ptr<int> bootstrapNumber;
    //Information deriving from some operations done in the program
    unsigned int maximumMomentNeededOverall; //this could not coincide with the max_element of the vector momentsToBeReweighted in the case of multiple columns per observable!!
    std::vector<unsigned int> momentsToBeReweighted;
    std::vector<int> binsizesToBeUsedForBinning;
	bool reweightProbabilityDistributions;
	realFloat binsizeForProbabilityDistribution;
};

struct ReweightingProcedure {
	std::vector<unsigned int> momentsToBeReweighted;
	std::vector<int> binsizesToBeUsed;
	std::vector<std::string> quantitiesConsidered;
	bool reweightProbabilityDistributions;
};


class Reweighter {
	friend class ReweighterTester;
public:
	Reweighter() = delete;
	/*
	 * TODO:
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	Reweighter(LqcdReweightingParameters parameters);
	std::vector<std::vector<realFloat> > getValuesOfNewParameters();
	std::vector<std::vector<Observables> > getReweightedObservables();
	std::vector<std::vector<std::map<std::string, DataSample> > > getReweightedObservablesEstimators();
private:
	ReweighterIO reweighterIO;
	std::vector<std::string> quantitiesToBeReweighted;
	unsigned int maximumMomentNeededOverall;
    std::vector<std::pair<realFloat, realFloat> >  newRangesOfParameters;
    std::vector<unsigned int>  newNumberOfPointsOfParameters;
    bool useSimulatedPointsAsNewPoints;
    std::vector<std::vector<realFloat> > valuesOfNewParameters;
	realFloat precisionOfIterativeProcedureToCalculateLogZ;
	std::vector<std::vector<Observables> > observablesAtNewPoints;
	std::unique_ptr<std::vector<std::vector<std::map<std::string, DataSample> > > > observablesEstimatorsAtNewPoints; //e.g. [newPoint][obsInFile][Skewness::observableName]
	//The following methods are here in order to be tested one by one (in principle they could be static function in the .cpp file)
	Reweighter(std::initializer_list<std::string>);
	static LqcdReweightingParameters createLqcdParameters(std::initializer_list<std::string>);
	std::vector<ReweightingProcedure> getReweightingProceduresToBePerformed();
	RawDataForReweightingAndMetainformation getRawDataForReweightingAndMetainformation(std::vector<unsigned int> momentsToBeReweighted, std::vector<int> binsizesToBeUsed, bool reweightProbabilityDistribution);
};



class ReweighterTester{
public:
	ReweighterTester(std::initializer_list<std::string> options, bool reweight = false)
		: reweighter((reweight ? Reweighter(Reweighter::createLqcdParameters(options)) : Reweighter(options))){};
	std::vector<ReweightingProcedure> getReweightingProceduresToBePerformed(){ return reweighter.getReweightingProceduresToBePerformed(); };
	std::vector<std::vector<Observables> > getReweightedObservables(){ return reweighter.getReweightedObservables(); };
	//The following method is used to test MomentsReweighter(Helper) class
	RawDataForReweightingAndMetainformation getRawDataForReweightingAndMetainformation(std::vector<unsigned int> momentsToBeReweighted, std::vector<int> binsizesToBeUsed, bool reweightProbabilityDistribution = true)
	{
		return reweighter.getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed, reweightProbabilityDistribution);
	}
	static void printRawDataForReweightingAndMetainformation(RawDataForReweightingAndMetainformation rawDataAndInfo)
	{
		std::cout << "----------------------------------------------------------------------\n";
		std::cout << "- rawData: " << rawDataAndInfo.rawData.getNumberOfDatafiles() << " files\n";
		for(int i=0; i<rawDataAndInfo.rawData.getNumberOfDatafiles(); i++)
			std::cout << "  - file " << i << " has " << rawDataAndInfo.rawData[i][0].getNumberOfElements() << " lines\n";
		std::cout << "- namesOfParametersIgnoringMetaParameters: size " << rawDataAndInfo.namesOfParametersIgnoringMetaParameters.size() << "  ->  ";
		for(auto i: rawDataAndInfo.namesOfParametersIgnoringMetaParameters) std::cout << i << " - ";
		std::cout << "\b\b\033[K\n";
		std::cout << "- valuesOfSimulationParametersIgnoringMetaParameters: size " << rawDataAndInfo.valuesOfSimulationParametersIgnoringMetaParameters.size() << "  ->  ";
		for(size_t i=0; i<rawDataAndInfo.valuesOfSimulationParametersIgnoringMetaParameters.size(); i++){
			for(size_t j=0; j<rawDataAndInfo.valuesOfSimulationParametersIgnoringMetaParameters[i].size(); j++)
				std::cout << rawDataAndInfo.valuesOfSimulationParametersIgnoringMetaParameters[i][j] << " , ";
			std::cout << "\b\b\033[K| ";
		}
		std::cout << "\b\b\033[K\n";
		std::cout << "- valuesOfSpecifiedLogZ: "; for(auto j: rawDataAndInfo.valuesOfSpecifiedLogZ) std::cout << j << " "; std::cout << "\n";
		std::cout << "- newRangesOfParameters: "; for(auto j: rawDataAndInfo.newRangesOfParameters) std::cout << j.first << " <-> " << j.second << " | "; std::cout << "\b\b\033[K\n";
		std::cout << "- newNumberOfPointsOfParameters: "; for(auto j: rawDataAndInfo.newNumberOfPointsOfParameters) std::cout << j << " - "; std::cout << "\b\b\033[K\n";
		std::cout << "- isMeanKnownToBeZero: "<< rawDataAndInfo.isMeanKnownToBeZero << "\n";
		std::cout << "- precisionToCalculateLogZ: "<< rawDataAndInfo.precisionToCalculateLogZ << "\n";
		std::cout << "- errorMethod: "<< rawDataAndInfo.errorMethod << "\n";
		if(rawDataAndInfo.bootstrapNumber)
			std::cout << "- bootstrapNumber: "<< *(rawDataAndInfo.bootstrapNumber) << "\n";
		std::cout << "- maximumMomentNeededOverall: "<< rawDataAndInfo.maximumMomentNeededOverall << "\n";
		std::cout << "- momentsToBeReweighted: "; for(auto j: rawDataAndInfo.momentsToBeReweighted) std::cout << j << " "; std::cout << "\n";
		std::cout << "- binsizesToBeUsed: "; for(auto j: rawDataAndInfo.binsizesToBeUsedForBinning) std::cout << j << " - "; std::cout << "\b\b\033[K\n";
		std::cout << "----------------------------------------------------------------------\n";
	}
private:
	Reweighter reweighter;
};












#endif /* REWEIGHTER_HPP_ */
