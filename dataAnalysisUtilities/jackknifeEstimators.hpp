/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "dataSampleAnalyzer.hpp"
#include <iostream>

class JackknifeEstimators: public DataSampleAnalyzer
{
public:

	JackknifeEstimators(DataSampleAnalyzer sampleIn) :
		DataSampleAnalyzer(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
	};

	double getJackknifeVariance();
	double getJackknifeError();

protected:
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed(int n);
	DataSample createJackknifeEstimatorsWithBinning(int numberOfBins, int binsize);
};

class JackknifeEstimatorsFromBinnedDataSample: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinnedDataSample(DataSampleAnalyzer sampleIn);
};

class JackknifeEstimatorsFromBinningWithNumberOfBins: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithNumberOfBins(DataSampleAnalyzer sampleIn, int numberOfBins);
};

class JackknifeEstimatorsFromBinningWithBinsize: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithBinsize(DataSampleAnalyzer sampleIn, int numberOfBins);
private:
	void checkIfJackknifeCanBePerformedWithBinsize(int binsize);
};

#endif /* JACKKNIFEESTIMATORS_HPP_ */
