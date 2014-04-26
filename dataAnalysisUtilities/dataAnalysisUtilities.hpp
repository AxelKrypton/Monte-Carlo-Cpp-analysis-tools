#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include <iostream>
#include "binnedDataSample.hpp"

class EstimateAndError
{
public:
	EstimateAndError() :
		estimate(0.), error(0.)
	{}

	EstimateAndError(double mean, double error) :
		estimate(mean), error(error)
	{}

	double estimate;
	double error;
};

static double meanOfDataSample(DataSampleAnalyzer sampleIn)
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
static double unbiasedVarianceOfDataSample(DataSampleAnalyzer sampleIn)
{
	return double(sampleIn.getNumberOfElements()) / double(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthCentralMoment(2);
}

static double unbiasedVarianceOfMean(DataSampleAnalyzer sampleIn)
{
	return 1. / double(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

EstimateAndError calcMeanAndErrorOfDataSample(DataSample sampleIn)
{
	DataSampleAnalyzer tmp(sampleIn);
	double mean;
	double error;

	mean = meanOfDataSample(tmp);
	error = sqrt( unbiasedVarianceOfMean(tmp) );

	return EstimateAndError(mean, error);
}

EstimateAndError calcMeanAndErrorOfDataSampleWithBinningFromBinsize(DataSample sampleIn, int binsize)
{
	BinnedDataSampleFromBinsize binnedSample (sampleIn, binsize);
	double mean;
	double error;

	mean = meanOfDataSample(binnedSample);
	error = sqrt( unbiasedVarianceOfMean(binnedSample) );

	return EstimateAndError(mean, error);
}

EstimateAndError calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(DataSample sampleIn, int numberOfBins)
{
	BinnedDataSampleFromNumberOfBins binnedSample (sampleIn, numberOfBins);
	double mean;
	double error;

	mean = meanOfDataSample(binnedSample);
	error = sqrt( unbiasedVarianceOfMean(binnedSample) );

	return EstimateAndError(mean, error);
}

static double unbiasedErrorOfVariance(DataSampleAnalyzer sampleIn)
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
	DataSampleAnalyzer varianceSample = (sampleIn - sampleIn.getNthMoment(1)) ^ 2;
	return sqrt( unbiasedVarianceOfMean(varianceSample) );
}

EstimateAndError calcVarianceAndErrorOfDataSample(DataSample sampleIn)
{
	DataSampleAnalyzer sample(sampleIn);
	double variance;
	double error;

	variance = unbiasedVarianceOfDataSample(sample);
	DataSampleAnalyzer varianceSample = (sample - sample.getNthMoment(1)) ^ 2;
	error = unbiasedErrorOfVariance(sample);

	return EstimateAndError(variance, error);
}

//todo: this include should not be necessary in the end
#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

//todo: work over this
static EstimateAndError calcSkewness(DataSampleAnalyzer sample, int & numberOfBins)
{
    /**
     * Skewness gamma_1 is defined as:
     *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
     */
    DataSampleAnalyzer thirdCentralMoment = (sample - sample.getNthMoment(1)) ^ 3;
    DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
    JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
    //this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
    JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
    double skewness = skewnessSample.getNthMoment(1);
    double error = skewnessSample.getJackknifeError();

    return EstimateAndError(skewness, error);
}

//todo: work over this
//todo: repair: this is acutally binder, rename? print also kurtosis?
static EstimateAndError calcKurtosis(DataSampleAnalyzer sample, int numberOfBins)
{
    /**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */
    DataSampleAnalyzer fourthCentralMoment = (sample - sample.getNthMoment(1)) ^ 4;
    DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    DataSampleAnalyzer binnedSample1 = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
    JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
    //this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
    JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
    double skewness = kurtosisSample.getNthMoment(1);
    double error = kurtosisSample.getJackknifeError();

    return EstimateAndError(skewness, error);
}

#endif /* DATAANALYSISUTILITIES_HPP_ */
