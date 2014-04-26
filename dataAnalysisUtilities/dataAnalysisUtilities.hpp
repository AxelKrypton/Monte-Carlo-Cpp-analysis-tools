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

#endif /* DATAANALYSISUTILITIES_HPP_ */
