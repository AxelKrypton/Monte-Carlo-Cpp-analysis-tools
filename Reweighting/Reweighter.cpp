#include "Reweighter.hpp"
#include "MomentsReweighter.hpp"

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters);
static std::vector<int> getBinsizesToBeUsedBasedOnObservable(std::string, bool, std::vector<Binsizes>);
static std::initializer_list<unsigned int> getNeededMomentsBasedOnObservableName(std::string, bool);
static std::vector<ReweightingProcedure> getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(std::map<std::string, std::vector<int> >, bool);
static std::vector<unsigned int> getUnionOfVectors(std::vector<unsigned int>, std::vector<unsigned int>);
static unsigned int getMaximumMomentToBeReweighted(const std::vector<ReweightingProcedure>&);
static void printInformationAboutReweightingProcedure(const ReweightingProcedure&);
static void checkSizesOfMomentsAndMomentsEstimators(std::vector<std::vector<Moments> >, std::vector<std::vector<MomentsEstimators> >, size_t, size_t);
static void setObservablesAtNewPointsFromMomentsAndMomentEstimators(std::vector<std::vector<Observables> >&, bool, ErrorCalculationMethod, std::vector<std::vector<Moments> >,
																	std::vector<std::vector<MomentsEstimators> >, std::vector<std::string>, const std::vector<unsigned int>&,
																	const unsigned int, const unsigned int);
static bool isObservableToBeEvaluatedUsingMultipleColumns(const unsigned int, const std::vector<unsigned int>&, const unsigned int, const unsigned int);
static void setMeanToZeroAtNewPoints(std::vector<std::vector<Observables> >&);

/*****************************************************************************************/

Reweighter::Reweighter(std::initializer_list<std::string> options) : reweighterIO(createLqcdParameters(options))
{
	LqcdReweightingParameters parameters = createLqcdParameters(options);
	quantitiesToBeReweighted = getQuantitiesToBeReweighted(parameters);
	precisionOfIterativeProcedureToCalculateLogZ = parameters.getWeightPrecision();
	newNumberOfPointsOfParameters = {parameters.getNumberOfNewBetaPoints()}; //TODO: Use method of class ReweightingParameters to be implemented!!
	newRangesOfParameters = {std::make_pair(parameters.getNewBetaRange_low(), parameters.getNewBetaRange_high())}; //TODO: Use method of class ReweightingParameters to be implemented!!
	maximumMomentNeededOverall = 4;//Hard coded for tests, TODO: think a better way
	std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
	for(auto i: quantitiesToBeReweighted)
		std::cout << i << " - ";
	std::cout << "\n";
	std::cout << "reweighterIO.valuesOfSpecifiedLogZ.size() = " << reweighterIO.valuesOfSpecifiedLogZ.size() << "  -> ";
	for(auto i: reweighterIO.valuesOfSpecifiedLogZ)
		std::cout << i << " ";
	std::cout << "\n";
}

Reweighter::Reweighter(LqcdReweightingParameters parameters) : reweighterIO(parameters), quantitiesToBeReweighted(getQuantitiesToBeReweighted(parameters)),
															   precisionOfIterativeProcedureToCalculateLogZ(parameters.getWeightPrecision())
{
	newNumberOfPointsOfParameters = {parameters.getNumberOfNewBetaPoints()}; //TODO: Use method of class ReweightingParameters to be implemented!!
	newRangesOfParameters = {std::make_pair(parameters.getNewBetaRange_low(), parameters.getNewBetaRange_high())}; //TODO: Use method of class ReweightingParameters to be implemented!!
	std::vector<ReweightingProcedure> reweightingProceduresToBePerformed = getReweightingProceduresToBePerformed();
	maximumMomentNeededOverall = getMaximumMomentToBeReweighted(reweightingProceduresToBePerformed);
	//Before setting the observables, we have to reserve the correct amount of memory
	const size_t numberOfNewPoints = std::accumulate(newNumberOfPointsOfParameters.begin(), newNumberOfPointsOfParameters.end(), 1, std::multiplies<unsigned int>());
	const size_t numberOfObservablesInFiles = reweighterIO.readFromFileDataContainer[0].getNumberOfDataSample() - reweighterIO.namesOfParametersIgnoringMetaParameters.size()
											- reweighterIO.columnsToBeReweightedUsingMultipleColumns.size()*(maximumMomentNeededOverall-1); //neglect multiple columns (count one column only)
	observablesAtNewPoints = std::vector<std::vector<Observables>>(numberOfNewPoints, std::vector<Observables>(numberOfObservablesInFiles, Observables()));

	std::cout << "maximumMomentNeededOverall = " << maximumMomentNeededOverall << "\n";
	std::cout << "numberOfNewPoints = " << numberOfNewPoints << "\n";
	std::cout << "numberOfObservablesInFiles = " << numberOfObservablesInFiles << "\n";
	std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
	for(auto i: quantitiesToBeReweighted)
		std::cout << i << " - ";
	std::cout << "\n";

	for(auto rewProc: reweightingProceduresToBePerformed){
		printInformationAboutReweightingProcedure(rewProc);
		MomentsReweighter momentsReweighter(getRawDataForReweightingAndMetainformation(rewProc.momentsToBeReweighted, rewProc.binsizesToBeUsed));
		if(valuesOfNewParameters.empty()) valuesOfNewParameters = momentsReweighter.getValuesOfNewParameters();
		std::vector<std::vector<Moments> > momentsAtNewPoints = momentsReweighter.getMomentsAtNewPoints();
		std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints = momentsReweighter.getMomentsEstimatorsAtNewPoints();
		checkSizesOfMomentsAndMomentsEstimators(momentsAtNewPoints, momentsEstimatorsAtNewPoints, numberOfNewPoints, numberOfObservablesInFiles);
		setObservablesAtNewPointsFromMomentsAndMomentEstimators(observablesAtNewPoints, reweighterIO.isMeanKnownToBeZero, reweighterIO.errorMethod,
																momentsAtNewPoints, momentsEstimatorsAtNewPoints, rewProc.quantitiesConsidered,
																reweighterIO.columnsToBeReweightedUsingMultipleColumns,
																reweighterIO.namesOfParametersIgnoringMetaParameters.size(), maximumMomentNeededOverall);
	}

	//Set manually mean to zero if mean is known to be zero and MEAN is asked
	if(reweighterIO.isMeanKnownToBeZero && find(quantitiesToBeReweighted.begin(), quantitiesToBeReweighted.end(), Mean::observableName) != quantitiesToBeReweighted.end())
		setMeanToZeroAtNewPoints(observablesAtNewPoints);
}


std::vector<std::vector<double> > Reweighter::getValuesOfNewParameters(){
	return valuesOfNewParameters;
}

std::vector<std::vector<Observables> > Reweighter::getReweightedObservables(){
	return observablesAtNewPoints;
}


/******************************************** PRIVATE METHODS *************************************************/

std::vector<ReweightingProcedure> Reweighter::getReweightingProceduresToBePerformed(){
	std::map<std::string, std::vector<int> > binsizesToBeUsedPerQuantityToBeReweighted;
	for(auto quantity : quantitiesToBeReweighted){
		std::vector<int> binsizesToBeUsedBasedOnObservable = getBinsizesToBeUsedBasedOnObservable(quantity, reweighterIO.isMeanKnownToBeZero, reweighterIO.valuesOfSpecifiedBinsizes);
		if(!binsizesToBeUsedBasedOnObservable.empty()) //it is empty for mean if the mean is known to be zero
			binsizesToBeUsedPerQuantityToBeReweighted[quantity] = binsizesToBeUsedBasedOnObservable;
	}
	return getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(binsizesToBeUsedPerQuantityToBeReweighted, reweighterIO.isMeanKnownToBeZero);
}

RawDataForReweightingAndMetainformation Reweighter::getRawDataForReweightingAndMetainformation(std::vector<unsigned int> momentsToBeReweighted, std::vector<int> binsizesToBeUsed){
	return {reweighterIO.readFromFileDataContainer, reweighterIO.namesOfParametersIgnoringMetaParameters,
			reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters, reweighterIO.valuesOfSpecifiedLogZ,
			newRangesOfParameters, newNumberOfPointsOfParameters,
			reweighterIO.isMeanKnownToBeZero, precisionOfIterativeProcedureToCalculateLogZ,
			reweighterIO.columnsToBeReweightedUsingMultipleColumns, reweighterIO.errorMethod,
			reweighterIO.bootstrapNumber, maximumMomentNeededOverall, momentsToBeReweighted, binsizesToBeUsed};
}

LqcdReweightingParameters Reweighter::createLqcdParameters(std::initializer_list<std::string> options)
{
	std::vector<const char*> tmp;
	tmp.push_back(std::string("foo").c_str());
	for(auto i: options)
		tmp.push_back(i.c_str());
	int numberOfArguments = (int)tmp.size();
	LqcdReweightingParameters parameters(numberOfArguments, tmp.data());
	return parameters;
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


static unsigned int getMaximumMomentToBeReweighted(const std::vector<ReweightingProcedure>& rewProcedures){
	unsigned int result = 0;
	for(auto rewProc: rewProcedures){
		unsigned int localMax = *std::max_element(rewProc.momentsToBeReweighted.begin(), rewProc.momentsToBeReweighted.end());
		if( localMax > result)
			result = localMax;
	}
	return result;
}


static void printInformationAboutReweightingProcedure(const ReweightingProcedure& rewProc){
	std::string info = "   Reweighting moments ";
	for(auto i: rewProc.momentsToBeReweighted)
		info += std::to_string(i) + " ";
	info += "for ";
	for(auto i: rewProc.quantitiesConsidered)
		info += i + " ";
	info += "  ";
    std::cout << "\n";
    for(size_t i=0; i<info.length()+4; i++)
        std::cout << "*";
    std::cout << "\n**";
    for(size_t i=0; i<info.length(); i++)
		std::cout << " ";
	std::cout << "**\n";
    std::cout << "**" << info << "**\n**";
    for(size_t i=0; i<info.length(); i++)
		std::cout << " ";
    std::cout << "**\n";
	for(size_t i=0; i<info.length()+4; i++)
		std::cout << "*";
	std::cout << "\n\n";
}


static void checkSizesOfMomentsAndMomentsEstimators(std::vector<std::vector<Moments> > moments, std::vector<std::vector<MomentsEstimators> > momentsEstimators,
													size_t numberOfPoints, size_t numberOfObservablesInFiles)
{
	if(moments.size() != numberOfPoints)
		throw std::runtime_error("Resulting size of moments after Reweighting is different from the expected one (wrong numberOfPoints)!");
	for(size_t i=0; i<moments.size(); i++){
		if(moments[i].size() != numberOfObservablesInFiles)
			throw std::runtime_error("Resulting size of moments after Reweighting is different from the expected one (wrong numberOfObservablesInFiles)!");
	}

	if(momentsEstimators.size() != numberOfPoints)
		throw std::runtime_error("Resulting size of momentsEstimators after Reweighting is different from the expected one (wrong numberOfPoints)!");
	for(size_t i=0; i<momentsEstimators.size(); i++){
		if(momentsEstimators[i].size() != numberOfObservablesInFiles)
			throw std::runtime_error("Resulting size of momentsEstimators after Reweighting is different from the expected one (wrong numberOfObservablesInFiles)!");
	}
}

static void setObservablesAtNewPointsFromMomentsAndMomentEstimators(std::vector<std::vector<Observables> >& observables, bool isMeanZero,
																	ErrorCalculationMethod errorMethod, std::vector<std::vector<Moments> > moments,
																	std::vector<std::vector<MomentsEstimators> > momentsEstimators,
																	std::vector<std::string> quantitiesToBeSet, const std::vector<unsigned int>& columnsToBeReweightedUsingMultipleColumns,
																	const unsigned int numberOfReweightingParameters, const unsigned int mximumMomentNeeded)
{
	for(size_t newPoint=0; newPoint<observables.size(); newPoint++){
		for(size_t obsInFile=0; obsInFile<observables[newPoint].size(); obsInFile++){
			bool useMultipleEstimate = isObservableToBeEvaluatedUsingMultipleColumns(obsInFile, columnsToBeReweightedUsingMultipleColumns, numberOfReweightingParameters, mximumMomentNeeded);
			for(auto quantity: quantitiesToBeSet){
				if(quantity == Mean::observableName)
					observables[newPoint][obsInFile].mean = Mean(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile], isMeanZero, errorMethod, useMultipleEstimate).getValueAndError();
				else if(quantity == Variance::observableName)
					observables[newPoint][obsInFile].susceptibility = Variance(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile], isMeanZero, errorMethod, useMultipleEstimate).getValueAndError();
				else if(quantity == Skewness::observableName)
					observables[newPoint][obsInFile].skewness = Skewness(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile], isMeanZero, errorMethod, useMultipleEstimate).getValueAndError();
				else if(quantity == BinderCumulant::observableName)
					observables[newPoint][obsInFile].binderCumulant = BinderCumulant(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile], isMeanZero, errorMethod, useMultipleEstimate).getValueAndError();
				else
					throw std::invalid_argument("Unknown observable in \"setObservablesAtNewPointsFromMomentsAndMomentEstimators\" function!");
			}
		}
	}
}

/*
 * The following function is needed because the user will tell which columns must be reweighted using several columns
 * with respect to the input files, namely the number of the columns counts implicitly the number of reweighting
 * parameters. After the reweighting, instead, the observable number is just ranging from zero to the total number
 * of observables. Then to understand whether an observable must be estimated using several columns, some logic has
 * to be performed (in this function).
 */
static bool isObservableToBeEvaluatedUsingMultipleColumns(const unsigned int observable, const std::vector<unsigned int>& columnsToBeReweightedUsingMultipleColumns,
														  const unsigned int numberOfReweightingParameters, const unsigned int maximumMomentNeeded){
	std::vector<unsigned int> colsToBeRewUsingMultipleColsMappedIntoObsNumber;
	for(size_t i=0; i<columnsToBeReweightedUsingMultipleColumns.size(); i++)
		colsToBeRewUsingMultipleColsMappedIntoObsNumber.push_back(columnsToBeReweightedUsingMultipleColumns[i]-numberOfReweightingParameters-i*(maximumMomentNeeded-1));
	return find(colsToBeRewUsingMultipleColsMappedIntoObsNumber.begin(), colsToBeRewUsingMultipleColsMappedIntoObsNumber.end(), observable)
			!= colsToBeRewUsingMultipleColsMappedIntoObsNumber.end();
}

static void setMeanToZeroAtNewPoints(std::vector<std::vector<Observables> >& observables){
	for(size_t newPoint=0; newPoint<observables.size(); newPoint++){
		for(size_t obsInFile=0; obsInFile<observables[newPoint].size(); obsInFile++){
			if(std::isnan(observables[newPoint][obsInFile].mean.estimate))
				observables[newPoint][obsInFile].mean.estimate = 0.0;
			else
				throw std::logic_error("Error setting mean.estimate to 0.0 since it should be NAN but it isn't!");
			if(std::isnan(observables[newPoint][obsInFile].mean.error))
				observables[newPoint][obsInFile].mean.error = 0.0;
			else
				throw std::logic_error("Error setting mean.error to 0.0 since it should be NAN but it isn't!");
		}
	}
}



