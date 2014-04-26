/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "DataSample.hpp"
#include <iostream>

//todo: add fcts. for mean and error
//todo: overload constructor to add function
class JackknifeEstimators: public DataSample
{
public:

	JackknifeEstimators(int numberOfElementsIn) :
		DataSample(numberOfElementsIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
		DataSampleBasic tmp = calculatePseudoValues();
		setValues(tmp);
	};

	JackknifeEstimators(DataSample sampleIn) :
		DataSample(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
		DataSampleBasic tmp = calculatePseudoValues();
		setValues(tmp);
	};

	double getJackknifeVariance();
	double getJackknifeError();

protected:
	DataSampleBasic calculatePseudoValues();
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed(int n);
	//todo: is this simply blocked binning and not jackknifing?
	DataSampleBasic createJackknifeEstimatorsWithBinning(int numberOfBins, int binsize);
};

class JackknifeEstimatorsFromBinnedDataSample: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinnedDataSample(DataSample sampleIn);
};

class JackknifeEstimatorsFromBinningWithNumberOfBins: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithNumberOfBins(DataSample sampleIn, int numberOfBins);
};

class JackknifeEstimatorsFromBinningWithBinsize: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithBinsize(DataSample sampleIn, int numberOfBins);
private:
	void checkIfJackknifeCanBePerformedWithBinsize(int binsize);
};

#endif /* JACKKNIFEESTIMATORS_HPP_ */
