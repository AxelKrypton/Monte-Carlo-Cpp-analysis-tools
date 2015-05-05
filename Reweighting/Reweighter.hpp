#ifndef REWEIGHTER_HPP_
#define REWEIGHTER_HPP_

#include "SimulationDataContainer.hpp"
#include "MomentsReweightingDataHandler.hpp"
#include "ReweighterIO.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"




struct RawDataForReweightingAndMetainformation {
	SimulationDataContainer rawData;
	std::vector<std::string> namesOfParametersIgnoringMetaParameters;
    std::vector<std::vector<double> > valuesOfSimulationParametersIgnoringMetaParameters;
    std::vector<int> numberOfBinsToBeUsed;
    std::vector<double> valuesOfSpecifiedLogZ;
    bool isMeanKnownToBeZero;
};

struct ReweightingProcedure {
	std::vector<unsigned int> momentsToBeReweighted;
	std::vector<int> binsizesToBeUsed;
	std::vector<std::string> quantitiesConsidered;
};


class Reweighter {
	friend class ReweighterTester;
public:
	Reweighter() = delete;
	/*
	 * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
	 *            case LqcdReweightingParameters. It is temporary in order to make everything work!
	 */
	Reweighter(LqcdReweightingParameters parameters);
	std::vector<std::vector<Observables> > getReweightedObservables();
private:
	ReweighterIO reweighterIO;
	std::vector<std::string> quantitiesToBeReweighted;
	std::vector<std::vector<Observables> > observablesAtNewPoints;
	//The following methods are here in order to be tested one by one (in principle they could be static function in the .cpp file)
	Reweighter(std::initializer_list<std::string>);
	std::vector<ReweightingProcedure> getReweightingProceduresToBePerformed();
};
















#endif /* REWEIGHTER_HPP_ */
