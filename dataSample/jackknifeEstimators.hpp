/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */
//todo: check which version of jackknife is correct (whole sum or binned whole sum)
//todo: in case, replace all reference values accordingly!
//todo: In any case, it seems as if the precision can be replaced by double precision in the tests!
//todo: generalise numberOfBins class to binsize
//todo: think about better name

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "dataSample.hpp"
#include <iostream>

class JackknifeEstimators: public DataSample
{
public:

	JackknifeEstimators(DataSample sampleIn) :
		DataSample(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfElements);
	};

	double getJackknifeVariance();
	double getJackknifeError();

protected:
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed(int n);
};

class JackknifeEstimatorsFromBinnedDataSample: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinnedDataSample(DataSample sampleIn);
};

class JackknifeEstimatorsFromBinning: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinning(DataSample sampleIn, int numberOfBins);
	std::valarray<double> createJackknifeEstimatorsWithBinning(DataSample sampleIn, int numberOfBins, int binsize);
};

#endif /* JACKKNIFEESTIMATORS_HPP_ */
