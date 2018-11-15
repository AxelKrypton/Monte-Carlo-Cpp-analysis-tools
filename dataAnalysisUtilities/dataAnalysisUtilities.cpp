# include "dataAnalysisUtilities.hpp"

#include "binning.hpp"
#include "binnedDataSample.hpp"
#include "../IO/io_utilities.hpp"
#include "jackknifeAnalysis.hpp"
#include "binnedDataSample.hpp"

static Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable){
	Parameters tmp = parameters;
	if(observable == "mean"){
		tmp.binsize = tmp.binsizeMoments[1];
		tmp.numberOfBins = tmp.numberOfBinsMoments[1];
	}else if(observable == "variance"){
		tmp.binsize = tmp.binsizeCentralMoments[2];
		tmp.numberOfBins = tmp.numberOfBinsCentralMoments[2];
	}else if(observable == "skewness"){
		tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[3]);
		tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[3]);
	}else if(observable == "kurtosis"){
		tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[4]);
		tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[4]);
	}else{
		throw std::invalid_argument("Unknown observable in buildLocalParametersWithCorrectBinningInformation function!");
	}
	return tmp;
}

static void printBinningInformation(const Parameters& parameters, std::string observable){
	std::cout << "# Performing binning in " << observable << " calculation using ";
	if(parameters.useNumberOfBinsForBinning)
		std::cout << parameters.numberOfBins << " as number of bins!\n";
	else
		std::cout << parameters.binsize << " as binsize!\n";
}


static realFloat meanOfDataSample(DataSample & sampleIn)
{
	return sampleIn.getNthMoment(1);
}

/**
 * Note that the unbiased estimate of the 
 * variance of the mean is the same as the 
 * error coming from a jackknife analysis,
 * as the pseudovalues for the mean
 * are again the original data points.
 */
static realFloat unbiasedVarianceOfMean(DataSample & sampleIn, bool isMeanKnownToBeZero)
{
	return isMeanKnownToBeZero ? 1. / realFloat(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthMoment(2)
							   : 1. / realFloat(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn, bool isMeanKnownToBeZero)
{
	realFloat mean = 0.;
	realFloat error = 0.;

	if(!isMeanKnownToBeZero) mean = meanOfDataSample(sampleIn);
	error = sqrt( unbiasedVarianceOfMean(sampleIn, isMeanKnownToBeZero) );

	return EstimateAndError(mean, error);
}

EstimateAndError calcMeanAndErrorOfDataSample(DataSample sampleIn, Parameters parameters)
{
	Parameters binningParameters = buildLocalParametersWithCorrectBinningInformation(parameters, "mean");
	printBinningInformation(binningParameters, "MEAN");
	DataSample binnedData = performBinning(sampleIn, binningParameters);

	return calcMeanAndErrorOfUncorrelatedDataSample(binnedData, parameters.isMeanKnownToBeZero);
}


/**
 * A Jackknife analysis of the (naive) sample variance
 *   1/N Sum ( sample[i] - mean )^2
 * yields that the pseudovalues are
 *   N/(N-1) Sum ( sample[i] - mean )^2
 * (Example 3 in "Jackknife.pdf").
 * This means that the jackknife error estimate is
 *   sqrt(1/(N-1) * (Variance of the pseudo-values) )
 * Hence, one can generate a new sample with each entry x_j
 * the "variance" of entry x_j ( (x_j - mean)^2 ) and treat
 * it the same way as an error on a mean.
 */
static realFloat unbiasedErrorOfVariance(DataSample & varianceSample)
{
	return sqrt( unbiasedVarianceOfMean(varianceSample, 0) );
}

/**
 * A biased estimate of the sample variance is the naive
 *   definition, the second central moment.
 * An unbiased estimate of variance of the sample is
 *   n/(n-1) * biasedEstimator(varianceOfSample)
 * and the estimate of the variance of the mean of the sample is always:
 *   varianceEstimator(sample) / n
 * because of the central limit theorem.
 * See B.A. Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis"
 * Note that for the mean the unbiased variance yields
 * the same error as jackknifing.
 */
static realFloat unbiasedVarianceOfDataSample(DataSample & sampleIn, bool isMeanKnownToBeZero)
{
	return isMeanKnownToBeZero ? realFloat(sampleIn.getNumberOfElements()) / realFloat(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthMoment(2)
							   : realFloat(sampleIn.getNumberOfElements()) / realFloat(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthCentralMoment(2);
}

//TODO: Bad to pass both isMeanKnownToBeZero and parameters that contains the first. Once done the todo in  dataAnalysisUtilities.hpp, this is basically solved!
static EstimateAndError calcVarianceAndError(DataSample& sampleIn, bool isMeanKnownToBeZero , Parameters *parameters = NULL, bool shouldUseBinning = false)
{
	realFloat variance = 0.;
	realFloat error = 0.;

	DataSample varianceSample = isMeanKnownToBeZero ? sampleIn.getNthMomentPerDataPoint(2) : sampleIn.getNthCentralMomentPerDataPoint(2);
	if ( shouldUseBinning )
	{
		Parameters binningParameters = buildLocalParametersWithCorrectBinningInformation(*parameters, "variance");
		printBinningInformation(binningParameters, "VARIANCE");
		DataSample binnedData = performBinning(varianceSample, binningParameters);
		error = unbiasedErrorOfVariance(binnedData);
		// todo: improve!
		performBinning(sampleIn, binningParameters);
	}
	else
	{
		error = unbiasedErrorOfVariance(varianceSample);
	}

	//this must be calculated after a possible binning as this may discard some elements!
	variance = unbiasedVarianceOfDataSample(sampleIn, isMeanKnownToBeZero);
	
	return EstimateAndError(variance, error);
}

EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample & sampleIn, bool isMeanKnownToBeZero)
{
	return calcVarianceAndError(sampleIn, isMeanKnownToBeZero);
}

EstimateAndError calcVarianceAndErrorOfDataSample(DataSample sampleIn, Parameters parameters)
{
	return calcVarianceAndError(sampleIn, parameters.isMeanKnownToBeZero, &parameters, true);
}

EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample sampleIn, Parameters parameters)
{
	/**
		* Skewness gamma_1 is defined as:
		*   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		*/
	DataSample thirdCentralMomentSample = parameters.isMeanKnownToBeZero ? sampleIn.getNthMomentPerDataPoint(3) : sampleIn.getNthCentralMomentPerDataPoint(3);
	DataSample secondCentralMomentSample = parameters.isMeanKnownToBeZero ? sampleIn.getNthMomentPerDataPoint(2) : sampleIn.getNthCentralMomentPerDataPoint(2);

	Parameters binningParameters = buildLocalParametersWithCorrectBinningInformation(parameters, "skewness");
	printBinningInformation(binningParameters, "SKEWNESS");
	DataSample binnedSample1 = performBinning(thirdCentralMomentSample, binningParameters);
	DataSample binnedSample2 = performBinning(secondCentralMomentSample, binningParameters);
	
//	auto calcSkewness = [] (DataSample & in1, DataSample & in2) -> DataSample { return in1 / (in2 ^ (3. / 2)); };
	auto calcSkewness = [] (std::vector<DataSample> in) -> DataSample { return in[0] / (in[1] ^ (3. / 2)); };
	std::vector<DataSample> sampleForJack;
	sampleForJack.push_back(binnedSample1);
	sampleForJack.push_back(binnedSample2);
	
//	return jackknifeAnalysis(binnedSample1, binnedSample2, calcSkewness);
	return jackknifeAnalysis(sampleForJack, calcSkewness);
}

EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample sampleIn, Parameters parameters)
{
	/**
	 * The Fourth Std. Moment beta_2 is defined as:
	 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
	 * This is also referred to as "kurtosis"
	 * The excess kurtosis gamma_2 is defined as:
	 *   gamma_2 = beta_2 - 3
	 */
	DataSample fourthCentralMoment = parameters.isMeanKnownToBeZero ? sampleIn.getNthMomentPerDataPoint(4) : sampleIn.getNthCentralMomentPerDataPoint(4);
	DataSample secondCentralMoment = parameters.isMeanKnownToBeZero ? sampleIn.getNthMomentPerDataPoint(2) : sampleIn.getNthCentralMomentPerDataPoint(2);
	
	Parameters binningParameters = buildLocalParametersWithCorrectBinningInformation(parameters, "kurtosis");
	printBinningInformation(binningParameters, "KURTOSIS");
	DataSample binnedSample1 = performBinning(fourthCentralMoment, binningParameters);
	DataSample binnedSample2 = performBinning(secondCentralMoment, binningParameters);
	
	auto calcKurtosis = [] (DataSample & in1, DataSample & in2) -> DataSample { return in1 / (in2 ^ 2.); };

	return jackknifeAnalysis(binnedSample1, binnedSample2, calcKurtosis);
}

/*
 * Here, following the Berg book (pages 201-202), a method to estimate the integrated_autocorrelation_time
 * with an error is implemented. It is based on the Jackknife method. The idea is to build a set of
 * jackknife estimators for the autocorrelation_function C(t) of eq. (4.3) at some FIXED time.
 *
 * Once this set X of estimators is ready one can
 *   - either use the jackknife function above with a trivial f in order to get a value with
 *     error for C(t)
 *      --->  jackknife(X, [] (realFloat val) -> realFloat {return val;})
 *   - or build other sets of estimators like X at different times and use them to build a set Y
 *     of estimators for the integrated_autocorrelation_time at time t (see eq. (4.14)). Again
 *     use the jackknife function above with a trivial f in order to get a value with error for tau_int(t)
 *      --->  jackknife(Y, [] (realFloat val) -> realFloat {return val;})
 *  In both cases, one can produce plots similar to those of Figure 4.1-4.2.
 *
 *  Plotting the resulting data with errors, from the FIRST plateau, one can make the final estimate
 *  of the integrated_autocorrelation_time (read around equation (4.13) to understand why to look at
 *  the first plateau is important).
 *
 *  NOTE: Probably the jackknife call could in principle done with a more complicated function
 *        so that the sets X and Y can be built directly inside the jackknife. This is not however
 *        wise because to apply the jackknife one must have NOT correlated data and here this is
 *        achieved with a binning on the estimators BEFORE calling the jackknife.
 *
 *  NOTE: In principle, if one is interested only to a rough idea of the integrated_autocorrelation_time
 *        he can implement the equation (4.14) and look for a plateau plotting the output data.
 *        Of course this is not so rigorous because no error is estimated.
 *
 *  NOTE: The number of bins in the following function is that used to make binning on the estimators before
 *        the jackknife and it is contained in parameters (numberOfBinsForAutocorrelation).
 *        On page 201 of Berg's book there is written that it has to be much smaller of the total amount of data.
 *        This is up to the user, but if it is not given, then it is set to 10 (in principle fine for a data sample
 *        with more than 1000 data).
 */
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample & sample, int time);
static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample & sample, Parameters parameters);

std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters)
{

	std::vector<BinnedDataSampleFromNumberOfBins>
	autocorrelationFunctionValuesBinnedSets = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

	std::vector<EstimateAndError> result;
    auto identicalFunction = [] (DataSample& in) -> DataSample { return in; };
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
        result.push_back(jackknifeAnalysis(autocorrelationFunctionValuesBinnedSets[time], identicalFunction));
	}

	return result;
}

std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters)
{

	std::vector<BinnedDataSampleFromNumberOfBins>
	autocorrelationFunctionValuesBinnedSets = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

	std::vector<DataSample> integratedTimeBinnedSets;
	//The first array of integratedTimeBinnedSets must be an array of ones
	integratedTimeBinnedSets.push_back(DataSample(std::valarray<realFloat>(1.0, autocorrelationFunctionValuesBinnedSets[0].getNumberOfElements())));

	for(int time=1; time<parameters.timeMaxAutocorrelationFunction; time++){
		integratedTimeBinnedSets.push_back((integratedTimeBinnedSets[time-1]
		                                    + 2.0 * autocorrelationFunctionValuesBinnedSets[time] / autocorrelationFunctionValuesBinnedSets[0]));
	}

	std::vector<EstimateAndError> result;
    auto identicalFunction = [] (DataSample& in) -> DataSample { return in; };
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
        result.push_back(jackknifeAnalysis(integratedTimeBinnedSets[time], identicalFunction));
    }

	return result;
}

//todo: move this to IO
static std::string getFilenameForAutocorrelation(Parameters parameters)
{
	return parameters.file + "_auto";
}

void calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters)
{
	std::vector<EstimateAndError> result = calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(sample, parameters);

	std::vector<realFloat> estimates;
	std::vector<realFloat> errors;
	for (int i = 0; i< int(result.size()); i++ )
	{
		estimates.push_back(result[i].estimate);
		errors.push_back(result[i].error);
	}

	std::string filename = getFilenameForAutocorrelation(parameters);

	writeEstimateAndErrorArraysToFile("auto", estimates, errors, filename);
}

/*****************************************************************************************/
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample & sample, int time)
{
	DataSampleBasic x_first  = sample.sampleSlice(0, sample.getNumberOfElements() - time, 1);
	DataSampleBasic x_second = sample.sampleSlice(time, sample.getNumberOfElements() - time, 1);
	realFloat data_mean = sample.getNthMoment(1);
	/*
	 * Since we do not know in general if the "true" mean value of our sample is zero, we will
	 * always substitute it by its estimator (the mean of the sample itself), but then we introduce
	 * a bias that we correct here. Read paragraph between (4.19) and (4.20) on Berg's book for
	 * additional informations.
	 */
	DataSampleBasic result = ((x_first - data_mean) * (x_second - data_mean));
	result = result * sample.getNumberOfElements() / (sample.getNumberOfElements() - 1);
	return result;
}

static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample & sample, Parameters parameters)
{
	std::vector<BinnedDataSampleFromNumberOfBins> autocorrelationFunctionValuesBinnedSets;
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		autocorrelationFunctionValuesBinnedSets.push_back(BinnedDataSampleFromNumberOfBins(
				                                          autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(sample, time),
				                                          parameters.numberOfBinsForAutocorrelation, false, false));
	}
	return autocorrelationFunctionValuesBinnedSets;
}


