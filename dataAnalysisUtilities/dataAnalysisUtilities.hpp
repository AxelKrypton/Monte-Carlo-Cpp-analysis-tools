#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include <iostream>
#include "binnedDataSample.hpp"

class MeanAndError
{
public:
	MeanAndError() :
		mean(0.), error(0.)
	{}

	MeanAndError(double mean, double error) :
		mean(mean), error(error)
	{}

	double mean;
	double error;
};

static double meanOfDataSample(DataSampleAnalyzer sampleIn)
{
	return sampleIn.getNthMoment(1);
}

static double unbiasedVarianceOfMean(DataSampleAnalyzer sampleIn)
{
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
	return 1. / double(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

MeanAndError calcMeanAndErrorOfDataSample(DataSample sampleIn)
{
	DataSampleAnalyzer tmp(sampleIn);
	double mean;
	double error;

	mean = meanOfDataSample(tmp);
	error = sqrt( unbiasedVarianceOfMean(tmp) );

	return MeanAndError(mean, error);
}

MeanAndError calcMeanAndErrorOfDataSampleWithBinningFromBinsize(DataSample sampleIn, int binsize)
{
	BinnedDataSampleFromBinsize binnedSample (sampleIn, binsize);
	double mean;
	double error;

	mean = meanOfDataSample(binnedSample);
	error = sqrt( unbiasedVarianceOfMean(binnedSample) );

	return MeanAndError(mean, error);
}

MeanAndError calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(DataSample sampleIn, int numberOfBins)
{
	BinnedDataSampleFromNumberOfBins binnedSample (sampleIn, numberOfBins);
	double mean;
	double error;

	mean = meanOfDataSample(binnedSample);
	error = sqrt( unbiasedVarianceOfMean(binnedSample) );

	return MeanAndError(mean, error);
}

#endif /* DATAANALYSISUTILITIES_HPP_ */
