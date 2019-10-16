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
static void checkSizesOfHistogramsAndHistogramEstimators(std::vector<std::vector<Histogram> >, std::vector<std::vector<HistogramEstimator> >, size_t, size_t);
static void setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(std::vector<std::vector<ProbabilityDistribution> >&, ErrorCalculationMethod,
																					 std::vector<std::vector<Histogram> >, std::vector<std::vector<HistogramEstimator> >);																	
static void setObservablesEstimatorsAtNewPointsFromMomentsEstimators(std::vector<std::vector<std::map<std::string,DataSample> > >&, bool,
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
	useSimulatedPointsAsNewPoints = parameters.getUseSimulatedPointsAsNewPoints();
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
	useSimulatedPointsAsNewPoints = parameters.getUseSimulatedPointsAsNewPoints();
	std::vector<ReweightingProcedure> reweightingProceduresToBePerformed = getReweightingProceduresToBePerformed();
	if(reweightingProceduresToBePerformed.size() > 0){
		if(parameters.getNumberOfMultipleColumnsForSingleObservable() == 0)
			maximumMomentNeededOverall = getMaximumMomentToBeReweighted(reweightingProceduresToBePerformed);
		else
			maximumMomentNeededOverall = parameters.getNumberOfMultipleColumnsForSingleObservable();
		//Before setting the observables, we have to reserve the correct amount of memory
		size_t numberOfNewPoints, numberOfObservablesInFiles;
		if(useSimulatedPointsAsNewPoints)
			numberOfNewPoints = reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters.size();
		else
			numberOfNewPoints = std::accumulate(newNumberOfPointsOfParameters.begin(), newNumberOfPointsOfParameters.end(), 1, std::multiplies<unsigned int>());
		numberOfObservablesInFiles = reweighterIO.readFromFileDataContainer[0].getNumberOfDataSample() - reweighterIO.namesOfParametersIgnoringMetaParameters.size()
								   - reweighterIO.columnsToBeReweightedUsingMultipleColumns.size()*(maximumMomentNeededOverall-1); //neglect multiple columns (count one column only)
		observablesAtNewPoints = std::vector<std::vector<Observables>>(numberOfNewPoints, std::vector<Observables>(numberOfObservablesInFiles, Observables()));
		if(parameters.getPrintEstimatorsToFile())
		    observablesEstimatorsAtNewPoints = std::unique_ptr<std::vector<std::vector<std::map<std::string,DataSample>>>>(new std::vector<std::vector<std::map<std::string,DataSample>>>(numberOfNewPoints, std::vector<std::map<std::string,DataSample>>(numberOfObservablesInFiles, std::map<std::string,DataSample>())));
		else
		    observablesEstimatorsAtNewPoints = nullptr;
		probabilityDistributionsAtNewPoints = std::vector<std::vector<ProbabilityDistribution> >(numberOfNewPoints, std::vector<ProbabilityDistribution>(numberOfObservablesInFiles,ProbabilityDistribution()));
		std::cout << "maximumMomentNeededOverall = " << maximumMomentNeededOverall << "\n";
		std::cout << "numberOfNewPoints = " << numberOfNewPoints << "\n";
		std::cout << "numberOfObservablesInFiles = " << numberOfObservablesInFiles << "\n";
		std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
		for(auto i: quantitiesToBeReweighted)
			std::cout << i << " - ";
		std::cout << "\n";

		for(auto rewProc: reweightingProceduresToBePerformed){
			if(reweighterIO.deactivateReweightingForProbabilityDistribution)
				rewProc.reweightProbabilityDistributions=false;
			printInformationAboutReweightingProcedure(rewProc);
			MomentsReweighter momentsReweighter(getRawDataForReweightingAndMetainformation(rewProc.momentsToBeReweighted, rewProc.binsizesToBeUsed, rewProc.reweightProbabilityDistributions));
			if(valuesOfNewParameters.empty()) valuesOfNewParameters = momentsReweighter.getValuesOfNewParameters();
			std::vector<std::vector<Moments> > momentsAtNewPoints = momentsReweighter.getMomentsAtNewPoints();
			std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints = momentsReweighter.getMomentsEstimatorsAtNewPoints();
			checkSizesOfMomentsAndMomentsEstimators(momentsAtNewPoints, momentsEstimatorsAtNewPoints, numberOfNewPoints, numberOfObservablesInFiles);
			setObservablesAtNewPointsFromMomentsAndMomentEstimators(observablesAtNewPoints, reweighterIO.isMeanKnownToBeZero, reweighterIO.errorMethod,
																	momentsAtNewPoints, momentsEstimatorsAtNewPoints, rewProc.quantitiesConsidered,
																	reweighterIO.columnsToBeReweightedUsingMultipleColumns,
																	reweighterIO.namesOfParametersIgnoringMetaParameters.size(), maximumMomentNeededOverall);
			if(parameters.getPrintEstimatorsToFile())
			    setObservablesEstimatorsAtNewPointsFromMomentsEstimators(*observablesEstimatorsAtNewPoints, reweighterIO.isMeanKnownToBeZero, momentsEstimatorsAtNewPoints,
			                                                             rewProc.quantitiesConsidered, reweighterIO.columnsToBeReweightedUsingMultipleColumns,
			                                                             reweighterIO.namesOfParametersIgnoringMetaParameters.size(), maximumMomentNeededOverall);
			if(rewProc.reweightProbabilityDistributions){
				printProbabilityDistributionToFile=rewProc.reweightProbabilityDistributions;
				std::cout << "==========================================================" << std::endl;
				std::cout << " Set probability distributions from estimators..." << std::endl;
				std::vector<std::vector<Histogram> > histogramsAtNewPoints = momentsReweighter.getProbabilityDistributionsAtNewPoints();
				std::vector<std::vector<HistogramEstimator> > histogramEstimatorsAtNewPoints = momentsReweighter.getProbabilityDistributionEstimatorsAtNewPoints();
				checkSizesOfHistogramsAndHistogramEstimators(histogramsAtNewPoints, histogramEstimatorsAtNewPoints, numberOfNewPoints, numberOfObservablesInFiles);
				setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(probabilityDistributionsAtNewPoints, reweighterIO.errorMethod, 
																						 histogramsAtNewPoints, histogramEstimatorsAtNewPoints);
				std::cout << "  ...done!" << std::endl;
				std::cout << " Reweighting of probability distributions done!" << std::endl;
				std::cout << "==========================================================" << std::endl;
				std::cout << std::endl;
			}
		}

		//Set manually mean to zero if mean is known to be zero and MEAN is asked
		if(reweighterIO.isMeanKnownToBeZero && find(quantitiesToBeReweighted.begin(), quantitiesToBeReweighted.end(), Mean::observableName) != quantitiesToBeReweighted.end())
			setMeanToZeroAtNewPoints(observablesAtNewPoints);
	}
}


std::vector<std::vector<realFloat> > Reweighter::getValuesOfNewParameters(){
	return valuesOfNewParameters;
}

std::vector<std::vector<Observables> > Reweighter::getReweightedObservables(){
	return observablesAtNewPoints;
}

std::vector<std::vector<std::map<std::string, DataSample> > > Reweighter::getReweightedObservablesEstimators(){
    if(observablesEstimatorsAtNewPoints == nullptr)
        throw std::logic_error("getReweightedObservablesEstimators method of Reweighter called but ObservablesEstimators not asked to be set!");
    else
        return *observablesEstimatorsAtNewPoints;
}

std::vector<std::vector<ProbabilityDistribution> > Reweighter::getReweightedProbabilityDistributions(){
	return probabilityDistributionsAtNewPoints;
}

bool Reweighter::getPrintProbabilityDistributionToFile(){
	return printProbabilityDistributionToFile;
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

RawDataForReweightingAndMetainformation Reweighter::getRawDataForReweightingAndMetainformation(std::vector<unsigned int> momentsToBeReweighted, std::vector<int> binsizesToBeUsed, bool reweightProbabilityDistributions){
	return {reweighterIO.readFromFileDataContainer, reweighterIO.namesOfParametersIgnoringMetaParameters,
			reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters, reweighterIO.valuesOfSpecifiedLogZ,
			newRangesOfParameters, newNumberOfPointsOfParameters, useSimulatedPointsAsNewPoints,
			reweighterIO.isMeanKnownToBeZero, precisionOfIterativeProcedureToCalculateLogZ,
			reweighterIO.columnsToBeReweightedUsingMultipleColumns, reweighterIO.errorMethod,
			reweighterIO.bootstrapNumber, maximumMomentNeededOverall, momentsToBeReweighted, binsizesToBeUsed,
			reweightProbabilityDistributions, reweighterIO.binsizeProbabilityDistribution};
}

LqcdReweightingParameters Reweighter::createLqcdParameters(std::initializer_list<std::string> options)
{
	std::vector<std::string> optionsAsStrings(options);
	optionsAsStrings.insert(optionsAsStrings.begin(), "foo");
	std::vector<const char *> argv(optionsAsStrings.size());
	std::transform(optionsAsStrings.begin(), optionsAsStrings.end(), argv.begin(), [](std::string& str){return str.c_str();});
	LqcdReweightingParameters parameters((int)argv.size(), argv.data());
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
	if(!parameters.getDeactivateReweightingForKurtosis())
		quantities.push_back(Kurtosis::observableName);
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
	else if(obsName == Kurtosis::observableName)
		return isMeanZero ? Kurtosis::neededMomentsWithZeroMean : Kurtosis::neededMoments;
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
		bool isMeanCalculated=false;
        for(std::string quantity : reweightingProcedure.quantitiesConsidered){
            if(quantity == Mean::observableName){
                isMeanCalculated = true;
                break;
            }
        }
		reweightingProcedure.reweightProbabilityDistributions = isMeanCalculated;
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
				else if(quantity == Kurtosis::observableName)
					observables[newPoint][obsInFile].kurtosis = Kurtosis(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile], isMeanZero, errorMethod, useMultipleEstimate).getValueAndError();
				else
					throw std::invalid_argument("Unknown observable in \"setObservablesAtNewPointsFromMomentsAndMomentEstimators\" function!");
			}
		}
	}
}

static void checkSizesOfHistogramsAndHistogramEstimators(std::vector<std::vector<Histogram> > histogram, std::vector<std::vector<HistogramEstimator> > histogramEstimator,
														 size_t numberOfPoints, size_t numberOfObservablesInFiles)
{
	if(histogram.size() != numberOfPoints)
		throw std::runtime_error("Resulting size of histograms after Reweighting is different from the expected one (wrong numberOfPoints)!");
	for(size_t i=0; i<histogram.size(); i++){
		if(histogram[i].size() != numberOfObservablesInFiles)
			throw std::runtime_error("Resulting size of histograms after Reweighting is different from the expected one (wrong numberOfInputObservables)!");
	}

	if(histogramEstimator.size() != numberOfPoints)
		throw std::runtime_error("Resulting size of histogramEstimator after Reweighting is different from the expected one (wrong numberOfPoints)!");
	for(size_t i=0; i<histogramEstimator.size(); i++){
		if(histogramEstimator[i].size() != numberOfObservablesInFiles)
			throw std::runtime_error("Resulting size of histogramEstimator after Reweighting is different from the expected one (wrong numberOfInputObservables)!");
	}
}

static void setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(std::vector<std::vector<ProbabilityDistribution> >& probabilityDistributions, ErrorCalculationMethod errorMethod,
																					 std::vector<std::vector<Histogram> > histograms, std::vector<std::vector<HistogramEstimator> > histogramEstimators)
{
	for(size_t indexNewPoint=0; indexNewPoint<histograms.size(); indexNewPoint++){
		for(size_t indexInputObservable=0; indexInputObservable<histograms[indexNewPoint].size(); indexInputObservable++){
			ProbabilityDistribution tmpProbabilityDistribution(histograms[indexNewPoint][indexInputObservable], histogramEstimators[indexNewPoint][indexInputObservable], errorMethod);
			probabilityDistributions[indexNewPoint][indexInputObservable]=tmpProbabilityDistribution;
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
			if((boost::math::isnan)(observables[newPoint][obsInFile].mean.estimate)) //parenthesis around boost::math::isnan crucial otherwise the std lib macro is called!
				observables[newPoint][obsInFile].mean.estimate = 0.0;
			else
				throw std::logic_error("Error setting mean.estimate to 0.0 since it should be NAN but it isn't!");
			if((boost::math::isnan)(observables[newPoint][obsInFile].mean.error)) //parenthesis around boost::math::isnan crucial otherwise the std lib macro is called!
				observables[newPoint][obsInFile].mean.error = 0.0;
			else
				throw std::logic_error("Error setting mean.error to 0.0 since it should be NAN but it isn't!");
		}
	}
}

static void setObservablesEstimatorsAtNewPointsFromMomentsEstimators(std::vector<std::vector<std::map<std::string,DataSample> > >& observablesEstimators, bool isMeanZero,
                                                                     std::vector<std::vector<MomentsEstimators> > momentsEstimators,
                                                                     std::vector<std::string> quantitiesToBeSet, const std::vector<unsigned int>& columnsToBeReweightedUsingMultipleColumns,
                                                                     const unsigned int numberOfReweightingParameters, const unsigned int mximumMomentNeeded)
{
    for(size_t newPoint=0; newPoint<observablesEstimators.size(); newPoint++){
        for(size_t obsInFile=0; obsInFile<observablesEstimators[newPoint].size(); obsInFile++){
            bool useMultipleEstimate = isObservableToBeEvaluatedUsingMultipleColumns(obsInFile, columnsToBeReweightedUsingMultipleColumns, numberOfReweightingParameters, mximumMomentNeeded);
            for(auto quantity: quantitiesToBeSet){
                if(quantity == Mean::observableName)
                    observablesEstimators[newPoint][obsInFile][Mean::observableName] = Mean::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero, useMultipleEstimate);
                else if(quantity == Variance::observableName)
                    observablesEstimators[newPoint][obsInFile][Variance::observableName]= Variance::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero, useMultipleEstimate);
                else if(quantity == Skewness::observableName)
                    observablesEstimators[newPoint][obsInFile][Skewness::observableName] = Skewness::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero, useMultipleEstimate);
                else if(quantity == Kurtosis::observableName)
                    observablesEstimators[newPoint][obsInFile][Kurtosis::observableName] = Kurtosis::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero, useMultipleEstimate);
                else
                    throw std::invalid_argument("Unknown observable in \"setObservablesAtNewPointsFromMomentsAndMomentEstimators\" function!");
            }
        }
    }
}


