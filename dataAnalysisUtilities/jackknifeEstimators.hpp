/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "DataSample.hpp"

//todo: add fcts. for mean and error
//todo: overload constructor to add function
class JackknifeEstimators: public DataSample
{
public:
	JackknifeEstimators(int numberOfElementsIn);
	JackknifeEstimators(DataSample sampleIn);

	double getJackknifeVariance();
	double getJackknifeError();
protected:
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed(int n);
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



#endif /* JACKKNIFEESTIMATORS_HPP_ */
