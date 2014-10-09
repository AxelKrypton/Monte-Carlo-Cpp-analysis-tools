# include "dataAnalysisUtilities.hpp"

#include "binnedDataSample.hpp"
#include "../IO/io_utilities.hpp"

DataSample performBinning(DataSample & rawData, const Parameters parameters)
{
	if ( parameters.useBinning )
	{
			std::cout << "Perform binning on data sample..." << std::endl;
			if ( parameters.useNumberOfBinsForBinning)
			{
				return performBinningFromNumberOfBins(rawData, parameters.numberOfBins, parameters.adjustDataSampleSizeToBinning, parameters.binningMustFitDataSampleSize);
			}
			else
			{
				return performBinningFromBinsize(rawData, parameters.binsize, parameters.adjustDataSampleSizeToBinning, parameters.binningMustFitDataSampleSize);
			}
	}
	else
	{
		throw std::invalid_argument("Binning requested, but have different inputparameters!");
	}
}

static double meanOfDataSample(DataSample & sampleIn)
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
static double unbiasedVarianceOfMean(DataSample & sampleIn)
{
	return 1. / double(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
	DataSample binnedData = performBinning(sampleIn, parameters);

	return calcMeanAndErrorOfUncorrelatedDataSample(binnedData);
}

EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn)
{
	double mean = 0.;
	double error = 0.;

	mean = meanOfDataSample(sampleIn);
	error = sqrt( unbiasedVarianceOfMean(sampleIn) );

	return EstimateAndError(mean, error);
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
static double unbiasedErrorOfVariance(DataSample & varianceSample)
{
	return sqrt( unbiasedVarianceOfMean(varianceSample) );
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
static double unbiasedVarianceOfDataSample(DataSample & sampleIn)
{
	return double(sampleIn.getNumberOfElements()) / double(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthCentralMoment(2);
}

static DataSample createVarianceSample(DataSample sampleIn)
{
	return (sampleIn - sampleIn.getNthMoment(1)) ^ 2;
}

EstimateAndError calcVarianceAndError(DataSample & sampleIn, Parameters parameters, bool shouldUseBinning = false)
{
	double variance = 0.;
	double error = 0.;

	DataSample varianceSample = createVarianceSample(sampleIn);
	if ( shouldUseBinning )
	{
		DataSample binnedData = performBinning(varianceSample, parameters);
		error = unbiasedErrorOfVariance(binnedData);
	}
	else
	{
		error = unbiasedErrorOfVariance(varianceSample);
	}

	//this must be calculated after a possible binning as this may discard some elements!
	variance = unbiasedVarianceOfDataSample(sampleIn);
	
	return EstimateAndError(variance, error);
}

EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample & sampleIn)
{
	const char * dummyArguments[] = {"foo", "dummyFile"};
	Parameters dummy(2, dummyArguments);
	return calcVarianceAndError(sampleIn, dummy);
}

EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
	return calcVarianceAndError(sampleIn, parameters, true);
}

//todo: this include should not be necessary in the end
#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

DataSample calcSkewness(DataSample & in1, DataSample & in2)
{
	return in1 / (in2 ^ (3. / 2));
}

EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
	/**
		* Skewness gamma_1 is defined as:
		*   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		*/
	double skewness = 0.;
	double error = 0.;
	
	skewness = sampleIn.getNthCentralMoment(3) / pow (sampleIn.getNthCentralMoment(2),(3./2.) );
	
	DataSample thirdCentralMomentSample = (sampleIn - sampleIn.getNthMoment(1)) ^ 3;
	DataSample secondCentralMomentSample = (sampleIn - sampleIn.getNthMoment(1)) ^ 2;	
	
	DataSample binnedSample1 = performBinning(thirdCentralMomentSample, parameters);
	DataSample binnedSample2 = performBinning(secondCentralMomentSample, parameters);
	
	JackknifeEstimators jackSample1(binnedSample1);
	JackknifeEstimators jackSample2(binnedSample2);
	
	DataSample skewnessSample = calcSkewness(jackSample1, jackSample2);
	
	error = calculateJacknifeError(skewnessSample);
	
	return EstimateAndError(skewness, error);
}

DataSample calcBinder(DataSample & in1, DataSample & in2)
{
	return in1 / (in2 ^ 2);
}

EstimateAndError calcBinderAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
	/**
	 * The Fourth Std. Moment beta_2 is defined as:
	 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
	 * This is also referred to as "Binder-cumulant"
	 * The Kurtosis gamma_2 is defined as:
	 *   gamma_2 = beta_2 - 3
	 */
	double binder = sampleIn.getNthCentralMoment(4) / pow (sampleIn.getNthCentralMoment(2),2. );
	double error = 0.;
	
	DataSample fourthCentralMoment = (sampleIn - sampleIn.getNthMoment(1)) ^ 4;
	DataSample secondCentralMoment = (sampleIn - sampleIn.getNthMoment(1)) ^ 2;
	
	DataSample binnedSample1 = performBinning(fourthCentralMoment, parameters);
	DataSample binnedSample2 = performBinning(secondCentralMoment, parameters);	

	JackknifeEstimators jackSample1(binnedSample1);
	JackknifeEstimators jackSample2(binnedSample2);
	
	DataSample binderSample = calcBinder(jackSample1, jackSample2);

	error = calculateJacknifeError(binderSample);
	
	return EstimateAndError(binder, error);
}

/*
 * Here, following the Berg book (pages 201-202), a method to estimate the integrated_autocorrelation_time
 * with an error is implemented. It is based on the Jackknife method. The idea is to build a set of
 * jackknife estimators for the autocorrelation_function C(t) of eq. (4.3) at some FIXED time.
 *
 * Once this set X of estimators is ready one can
 *   - either use the jackknife function above with a trivial f in order to get a value with
 *     error for C(t)
 *      --->  jackknife(X, [] (double val) -> double {return val;})
 *   - or build other sets of estimators like X at different times and use them to build a set Y
 *     of estimators for the integrated_autocorrelation_time at time t (see eq. (4.14)). Again
 *     use the jackknife function above with a trivial f in order to get a value with error for tau_int(t)
 *      --->  jackknife(Y, [] (double val) -> double {return val;})
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
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		JackknifeEstimators jackSample(autocorrelationFunctionValuesBinnedSets[time]);
		result.push_back(EstimateAndError(autocorrelationFunctionValuesBinnedSets[time].getNthMoment(1), jackSample.getJackknifeError()));
	}

	return result;
}

std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters)
{

	std::vector<BinnedDataSampleFromNumberOfBins>
	autocorrelationFunctionValuesBinnedSets = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

	std::vector<DataSample> integratedTimeBinnedSets;
	//The first array of integratedTimeBinnedSets must be an array of ones
	integratedTimeBinnedSets.push_back(DataSample(std::valarray<double>(1.0, autocorrelationFunctionValuesBinnedSets[0].getNumberOfElements())));

	for(int time=1; time<parameters.timeMaxAutocorrelationFunction; time++){
		integratedTimeBinnedSets.push_back((integratedTimeBinnedSets[time-1]
		                                    + 2.0 * autocorrelationFunctionValuesBinnedSets[time] / autocorrelationFunctionValuesBinnedSets[0]));
	}

	std::vector<EstimateAndError> result;
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		JackknifeEstimators jackSample(integratedTimeBinnedSets[time]);
		result.push_back(EstimateAndError(integratedTimeBinnedSets[time].getNthMoment(1), jackSample.getJackknifeError()));
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

	std::vector<double> estimates;
	std::vector<double> errors;
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
	double data_mean = sample.getNthMoment(1);
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


EstimateAndError::EstimateAndError() :
	estimate(0.), error(0.)
{}

EstimateAndError::EstimateAndError(double mean, double error) :
	estimate(mean), error(error)
{}

