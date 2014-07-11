# include "dataAnalysisUtilities.hpp"

static double meanOfDataSample(DataSample & sampleIn)
{
	return sampleIn.getNthMoment(1);
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

static double unbiasedVarianceOfMean(DataSample & sampleIn)
{
	return 1. / double(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

static double unbiasedErrorOfVariance(DataSample & sampleIn)
{
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
	DataSample varianceSample = (sampleIn - sampleIn.getNthMoment(1)) ^ 2;
	return sqrt( unbiasedVarianceOfMean(varianceSample) );
}

EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn)
{
	double mean;
	double error;

	mean = meanOfDataSample(sampleIn);
	error = sqrt( unbiasedVarianceOfMean(sampleIn) );

	return EstimateAndError(mean, error);
}

EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn)
{
	double variance;
	double error;

	variance = unbiasedVarianceOfDataSample(sampleIn);
	error = unbiasedErrorOfVariance(sampleIn);

	return EstimateAndError(variance, error);
}

//todo: this include should not be necessary in the end
#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

//todo: work over this
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
    /**
     * Skewness gamma_1 is defined as:
     *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
     */

	//todo: this is a workaround, the binned dataSample as argument has to be implemented.
	DataSample sample(parameters.file);

    DataSample thirdCentralMoment = (sample - sample.getNthMoment(1)) ^ 3;
    DataSample secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    if ( parameters.useNumberOfBinsForBinning)
      {
	BinnedDataSampleFromNumberOfBins binnedSample1(thirdCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromNumberOfBins binnedSample2(secondCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);

	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
	JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
	double skewness = skewnessSample.getNthMoment(1);
	double error = skewnessSample.getJackknifeError();
    
	return EstimateAndError(skewness, error);
      }
    else
      {
	BinnedDataSampleFromBinsize binnedSample1(thirdCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromBinsize binnedSample2(secondCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);

	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
	JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
	double skewness = skewnessSample.getNthMoment(1);
	double error = skewnessSample.getJackknifeError();
	
	return EstimateAndError(skewness, error);
      }
}

//todo: work over this
//todo: repair: this is acutally binder, rename? print also kurtosis?
EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
    /**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */

	//todo: this is a workaround, the binned dataSample as argument has to be implemented.
	DataSample sample(parameters.file);

    DataSample fourthCentralMoment = (sample - sample.getNthMoment(1)) ^ 4;
    DataSample secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    if ( parameters.useNumberOfBinsForBinning)
      {
	BinnedDataSampleFromNumberOfBins binnedSample1 (fourthCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromNumberOfBins binnedSample2 (secondCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
	JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
	double skewness = kurtosisSample.getNthMoment(1);
	double error = kurtosisSample.getJackknifeError();

	return EstimateAndError(skewness, error);
      }
    else
      {
	BinnedDataSampleFromBinsize binnedSample1(fourthCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromBinsize binnedSample2(secondCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
	JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
	double skewness = kurtosisSample.getNthMoment(1);
	double error = kurtosisSample.getJackknifeError();

	return EstimateAndError(skewness, error);
      }

}

//todo: implement
EstimateAndError calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters)
{
	throw std::invalid_argument("Autocorrelation is not implemented yet. Aborting!");
}

DataSample createDataSampleFromDatafile(std::string filename, Parameters parameters)
{
	DataSample dataSample(filename);
	if ( parameters.useBinning)
	{
		if ( parameters.useNumberOfBinsForBinning)
		{
			return BinnedDataSampleFromNumberOfBins(dataSample, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
		}
		else
		{
			return BinnedDataSampleFromBinsize(dataSample, parameters.binsize, parameters.binningMustFitDataSampleSize);
		}
	}
	else
	{
		return dataSample;
	}
}
