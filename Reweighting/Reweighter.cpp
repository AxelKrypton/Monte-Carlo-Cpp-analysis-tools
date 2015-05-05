#include "Reweighter.hpp"

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters);
static std::vector<int> getBinsizesToBeUsedBasedOnObservable(std::string, bool, std::vector<Binsizes>);
static std::initializer_list<unsigned int> getNeededMomentsBasedOnObservableName(std::string, bool);
static std::vector<ReweightingProcedure> getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(std::map<std::string, std::vector<int> >, bool);
static std::vector<unsigned int> getUnionOfVectors(std::vector<unsigned int>, std::vector<unsigned int>);
static LqcdReweightingParameters createLqcdParameters(std::initializer_list<std::string>);

/*****************************************************************************************/

Reweighter::Reweighter(std::initializer_list<std::string> options) : reweighterIO(createLqcdParameters(options))
{
	quantitiesToBeReweighted = getQuantitiesToBeReweighted(createLqcdParameters(options));
	std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
	for(auto i: quantitiesToBeReweighted)
		std::cout << i << " - ";
	std::cout << "\n";
}

Reweighter::Reweighter(LqcdReweightingParameters parameters) : reweighterIO(parameters)
{
	quantitiesToBeReweighted = getQuantitiesToBeReweighted(parameters);
	//...
}

std::vector<std::vector<Observables> > Reweighter::getReweightedObservables(){
	return observablesAtNewPoints;
}







/******************************************** PRIVATE METHODS *************************************************/

/*
 * The return type of this function looks complicate. It is intended to work as follows.
 *   - The outermost vector takes trace of the reweighting procedures to be done
 *   - For each reweighting procedure we have a pair:
 *       - The first entry are the moments to be reweighted using the MomentsReweighter
 *       - The second entry is the binsizes to be used in the reweighting procedure
 */
std::vector<ReweightingProcedure> Reweighter::getReweightingProceduresToBePerformed(){
	std::map<std::string, std::vector<int> > binsizesToBeUsedPerQuantityToBeReweighted;
	for(auto quantity : quantitiesToBeReweighted){
		std::vector<int> binsizesToBeUsedBasedOnObservable = getBinsizesToBeUsedBasedOnObservable(quantity, reweighterIO.isMeanKnownToBeZero, reweighterIO.valuesOfSpecifiedBinsizes);
		if(!binsizesToBeUsedBasedOnObservable.empty()) //it is empty for mean if the mean is known to be zero
			binsizesToBeUsedPerQuantityToBeReweighted[quantity] = binsizesToBeUsedBasedOnObservable;
	}
	return getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(binsizesToBeUsedPerQuantityToBeReweighted, reweighterIO.isMeanKnownToBeZero);
}




/******************************************** STATIC FUNCTIONS *************************************************/

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters parameters){
	std::vector<std::string> quantities;
	if(!parameters.getDeactivateReweightingForMean())
		quantities.push_back(Mean::observableName);
	if(!parameters.getDeactivateReweightingForVariance())
		quantities.push_back(Variance::observableName);
	if(!parameters.getDeactivateReweightingForSkewness())
		quantities.push_back(Skewness::observableName);
	if(!parameters.getDeactivateReweightingForBinder())
		quantities.push_back(BinderCumulant::observableName);
	return quantities;
}

static std::vector<int> getBinsizesToBeUsedBasedOnObservable(std::string obsName, bool isMeanZero, std::vector<Binsizes> binsizesFromConfigFile){
	std::vector<int> result;
	if(obsName == Mean::observableName && isMeanZero)
		return result; //Particular case, return empty object, no reweighting to be done!!
	else{
		for(size_t i=0; i<binsizesFromConfigFile.size(); i++){
			std::vector<int> binsizes = binsizesFromConfigFile[i].at( getNeededMomentsBasedOnObservableName(obsName, isMeanZero) );
			result.push_back(*std::max_element(binsizes.begin(),binsizes.end()));
		}
		return result;
	}
}

static std::initializer_list<unsigned int> getNeededMomentsBasedOnObservableName(std::string obsName, bool isMeanZero){
	if(obsName == Mean::observableName)
		return isMeanZero ? Mean::neededMomentsWithZeroMean : Mean::neededMoments;
	else if(obsName == Variance::observableName)
		return isMeanZero ? Variance::neededMomentsWithZeroMean : Variance::neededMoments;
	else if(obsName == Skewness::observableName)
		return isMeanZero ? Skewness::neededMomentsWithZeroMean : Skewness::neededMoments;
	else if(obsName == BinderCumulant::observableName)
		return isMeanZero ? BinderCumulant::neededMomentsWithZeroMean : BinderCumulant::neededMoments;
	else
		throw std::invalid_argument("Unknown observable in \"getNeededMomentsBasedOnObservableName\" function!");
}


/*
 * ATTENTION: Here the map binsizesPerQuantity is modified inside the function! It is passed BY VALUE on purpose!!!
 */
static std::vector<ReweightingProcedure> getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(std::map<std::string, std::vector<int> > binsizesPerQuantity, bool isMeanZero){
	std::vector<ReweightingProcedure> reweightingProcedures;
	while(!binsizesPerQuantity.empty()){
		ReweightingProcedure reweightingProcedure;
		reweightingProcedure.binsizesToBeUsed = binsizesPerQuantity.begin()->second;
		reweightingProcedure.momentsToBeReweighted = getNeededMomentsBasedOnObservableName(binsizesPerQuantity.begin()->first, isMeanZero);
		reweightingProcedure.quantitiesConsidered.push_back(binsizesPerQuantity.begin()->first);
		for(std::map<std::string, std::vector<int> >::iterator it = ++(binsizesPerQuantity.begin()); it!=binsizesPerQuantity.end(); it++){
			if(it->second == binsizesPerQuantity.begin()->second){
				reweightingProcedure.momentsToBeReweighted = getUnionOfVectors(reweightingProcedure.momentsToBeReweighted,
						                                                       getNeededMomentsBasedOnObservableName(it->first, isMeanZero));
				reweightingProcedure.quantitiesConsidered.push_back(it->first);
				it = binsizesPerQuantity.erase(it);
				it--; //necessary since erase return an iterator to the following element in the map.
			}
		}
		reweightingProcedures.push_back(reweightingProcedure);
		binsizesPerQuantity.erase(binsizesPerQuantity.begin());
	}
	return reweightingProcedures;
}

static std::vector<unsigned int> getUnionOfVectors(std::vector<unsigned int> firstVec, std::vector<unsigned int> secondVec){
	std::vector<unsigned int> result(firstVec.size()+secondVec.size());
	std::sort(firstVec.begin(), firstVec.end());
	std::sort(secondVec.begin(), secondVec.end());
	std::vector<unsigned int>::iterator it = std::set_union(firstVec.begin(), firstVec.end(), secondVec.begin(), secondVec.end(), result.begin());
	result.resize(it-result.begin());
	return result;
}


static LqcdReweightingParameters createLqcdParameters(std::initializer_list<std::string> options)
{
	std::vector<const char*> tmp;
	tmp.push_back(std::string("foo").c_str());
	for(auto i: options)
		tmp.push_back(i.c_str());
	int numberOfArguments = (int)tmp.size();
	LqcdReweightingParameters parameters(numberOfArguments, tmp.data());
	return parameters;
}
