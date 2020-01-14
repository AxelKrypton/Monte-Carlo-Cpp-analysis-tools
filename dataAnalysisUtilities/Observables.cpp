/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018-2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "Observables.hpp"
#include "binning.hpp"
#include "jackknifeAnalysis.hpp"
#include "bootstrapAnalysis.hpp"

/*****************************************************************************************/
realFloat evaluateErrorBasedOnMethod(DataSample dataSample, ErrorCalculationMethod errorMethod){
    if(errorMethod == jackknife)
        return calculateJacknifeError(dataSample);
    else if (errorMethod == bootstrap)
        return calculateBootstrapError(dataSample);
    else
        throw std::logic_error("Unknown error method in \"evaluateErrorBasedOnMethod\"! Aborting...");
}
/*****************************************************************************************/

static std::vector<DataSample> getMomentsPerDataPoint(DataSample&, std::initializer_list<unsigned int>, bool);
static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters&, std::string);
static void printBinningInformation(const Parameters&, std::string);
template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>&, const int);
//Use here an anonymous namespace because I am not sure about use of static keyword in template specialization
namespace {
    //TODO: Think whether it is possible to unify the following two templates in only one
    template<typename OBSERVABLE> functionForObservable pickUpCorrectFunctionForObservable(const bool isMeanZero, const bool useMultipleEstimate){
        if(isMeanZero)
            return OBSERVABLE::functionToCalculateOservableWithZeroMean;
        else{
            if(useMultipleEstimate)
                return OBSERVABLE::functionToCalculateOservableWithMultipleEstimates;
            else
                return OBSERVABLE::functionToCalculateOservableWithNonZeroMean;
        }
    }
    template<typename OBSERVABLE> functionForEstimators pickUpCorrectFunctionForEstimator(const bool isMeanZero, const bool useMultipleEstimate){
        if(isMeanZero)
            return OBSERVABLE::functionToBeAppliedToEstimatorsWithZeroMean;
        else{
            if(useMultipleEstimate)
                return OBSERVABLE::functionToBeAppliedToEstimatorsWithMultipleEstimates;
            else
                return OBSERVABLE::functionToBeAppliedToEstimatorsWithNonZeroMean;
        }
    }
    //Specialization for Mean case (we want to throw in the zeroMean case!)
    template<> functionForObservable pickUpCorrectFunctionForObservable<Mean>(const bool isMeanZero, const bool useMultipleEstimate){
        if(isMeanZero)
            throw std::logic_error("The Mean::getFunctionToCalculateObservable method should not be called with isMeanZero==true!! Aborting...");
        else{
            if(useMultipleEstimate)
                return Mean::functionToCalculateOservableWithMultipleEstimates;
            else
                return Mean::functionToCalculateOservableWithNonZeroMean;
        }
    }
    template<> functionForEstimators pickUpCorrectFunctionForEstimator<Mean>(const bool isMeanZero, const bool useMultipleEstimate){
        if(isMeanZero)
            throw std::logic_error("The Mean::getFunctionToBeAppliedToEstimators method should not be called with isMeanZero==true!! Aborting...");
        else{
            if(useMultipleEstimate)
                return Mean::functionToBeAppliedToEstimatorsWithMultipleEstimates;
            else
                return Mean::functionToBeAppliedToEstimatorsWithNonZeroMean;
        }
    }
}

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
const functionForObservable Mean::functionToCalculateOservableWithNonZeroMean = [] (Moments in) -> realFloat { return in[1]; };
const functionForObservable Mean::functionToCalculateOservableWithMultipleEstimates = [] (Moments in) -> realFloat { return getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1); };
const functionForEstimators Mean::functionToBeAppliedToEstimatorsWithNonZeroMean = [] (MomentsEstimators in) -> DataSample { return in[1]; };
const functionForEstimators Mean::functionToBeAppliedToEstimatorsWithMultipleEstimates = [] (MomentsEstimators in) -> DataSample { return getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1); };

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
															   return DataSample(std::valarray<realFloat>(0.0, in[0].getNumberOfElements())); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 1) throw std::invalid_argument("Invalid call to Mean function for estimators!"); return in[0]; };
}

functionForEstimators Mean::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
	return pickUpCorrectFunctionForEstimator<Mean>(isMeanZero, useMultipleEstimate);
}

functionForObservable Mean::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Mean>(isMeanZero, useMultipleEstimate);
}

DataSample Mean::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
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
const functionForObservable Variance::functionToCalculateOservableWithZeroMean = [] (Moments in) -> realFloat { return in[2]; };
const functionForObservable Variance::functionToCalculateOservableWithNonZeroMean = [] (Moments in) -> realFloat { return in[2] - in[1] * in[1]; };
const functionForObservable Variance::functionToCalculateOservableWithMultipleEstimates = [] (Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    return in[2] - firstMoment * firstMoment;
};
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithZeroMean = [] (MomentsEstimators in) -> DataSample { return in[2]; };
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithNonZeroMean = [] (MomentsEstimators in) -> DataSample { return in[2] - (in[1] ^ 2); };
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithMultipleEstimates = [] (MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return in[2] - firstMoment * firstMoment;
};

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

functionForEstimators Variance::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanZero, useMultipleEstimate);
}

functionForObservable Variance::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Variance>(isMeanZero, useMultipleEstimate);
}

DataSample Variance::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
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
const functionForObservable Skewness::functionToCalculateOservableWithZeroMean = [] (Moments in) -> realFloat { return in[3] / pow(in[2], 1.5); };
const functionForObservable Skewness::functionToCalculateOservableWithNonZeroMean = [] (Moments in) -> realFloat {
    realFloat x1 = in[1]; realFloat x2 = in[2]; realFloat x3 = in[3];
    return (x3-3*x2*x1+2*x1*x1*x1)/(pow(x2-x1*x1, 1.5));
};
const functionForObservable Skewness::functionToCalculateOservableWithMultipleEstimates = [] (Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    realFloat x2 = in[2]; realFloat x3 = in[3];
    return (x3-3*x2*firstMoment+2*firstMoment*firstMoment*firstMoment)/(pow(x2-firstMoment*firstMoment, 1.5));
};
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithZeroMean = [] (MomentsEstimators in) -> DataSample { return in[3] / (in[2] ^ 1.5); };
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithNonZeroMean = [] (MomentsEstimators in) -> DataSample {
    return (in[3] - ((3 * in[2]) * in[1]) + (2 * (in[1] ^ 3)))/((in[2] - (in[1] ^ 2)) ^ 1.5);
};
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithMultipleEstimates = [] (MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return (in[3] - ((3 * in[2]) * firstMoment) + (2 * firstMoment * firstMoment * firstMoment))/((in[2] - firstMoment * firstMoment) ^ 1.5);
};

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

functionForEstimators Skewness::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanZero, useMultipleEstimate);
}

functionForObservable Skewness::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Skewness>(isMeanZero, useMultipleEstimate);
}

DataSample Skewness::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Skewness::getNeededMoments()
{
	return isMeanZero ? Skewness::neededMomentsWithZeroMean : Skewness::neededMoments;
}


/***********************************************************************************************************************************/
/*********************************************************** KURTOSIS **************************************************************/
/***********************************************************************************************************************************/

//Definition of the static member for the linker
constexpr std::initializer_list<unsigned int> Kurtosis::neededMoments;
constexpr std::initializer_list<unsigned int> Kurtosis::neededMomentsWithZeroMean;
const std::string Kurtosis::observableName = "KURTOSIS";
const functionForObservable Kurtosis::functionToCalculateOservableWithZeroMean = [] (Moments in) -> realFloat { return in[4] / pow(in[2], 2.0); };
const functionForObservable Kurtosis::functionToCalculateOservableWithNonZeroMean = [] (Moments in) -> realFloat {
    realFloat x1 = in[1]; realFloat x2 = in[2]; realFloat x3 = in[3]; realFloat x4 = in[4];
    return (x4-4*x3*x1+6*x2*x1*x1-3*x1*x1*x1*x1)/(pow(x2-x1*x1, 2.0));
};
const functionForObservable Kurtosis::functionToCalculateOservableWithMultipleEstimates = [] (Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    realFloat x2 = in[2]; realFloat x3 = in[3]; realFloat x4 = in[4];
    return (x4-4*x3*firstMoment+6*x2*firstMoment*firstMoment-3*firstMoment*firstMoment*firstMoment*firstMoment)/(pow(x2-firstMoment*firstMoment, 2.0));
};
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithZeroMean = [] (MomentsEstimators in) -> DataSample { return in[4] / (in[2] ^ 2.0); };
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithNonZeroMean = [] (MomentsEstimators in) -> DataSample {
    return (in[4] - (4 * in[3] * in[1]) + (6 * in[2] * in[1] * in[1]) - (3 * in[1] * in[1] * in[1] * in[1]))/((in[2] - (in[1] ^ 2)) ^ 2);
};
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithMultipleEstimates = [] (MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return (in[4] - (4 * in[3] * firstMoment) + (6 * in[2] * firstMoment * firstMoment) - (3 * firstMoment * firstMoment * firstMoment * firstMoment))/((in[2] - (firstMoment ^ 2)) ^ 2);
};

Kurtosis::Kurtosis(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

Kurtosis::Kurtosis(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate) : ObservableAbstract(isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Kurtosis::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, Kurtosis::observableName);
}

void Kurtosis::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, Kurtosis::observableName);
}

functionForEstimatorsForJackknife Kurtosis::getFunctionToBeAppliedToEstimatorsForJackknife()
{
	if(isMeanZero)
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 2) throw std::invalid_argument("Invalid call to Kurtosis function with zero mean for estimators!");
															   DataSample ex2 = in[0]; //estimator second moment
															   DataSample ex4 = in[1]; //estimator third moment
															   return ex4 / (ex2 ^ 2.0); };
	else
		return [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 4) throw std::invalid_argument("Invalid call to Kurtosis function for estimators!");
															   DataSample ex1 = in[0]; //estimator first moment
															   DataSample ex2 = in[1]; //estimator second moment
															   DataSample ex3 = in[2]; //estimator third moment
															   DataSample ex4 = in[3]; //estimator fourth moment
															   return (ex4 - (4 * ex3 * ex1) + (6 * ex2 * ex1 * ex1) - (3 * ex1 * ex1 * ex1 * ex1))/((ex2 - (ex1 ^ 2)) ^ 2); };
}

functionForEstimators Kurtosis::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanZero, useMultipleEstimate);
}

functionForObservable Kurtosis::getFunctionToCalculateObservable(bool useMultipleEstimate	)
{
    return pickUpCorrectFunctionForObservable<Kurtosis>(isMeanZero, useMultipleEstimate);
}

DataSample Kurtosis::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Kurtosis::getNeededMoments()
{
	return isMeanZero ? Kurtosis::neededMomentsWithZeroMean : Kurtosis::neededMoments;
}




//===============================================================================================================================================//

static std::vector<DataSample> getMomentsPerDataPoint(DataSample& sampleIn, std::initializer_list<unsigned int> whichMoments, bool isMeanZero)
{
	std::vector<DataSample> returnVec;
	for(auto i: whichMoments)
		returnVec.push_back(isMeanZero ? sampleIn.getNthMomentPerDataPoint(i) : sampleIn.getNthCentralMomentPerDataPoint(i));
	return returnVec;
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
	}else if(observable == Kurtosis::observableName){
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



//TODO: Implement the following function in a general way
template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>& estimates, const int power){
	if(estimates.size() != 4)
		throw std::invalid_argument("So far only 4 multiple estimates are allowed in quantities calculations! Aborting...");

	switch(power){
	case 1:
		return (estimates[0] + estimates[1] + estimates[2] + estimates[3])/(realFloat)4.0;
	case 2:
		return ((estimates[0] * estimates[1]) + (estimates[0] * estimates[2]) + (estimates[0] * estimates[3]) +
				(estimates[1] * estimates[2]) + (estimates[1] * estimates[3]) + (estimates[2] * estimates[3]))/(realFloat)6.0;
	case 3:
		return ((estimates[0] * estimates[1] * estimates[2]) + (estimates[0] * estimates[1] * estimates[3]) +
				(estimates[0] * estimates[2] * estimates[3]) + (estimates[1] * estimates[2] * estimates[3]))/(realFloat)4.0;
	case 4:
		return (estimates[0] * estimates[1] * estimates[2] * estimates[3]);
	default:
		throw std::invalid_argument("Asked for a not implemented power in \"getPowerOfFirstMomentUsingSeveralEstimate\" function! Aborting...");
	}

}








