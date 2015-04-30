#include "Reweighter.hpp"

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters);

/*****************************************************************************************/

Reweighter::Reweighter(LqcdReweightingParameters parameters) : readFromFileDataContainer(parameters.getInputfile())
{
	quantitiesToBeReweighted = getQuantitiesToBeReweighted(parameters);




}



std::vector<std::vector<Observables> > Reweighter::getReweightedObservables(){
	return observablesAtNewPoints;
}








/******************************************** STATIC FUNCTIONS *************************************************/

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters parameters){
	std::vector<std::string> quantities;
	if(!parameters.getDeactivateReweightingForMean())
		quantities.push_back("MEAN");
	if(!parameters.getDeactivateReweightingForVariance())
		quantities.push_back("VARIANCE");
	if(!parameters.getDeactivateReweightingForSkewness())
		quantities.push_back("SKEWNESS");
	if(!parameters.getDeactivateReweightingForBinder())
		quantities.push_back("BINDER CUMULANT");
	return quantities;
}
