/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "binnedDataSample.hpp"
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
	};

	JackknifeEstimators(DataSample sampleIn) :
		DataSample(sampleIn)
	{
		int normalization = getJackknifeNormalization();
		double sumOfDataSampleElements = sampleIn.sum();
		//todo: do this removing specific elements -> less rounding errors
		setValues( (*this - sumOfDataSampleElements) * (-1./normalization) );
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

double calculateJacknifeError(DataSample & sampleIn);

#endif /* JACKKNIFEESTIMATORS_HPP_ */
