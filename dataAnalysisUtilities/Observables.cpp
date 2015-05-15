#include "Observables.hpp"
#include "binning.hpp"
#include "jackknifeAnalysis.hpp"
#include "bootstrapAnalysis.hpp"

static std::vector<DataSample> getMomentsPerDataPoint(DataSample&, std::initializer_list<unsigned int>, bool);
static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters&, std::string);
static void printBinningInformation(const Parameters&, std::string);
static double evaluateErrorBasedOnMethod(DataSample, ErrorCalculationMethod);
template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>&, const int);

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
	observableEstimateAndError = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToBeAppliedToEstimatorsForJackknife());
}

void ObservableAbstract::calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
{
	observableEstimateAndError.estimate = getFunctionToCalculateObservable(useMultipleEstimate)(moments);
	DataSample functionAppliedToEstimators = getFunctionToBeAppliedToEstimators(useMultipleEstimate)(estimators);
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

Mean::Mean(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate) : ObservableAbstract(isMeanZero)
{
	if(isMeanZero)
		observableEstimateAndError = EstimateAndError(0.0, 0.0);
	else
		calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Mean::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Mean::observableName);
}

void Mean::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Mean::observableName);
}

functionForEstimatorsForJackknife Mean::getFunctionToBeAppliedToEstimatorsForJackknife()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 0) throw std::invalid_argument("Invalid call to Mean function with zero mean for estimators!");
															   return DataSample(std::valarray<double>(0.0, in[0].getNumberOfElements())); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 1) throw std::invalid_argument("Invalid call to Mean function for estimators!"); return in[0]; };
}

//TODO: Unify the following two function with a static template
functionForEstimators Mean::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
	if(isMeanZero)
		throw std::logic_error("The Mean::getFunctionToBeAppliedToEstimators method should not be called with isMeanZero==true!! Aborting...");
	else{
		if(useMultipleEstimate)
			return [] (MomentsEstimators in) -> DataSample { return getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1); };
		else
			return [] (MomentsEstimators in) -> DataSample { return in[1]; };
	}

}

functionForObservable Mean::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
	if(isMeanZero)
		throw std::logic_error("The Mean::getFunctionToCalculateObservable method should not be called with isMeanZero==true!! Aborting...");
	else{
		if(useMultipleEstimate){
			return [] (Moments in) -> double { return getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 1); };
		}else
			return [] (Moments in) -> double { return in[1]; };
	}
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

Variance::Variance(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Variance::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Variance::observableName);
}

void Variance::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Variance::observableName);
}

functionForEstimatorsForJackknife Variance::getFunctionToBeAppliedToEstimatorsForJackknife()
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

//TODO: Unify the following two function with a static template
functionForEstimators Variance::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
	if(isMeanZero)
		return [] (MomentsEstimators in) -> DataSample { return in[2]; };
	else{
		if(useMultipleEstimate){
			return [] (MomentsEstimators in) -> DataSample {
				DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
				return in[2] - firstMoment * firstMoment;

//				return in[2] - getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 2);
			};
		}else
			return [] (MomentsEstimators in) -> DataSample { return in[2] - (in[1] ^ 2); };
	}
}

functionForObservable Variance::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in[2]; };
	else{
		if(useMultipleEstimate){
			return [] (Moments in) -> double {
				double firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 1);
				return in[2] - firstMoment * firstMoment;

//				return in[2] - getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 2);
			};
		}else
			return [] (Moments in) -> double { return in[2] - in[1] * in[1]; };
	}
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

Skewness::Skewness(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Skewness::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Skewness::observableName);
}

void Skewness::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Skewness::observableName);
}

functionForEstimatorsForJackknife Skewness::getFunctionToBeAppliedToEstimatorsForJackknife()
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

//TODO: Unify the following two function with a static template
functionForEstimators Skewness::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
	if(isMeanZero)
		return [] (MomentsEstimators in) -> DataSample { return in[3] / (in[2] ^ 1.5); };
	else{
		if(useMultipleEstimate){
			return [] (MomentsEstimators in) -> DataSample {
				DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
				return (in[3] - ((3 * in[2]) * firstMoment) + (2 * firstMoment * firstMoment * firstMoment))/((in[2] - firstMoment * firstMoment) ^ 1.5);

//				DataSample firstMomentSquared = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 2);
//				DataSample firstMomentCubic = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 3);
//				return (in[3] - ((3 * in[2]) * firstMoment) + (2 * firstMomentCubic))/((in[2] - firstMomentSquared) ^ 1.5);
			};
		}else
			return [] (MomentsEstimators in) -> DataSample { return (in[3] - ((3 * in[2]) * in[1]) + (2 * (in[1] ^ 3)))/((in[2] - (in[1] ^ 2)) ^ 1.5); };
	}
}

functionForObservable Skewness::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in[3] / pow(in[2], 1.5); };
	else{
		if(useMultipleEstimate){
			return [] (Moments in) -> double {
				double firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 1);
				double x2 = in[2]; double x3 = in[3];
				return (x3-3*x2*firstMoment+2*firstMoment*firstMoment*firstMoment)/(pow(x2-firstMoment*firstMoment, 1.5));

//				double firstMomentSquared = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 2);
//				double firstMomentCubic = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 3);
//				return (x3-3*x2*firstMoment+2*firstMomentCubic)/(pow(x2-firstMomentSquared, 1.5));
			};
		}else{
			return [] (Moments in) -> double {
				double x1 = in[1]; double x2 = in[2]; double x3 = in[3];
				return (x3-3*x2*x1+2*x1*x1*x1)/(pow(x2-x1*x1, 1.5));
			};
		}
	}
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

BinderCumulant::BinderCumulant(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters BinderCumulant::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, BinderCumulant::observableName);
}

void BinderCumulant::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, BinderCumulant::observableName);
}

functionForEstimatorsForJackknife BinderCumulant::getFunctionToBeAppliedToEstimatorsForJackknife()
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

//TODO: Unify the following two function with a static template
functionForEstimators BinderCumulant::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
	if(isMeanZero)
		return [] (MomentsEstimators in) -> DataSample { return in[4] / (in[2] ^ 2.0); };
	else{
		if(useMultipleEstimate){
			return [] (MomentsEstimators in) -> DataSample {
				DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
				return (in[4] - (4 * in[3] * firstMoment) + (6 * in[2] * firstMoment * firstMoment) - (3 * firstMoment * firstMoment * firstMoment * firstMoment))/((in[2] - (firstMoment ^ 2)) ^ 2);

//				DataSample firstMomentSquared = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 2);
//				DataSample firstMomentQuartic = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 4);
//				return (in[4] - (4 * in[3] * firstMoment) + (6 * in[2] * firstMomentSquared) - (3 * firstMomentQuartic))/((in[2] - firstMomentSquared) ^ 2);
			};
		}else{
			return [] (MomentsEstimators in) -> DataSample {
				return (in[4] - (4 * in[3] * in[1]) + (6 * in[2] * in[1] * in[1]) - (3 * in[1] * in[1] * in[1] * in[1]))/((in[2] - (in[1] ^ 2)) ^ 2);
			};
		}
	}
}

functionForObservable BinderCumulant::getFunctionToCalculateObservable(bool useMultipleEstimate	)
{
	if(isMeanZero)
		return [] (Moments in) -> double { return in[4] / pow(in[2], 2.0); };
	else{
		if(useMultipleEstimate){
			return [] (Moments in) -> double {
				double firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 1);
				double x2 = in[2]; double x3 = in[3]; double x4 = in[4];
				return (x4-4*x3*firstMoment+6*x2*firstMoment*firstMoment-3*firstMoment*firstMoment*firstMoment*firstMoment)/(pow(x2-firstMoment*firstMoment, 2.0));

//				double firstMomentSquared = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 2);
//				double firstMomentQuartic = getPowerOfFirstMomentUsingSeveralEstimate<double>(in(1), 4);
//				return (x4-4*x3*firstMoment+6*x2*firstMomentSquared-3*firstMomentQuartic)/(pow(x2-firstMomentSquared, 2.0));
			};
		}else{
			return [] (Moments in) -> double {
				double x1 = in[1]; double x2 = in[2]; double x3 = in[3]; double x4 = in[4];
				return (x4-4*x3*x1+6*x2*x1*x1-3*x1*x1*x1*x1)/(pow(x2-x1*x1, 2.0));
			};
		}
	}
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

//TODO: Implement the following class in a general way

template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>& estimates, const int power){
	if(estimates.size() != 4)
		throw std::invalid_argument("So far only 4 multiple estimates are allowed in quantities calculations! Aborting...");

	switch(power){
	case 1:
		return (estimates[0] + estimates[1] + estimates[2] + estimates[3])/4.0;
	case 2:
		return ((estimates[0] * estimates[1]) + (estimates[0] * estimates[2]) + (estimates[0] * estimates[3]) +
				(estimates[1] * estimates[2]) + (estimates[1] * estimates[3]) + (estimates[2] * estimates[3]))/6.0;
	case 3:
		return ((estimates[0] * estimates[1] * estimates[2]) + (estimates[0] * estimates[1] * estimates[3]) +
				(estimates[0] * estimates[2] * estimates[3]) + (estimates[1] * estimates[2] * estimates[3]))/4.0;
	case 4:
		return (estimates[0] * estimates[1] * estimates[2] * estimates[3]);
	default:
		throw std::invalid_argument("Asked for a not implemented power in \"getPowerOfFirstMomentUsingSeveralEstimate\" function! Aborting...");
	}

}








