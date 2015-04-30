#ifndef REWEIGHTER_HPP_
#define REWEIGHTER_HPP_

#include "SimulationDataContainer.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

class Reweighter {
public:
	Reweighter() = delete;
	/*
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	Reweighter(LqcdReweightingParameters parameters);
	std::vector<std::vector<Observables> > getReweightedObservables();
	void writeReweightingResultsToFile();
private:
	SimulationDataContainer readFromFileDataContainer;
	std::vector<std::vector<Observables> > observablesAtNewPoints;
	std::vector<std::string> quantitiesToBeReweighted;
};




#endif /* REWEIGHTER_HPP_ */
