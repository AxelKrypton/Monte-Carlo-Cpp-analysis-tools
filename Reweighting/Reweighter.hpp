#ifndef REWEIGHTER_HPP_
#define REWEIGHTER_HPP_

#include "SimulationDataContainer.hpp"
#include "MomentsReweightingDataHandler.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"




class RawDataForReweightingAndMetainformation {
public:
	SimulationDataContainer rawData;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters;
    std::vector<std::vector<double> > valuesOfSimulationParametersIgnoringMetaParameters;
    std::vector<int> numberOfBinsToBeUsed;
};




class ReweighterIO {
	friend class Reweighter;
public:
	ReweighterIO() = delete;
	/*
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	ReweighterIO(LqcdReweightingParameters parameters);
private:
	SimulationDataContainer readFromFileDataContainer;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters;
	std::vector<std::vector<double> > valuesOfSimulationParametersIgnoringMetaParameters;
	ErrorCalculationMethod errorMethod;
	std::unique_ptr<int> bootstrapNumber;

};






class Reweighter {
public:
	Reweighter() = delete;
	/*
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	Reweighter(LqcdReweightingParameters parameters);
	std::vector<std::vector<Observables> > getReweightedObservables();
	//void writeReweightingResultsToFile();
private:
	ReweighterIO reweighterIO;
	std::vector<std::string> quantitiesToBeReweighted;
	std::vector<std::vector<Observables> > observablesAtNewPoints;
};
















#endif /* REWEIGHTER_HPP_ */
