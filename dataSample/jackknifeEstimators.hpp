/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */
//todo: check which version of jackknife is correct (whole sum or binned whole sum)
//todo: in case, replace all reference values accordingly!
//todo: In any case, it seems as if the precision can be replaced by double precision here!

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
		checkIfJackknifeCanBePerformed();
	};

	double getJackknifeVariance();
	double getJackknifeError();

protected:
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed();
};

class JackknifeEstimatorsFromBinnedDataSample: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinnedDataSample(DataSample sampleIn) :
		JackknifeEstimators(sampleIn)
	{
		int normalization = getJackknifeNormalization();
		double sumOfDataSampleElements = values.sum();
		values = (sumOfDataSampleElements - values) / normalization;
		//todo: this is necessary because otherwise the moments from the above sample are returned
		//       as they are calculated in the constructor -> This must be done on demand!
		initMoments();
	}
};

//todo: generalise this to binsize
//todo: think about better name
class JackknifeEstimatorsFromBinning: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinning(DataSample sampleIn, int numberOfBins) :
		JackknifeEstimators(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfBins);
		//todo: this is the sum over the whole sample, not only the binned one!!
		double wholeSum = sampleIn.sum();
		int binsize = calcBinsize(numberOfBins);
		std::cout << "create binned jackknife estimators with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;

		std::valarray<double> binnedDataSample(numberOfBins);
		for(int iteration = 0; iteration < numberOfBins; iteration++)
		{
			std::valarray<double> tmp = values[std::slice(iteration*binsize, binsize, 1)];
			double partSum = tmp.sum();
			binnedDataSample[iteration] = (wholeSum - partSum) / (sampleIn.getNumberOfElements() - binsize);
		}
		values = binnedDataSample;

		numberOfElements = binnedDataSample.size();
		//todo: this is necessary because otherwise the moments from the above sample are returned
		//       as they are calculated in the constructor -> This must be done on demand!
		initMoments();
	}

private:
	void checkIfJackknifeCanBePerformed(int n);
};



#endif /* JACKKNIFEESTIMATORS_HPP_ */
