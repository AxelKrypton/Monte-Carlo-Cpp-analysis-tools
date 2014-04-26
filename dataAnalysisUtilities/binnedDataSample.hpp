#ifndef BINNEDDATASAMPLE_HPP_
#define BINNEDDATASAMPLE_HPP_

#include "DataSample.hpp"

class BinnedDataSample : public DataSample
{
protected:

	/**
	 * Binning.
	 * Following B.A. Berg,
	 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
	 * p.52.
	 */
	DataSampleBasic performBinning(DataSampleBasic sampleIn);
	void checkDiscardedElements(int valueIn, std::string descriptionIn, int elementsOfSample);

	int numberOfBins;
	int binsize;
};

class BinnedDataSampleFromNumberOfBins : public BinnedDataSample
{
public:
	BinnedDataSampleFromNumberOfBins(DataSampleBasic sampleIn, int numberOfBinsIn);

protected:
	void checkIfNumberOfBinsIsValid(int elementsOfSample);
	void calcBinsize(int elementsOfSample);
};

class BinnedDataSampleFromBinsize : public BinnedDataSample
{
public:
	BinnedDataSampleFromBinsize(DataSampleBasic sampleIn, int binsize);

protected:
	void checkIfBinsizeIsValid(int elementsOfSample);
	void calcNumberOfBins(int elementsOfSample);
};

#endif /* BINNEDDATASAMPLE_HPP_ */
