#ifndef REWEIGHTERIO_HPP_
#define REWEIGHTERIO_HPP_

#include "SimulationDataContainer.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"


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


#endif /* REWEIGHTERIO_HPP_ */
