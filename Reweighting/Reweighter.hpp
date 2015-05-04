#ifndef REWEIGHTER_HPP_
#define REWEIGHTER_HPP_

#include "SimulationDataContainer.hpp"
#include "MomentsReweightingDataHandler.hpp"
#include "ReweighterIO.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"




class RawDataForReweightingAndMetainformation {
public:
	SimulationDataContainer rawData;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters;
    std::vector<std::vector<double> > valuesOfSimulationParametersIgnoringMetaParameters;
    std::vector<int> numberOfBinsToBeUsed;
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
