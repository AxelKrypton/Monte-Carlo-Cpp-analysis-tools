#include "Observables.hpp"
#include "binning.hpp"
#include "jackknifeAnalysis.hpp"

static std::vector<DataSample> getMomentsPerDataPoint(DataSample&, std::vector<int>, bool);
static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters&, std::string);
static void printBinningInformation(const Parameters&, std::string);
static double calculateValueFromMomentsAndObsName(Moments, bool, std::string);

/*****************************************************************************************/

ObservableAbstract::ObservableAbstract(std::string obsName, bool isMeanKnownToBeZero) : observableName(obsName), isMeanZero(isMeanKnownToBeZero)
{
	observableEstimateAndError = EstimateAndError();
}

std::string ObservableAbstract::getObservableName()
{
	return observableName;
}

EstimateAndError ObservableAbstract::getValueAndError()
{
	return observableEstimateAndError;
}

//================================================ PROTECTED MEMBERS ================================================//

void ObservableAbstract::calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters)
{
	std::vector<DataSample> neededMomentsPerDataPoint = getNeededMomentsPerDataPoint(dataSample);
	Parameters binningParameters = getLocalParametersWithCorrectBinningInformation(parameters);
	printCorrectBinningInformation(binningParameters);
	std::vector<DataSample> binnedMomentsPerDataPoint = getBinnedNeededMoments(neededMomentsPerDataPoint, binningParameters);
	observableEstimateAndError = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToEvaluateObservable());
}

void ObservableAbstract::calculateAndSetValueAndError(Moments moments, std::vector<std::valarray<double> > estimators, ErrorCalculationMethod errorMethod)
{
	observableEstimateAndError.estimate = calculateValueFromMoments(moments);
	observableEstimateAndError.error = calculateErrorFromEstimators(estimators, errorMethod);
}

//================================================ PRIVATE MEMBERS ================================================//

std::vector<DataSample> ObservableAbstract::getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters){
	std::vector<DataSample> returnData;
	for(size_t i=0; i<dataSampleToBeBinned.size(); i++)
		returnData.push_back(performBinning(dataSampleToBeBinned[i], parameters));
	return returnData;
}


/***********************************************************************************************************************************/
/************************************************************* MEAN ****************************************************************/
/***********************************************************************************************************************************/

Mean::Mean(DataSample& dataSample, Parameters parameters) : ObservableAbstract("MEAN", parameters.isMeanKnownToBeZero)
{
	calculateAndSetValueAndError(dataSample, parameters);
}

Mean::Mean(Moments moments, std::vector<std::valarray<double> > estimators, bool isMeanZero, ErrorCalculationMethod errorMethod) : ObservableAbstract("MEAN", isMeanZero)
{
	calculateAndSetValueAndError(moments, estimators, errorMethod);
}

std::vector<DataSample> Mean::getNeededMomentsPerDataPoint(DataSample& dataSample)
{
	return getMomentsPerDataPoint(dataSample, std::vector<int>(1,1), isMeanZero);
}

Parameters Mean::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
	return buildLocalParametersWithCorrectBinningInformation(parameters, observableName);
}

void Mean::printCorrectBinningInformation(const Parameters& parameters)
{
	printBinningInformation(parameters, observableName);
}

functionSignature Mean::getFunctionToEvaluateObservable()
{
	auto identicalFunction = [] (std::vector<DataSample> in) -> DataSample { if(in.size() != 1) throw std::invalid_argument("Invalid call to Mean function!");
																			 return in[0]; };
	return identicalFunction;
}

double Mean::calculateValueFromMoments(Moments moments)
{
	return calculateValueFromMomentsAndObsName(moments, isMeanZero, observableName);
}










static std::vector<DataSample> getMomentsPerDataPoint(DataSample& sampleIn, std::vector<int> whichMoments, bool isMeanZero)
{
	std::vector<DataSample> returnVec;
	for(auto i: whichMoments)
		returnVec.push_back(isMeanZero ? sampleIn.getNthMomentPerDataPoint(i) : sampleIn.getNthCentralMomentPerDataPoint(i));
	return returnVec;
}

static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable){
	Parameters tmp = parameters;
	if(observable == "MEAN"){
		tmp.binsize = tmp.binsizeMoments[1];
		tmp.numberOfBins = tmp.numberOfBinsMoments[1];
	}else if(observable == "VARIANCE"){
		tmp.binsize = tmp.binsizeCentralMoments[2];
		tmp.numberOfBins = tmp.numberOfBinsCentralMoments[2];
	}else if(observable == "SKEWNESS"){
		tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[3]);
		tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[3]);
	}else if(observable == "BINDER CUMULANT"){
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

/*
 * In the following two functions it is understood that the moments and the estimators
 * are given ONLY for the needed moments and they are considered to be given in order.
 * For example in the Binder Cumulant, the moments vector will contain the second moment
 * as first entry and the fourth moment as second entry.
 */
static double calculateValueFromMomentsAndObsName(Moments moments, bool isMeanZero, std::string observable)
{
	if(observable == "MEAN"){
		double x1 = moments.at(1);
		return isMeanZero ? 0.0 : x1;
	}else if(observable == "VARIANCE"){
		if(isMeanZero){
			return moments.at(2);
		}else{
			double x1 = moments.at(1);
			double x2 = moments.at(2);
			return  x2-x1*x1;
		}
	}else if(observable == "SKEWNESS"){
		if(isMeanZero){
			double x2 = moments.at(2);
			double x3 = moments.at(3);
			return x3/(pow(x2, 1.5));
		}else{
			double x1 = moments.at(1);
			double x2 = moments.at(2);
			double x3 = moments.at(3);
			return (x3-3*x2*x1+2*x1*x1*x1)/(pow(x2-x1*x1, 1.5));
		}
	}else if(observable == "BINDER CUMULANT"){
		if(isMeanZero){
			double x2 = moments.at(2);
			double x4 = moments.at(4);
			return x4/(pow(x2, 2.0));
		}else{
			double x1 = moments.at(1);
			double x2 = moments.at(2);
			double x3 = moments.at(3);
			double x4 = moments.at(4);
			return (x4-4*x3*x1+6*x2*x1*x1-3*x1*x1*x1*x1)/(pow(x2-x1*x1, 2.0));
		}
	}else{
		throw std::invalid_argument("Unknown observable in buildLocalParametersWithCorrectBinningInformation function!");
	}
}






