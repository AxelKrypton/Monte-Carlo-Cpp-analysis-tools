#include "Observables.hpp"
#include "binning.hpp"
#include "jackknifeAnalysis.hpp"
#include "bootstrapAnalysis.hpp"

static std::vector<DataSample> getMomentsPerDataPoint(DataSample&, std::initializer_list<unsigned int>, bool);
static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters&, std::string);
static void printBinningInformation(const Parameters&, std::string);
//static double calculateValueFromMomentsAndObsName(Moments, bool, functionForObservable, std::string);
//static double calculateErrorFromEstimatorsAndObsName(MomentsEstimators, bool, functionForEstimators, ErrorCalculationMethod, std::string);
//static std::initializer_list<int> getNeededMomentsUsingObservableName(bool, std::string);
static double evaluateErrorBasedOnMethod(DataSample, ErrorCalculationMethod);

/*****************************************************************************************/

ObservableAbstract::ObservableAbstract(bool isMeanKnownToBeZero) : isMeanZero(isMeanKnownToBeZero)
{
	observableEstimateAndError = EstimateAndError();
}

EstimateAndError ObservableAbstract::getValueAndError()
{
	return observableEstimateAndError;
}

//================================================ PROTECTED MEMBERS ================================================//

void ObservableAbstract::calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters)
{
	std::vector<DataSample> neededMomentsPerDataPoint = calculateNeededMomentsPerDataPoint(dataSample);
	Parameters binningParameters = getLocalParametersWithCorrectBinningInformation(parameters);
	printCorrectBinningInformation(binningParameters);
	std::vector<DataSample> binnedMomentsPerDataPoint = getBinnedNeededMoments(neededMomentsPerDataPoint, binningParameters);
	observableEstimateAndError = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToBeAppliedToEstimators());
}

void ObservableAbstract::calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod)
{
	observableEstimateAndError.estimate = getFunctionToCalculateObservable()(moments);
	std::initializer_list<unsigned int> selectedMoments = getNeededMoments();
	if(selectedMoments.size() == 0)
		throw std::logic_error("Non sense call to \"calculateAndSetValueAndError\" function, since non moments are needed to evaluate the error! Aborting...");
	DataSample functionAppliedToEstimators = getFunctionToBeAppliedToEstimators()(estimators(selectedMoments));
	observableEstimateAndError.error = evaluateErrorBasedOnMethod(functionAppliedToEstimators, errorMethod);
}

//================================================ PRIVATE MEMBERS ================================================//

std::vector<DataSample> ObservableAbstract::calculateNeededMomentsPerDataPoint(DataSample& dataSample)
{
	return getMomentsPerDataPoint(dataSample, getNeededMoments(), isMeanZero);
}

std::vector<DataSample> ObservableAbstract::getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters){
	std::vector<DataSample> returnData;
	for(size_t i=0; i<dataSampleToBeBinned.size(); i++)
		returnData.push_back(performBinning(dataSampleToBeBinned[i], parameters));
	return returnData;
}


/***********************************************************************************************************************************/
/************************************************************* MEAN ****************************************************************/
/***********************************************************************************************************************************/

//Definition of the static member for the linker
const std::initializer_list<unsigned int> Mean::neededMoments = {1};
const std::initializer_list<unsigned int> Mean::neededMomentsWithZeroMean = {};
const std::string Mean::observableName = "MEAN";

Mean::Mean(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

Mean::Mean(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod) : ObservableAbstract(isMeanZero)
{
	if(isMeanZero)
		observableEstimateAndError = EstimateAndError(0.0, 0.0);
	else
		calculateAndSetValueAndError(moments, estimators, errorMethod);
}

Parameters Mean::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Mean::observableName);
}

void Mean::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Mean::observableName);
}

functionForEstimators Mean::getFunctionToBeAppliedToEstimators()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 0) throw std::invalid_argument("Invalid call to Mean function with zero mean for estimators!");
															   return DataSample(std::valarray<double>(0.0, in[0].getNumberOfElements())); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 1) throw std::invalid_argument("Invalid call to Mean function for estimators!"); return in[0]; };
}

functionForObservable Mean::getFunctionToCalculateObservable()
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in[0]*0.0; }; //in[0]*0.0 just to use the in parameter
	else
		return [] (Moments in) -> double { return in.at(1); };
}

std::initializer_list<unsigned int> Mean::getNeededMoments()
{
	return isMeanZero ? Mean::neededMomentsWithZeroMean : Mean::neededMoments;
}


/***********************************************************************************************************************************/
/************************************************************* VARIANCE ************************************************************/
/***********************************************************************************************************************************/

//Definition of the static member for the linker
constexpr std::initializer_list<unsigned int> Variance::neededMoments;
constexpr std::initializer_list<unsigned int> Variance::neededMomentsWithZeroMean;
const std::string Variance::observableName = "VARIANCE";

Variance::Variance(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

Variance::Variance(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod);
}

Parameters Variance::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Variance::observableName);
}

void Variance::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Variance::observableName);
}

functionForEstimators Variance::getFunctionToBeAppliedToEstimators()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 1) throw std::invalid_argument("Invalid call to Variance function with zero mean for estimators!");
															   return in[0]; };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 2) throw std::invalid_argument("Invalid call to Variance function for estimators!");
															   DataSample ex1 = in[0]; //estimator first moment
															   DataSample ex2 = in[1]; //estimator second moment
															   return ex2 - (ex1 ^ 2); };

}

functionForObservable Variance::getFunctionToCalculateObservable()
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in.at(2); };
	else
		return [] (Moments in) -> double { return in.at(2) - in.at(1) * in.at(1); };
}

std::initializer_list<unsigned int> Variance::getNeededMoments()
{
	return isMeanZero ? Variance::neededMomentsWithZeroMean : Variance::neededMoments;
}


/***********************************************************************************************************************************/
/************************************************************* SKEWNESS ************************************************************/
/***********************************************************************************************************************************/

//Definition of the static member for the linker
constexpr std::initializer_list<unsigned int> Skewness::neededMoments;
constexpr std::initializer_list<unsigned int> Skewness::neededMomentsWithZeroMean;
const std::string Skewness::observableName = "SKEWNESS";

Skewness::Skewness(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

Skewness::Skewness(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod);
}

Parameters Skewness::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Skewness::observableName);
}

void Skewness::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Skewness::observableName);
}

functionForEstimators Skewness::getFunctionToBeAppliedToEstimators()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 2) throw std::invalid_argument("Invalid call to Skewness function with zero mean for estimators!");
															   DataSample ex2 = in[0]; //estimator second moment
															   DataSample ex3 = in[1]; //estimator third moment
															   return ex3 / (ex2 ^ 1.5); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 3) throw std::invalid_argument("Invalid call to Skewness function for estimators!");
															   DataSample ex1 = in[0]; //estimator first moment
															   DataSample ex2 = in[1]; //estimator second moment
															   DataSample ex3 = in[2]; //estimator third moment
															   return (ex3 - ((3 * ex2) * ex1) + (2 * (ex1 ^ 3)))/((ex2 - (ex1 ^ 2)) ^ 1.5); };
}

functionForObservable Skewness::getFunctionToCalculateObservable()
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in.at(3) / pow(in.at(2), 1.5); };
	else
		return [] (Moments in) -> double { double x1 = in.at(1); double x2 = in.at(2); double x3 = in.at(3);
										   return (x3-3*x2*x1+2*x1*x1*x1)/(pow(x2-x1*x1, 1.5)); };
}

std::initializer_list<unsigned int> Skewness::getNeededMoments()
{
	return isMeanZero ? Skewness::neededMomentsWithZeroMean : Skewness::neededMoments;
}


/***********************************************************************************************************************************/
/******************************************************** BINDER CUMULANT **********************************************************/
/***********************************************************************************************************************************/

//Definition of the static member for the linker
constexpr std::initializer_list<unsigned int> BinderCumulant::neededMoments;
constexpr std::initializer_list<unsigned int> BinderCumulant::neededMomentsWithZeroMean;
const std::string BinderCumulant::observableName = "BINDER CUMULANT";

BinderCumulant::BinderCumulant(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

BinderCumulant::BinderCumulant(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod);
}

Parameters BinderCumulant::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, BinderCumulant::observableName);
}

void BinderCumulant::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, BinderCumulant::observableName);
}

functionForEstimators BinderCumulant::getFunctionToBeAppliedToEstimators()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 2) throw std::invalid_argument("Invalid call to BinderCumulant function with zero mean for estimators!");
															   DataSample ex2 = in[0]; //estimator second moment
															   DataSample ex4 = in[1]; //estimator third moment
															   return ex4 / (ex2 ^ 2.0); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 4) throw std::invalid_argument("Invalid call to BinderCumulant function for estimators!");
															   DataSample ex1 = in[0]; //estimator first moment
															   DataSample ex2 = in[1]; //estimator second moment
															   DataSample ex3 = in[2]; //estimator third moment
															   DataSample ex4 = in[3]; //estimator fourth moment
															   return (ex4 - (4 * ex3 * ex1) + (6 * ex2 * ex1 * ex1) - (3 * ex1 * ex1 * ex1 * ex1))/((ex2 - (ex1 ^ 2)) ^ 2); };
}

functionForObservable BinderCumulant::getFunctionToCalculateObservable()
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in.at(4) / pow(in.at(2), 2.0); };
	else
		return [] (Moments in) -> double { double x1 = in.at(1); double x2 = in.at(2); double x3 = in.at(3); double x4 = in.at(4);
										   return (x4-4*x3*x1+6*x2*x1*x1-3*x1*x1*x1*x1)/(pow(x2-x1*x1, 2.0)); };
}

std::initializer_list<unsigned int> BinderCumulant::getNeededMoments()
{
	return isMeanZero ? BinderCumulant::neededMomentsWithZeroMean : BinderCumulant::neededMoments;
}




//===============================================================================================================================================//

static std::vector<DataSample> getMomentsPerDataPoint(DataSample& sampleIn, std::initializer_list<unsigned int> whichMoments, bool isMeanZero)
{
	std::vector<DataSample> returnVec;
	for(auto i: whichMoments)
		returnVec.push_back(isMeanZero ? sampleIn.getNthMomentPerDataPoint(i) : sampleIn.getNthCentralMomentPerDataPoint(i));
	return returnVec;
}

static double evaluateErrorBasedOnMethod(DataSample dataSample, ErrorCalculationMethod errorMethod){
	if(errorMethod == jackknife)
		return calculateJacknifeError(dataSample);
	else if (errorMethod == bootstrap)
		return calculateBootstrapError(dataSample);
	else
		throw std::logic_error("Unknown error method in \"evaluateErrorBasedOnMethod\"! Aborting...");
}

static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable){
	Parameters tmp = parameters;
	if(observable == Mean::observableName){
		tmp.binsize = tmp.binsizeMoments[1];
		tmp.numberOfBins = tmp.numberOfBinsMoments[1];
	}else if(observable == Variance::observableName){
		tmp.binsize = tmp.binsizeCentralMoments[2];
		tmp.numberOfBins = tmp.numberOfBinsCentralMoments[2];
	}else if(observable == Skewness::observableName){
		tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[3]);
		tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[3]);
	}else if(observable == BinderCumulant::observableName){
		tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[4]);
		tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[4]);
	}else{
		throw std::invalid_argument("Unknown observable in buildLocalParametersWithCorrectBinningInformation function!");
	}
	return tmp;
}

static void printBinningInformation(const Parameters& parameters, std::string observable){
	std::cout << "### Performing binning in " << observable << " calculation using ";
	if(parameters.useNumberOfBinsForBinning)
		std::cout << parameters.numberOfBins << " as number of bins!\n";
	else
		std::cout << parameters.binsize << " as binsize!\n";
}


