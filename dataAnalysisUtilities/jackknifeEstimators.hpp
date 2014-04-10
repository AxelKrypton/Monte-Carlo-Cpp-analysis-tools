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

//todo: add fcts. for mean and error
//todo: overload constructor to add function
class JackknifeEstimators: public DataSampleAnalyzer
{
public:

	JackknifeEstimators(int numberOfElementsIn) :
		DataSampleAnalyzer(numberOfElementsIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
		DataSample tmp = calculatePseudoValues();
		setValues(tmp);
	};

	JackknifeEstimators(DataSampleAnalyzer sampleIn) :
		DataSampleAnalyzer(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
		DataSample tmp = calculatePseudoValues();
		setValues(tmp);
	};

	double getJackknifeVariance();
	double getJackknifeError();

protected:
	DataSample calculatePseudoValues();
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
