#ifndef BINNEDDATASAMPLE_HPP_
#define BINNEDDATASAMPLE_HPP_

#include "dataSampleAnalyzer.hpp"

class BinnedDataSample : public DataSample
{
protected:

	/**
	 * Binning.
	 * Following B.A. Berg,
	 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
	 * p.52.
	 */
	DataSample performBinning(DataSample sampleIn);
	void checkDiscardedElements(int valueIn, std::string descriptionIn, int elementsOfSample);

	int numberOfBins;
	int binsize;
};

class BinnedDataSampleFromNumberOfBins : public BinnedDataSample
{
public:
	BinnedDataSampleFromNumberOfBins(DataSample sampleIn, int numberOfBinsIn);

protected:
	void checkIfNumberOfBinsIsValid(int);
	void calcBinsize(int elementsOfSample);
};

#endif /* BINNEDDATASAMPLE_HPP_ */
