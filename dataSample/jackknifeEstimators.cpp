#include "jackknifeEstimators.hpp"

JackknifeEstimatorsFromBinnedDataSample::JackknifeEstimatorsFromBinnedDataSample(DataSample sampleIn) :
JackknifeEstimators(sampleIn)
{
	int normalization = getJackknifeNormalization();
	double sumOfDataSampleElements = sampleIn.sum();
	values = (sumOfDataSampleElements - values) / normalization;
}

std::valarray<double> JackknifeEstimatorsFromBinning::createJackknifeEstimatorsWithBinning(DataSample sampleIn, int numberOfBins, int binsize)
{
	//todo: this is the sum over the whole sample, not only the binned one!!
	double wholeSum = sampleIn.sum();
	std::cout << "create binned jackknife estimators with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
	std::valarray<double> binnedDataSample(numberOfBins);
	for (int iteration = 0; iteration < numberOfBins; iteration++)
	{
		std::valarray<double> tmp = values[std::slice(iteration * binsize,	binsize, 1)];
		double partSum = tmp.sum();
		binnedDataSample[iteration] = (wholeSum - partSum) / (sampleIn.getNumberOfElements() - binsize);
	}
	return binnedDataSample;
}

JackknifeEstimatorsFromBinning::JackknifeEstimatorsFromBinning(DataSample sampleIn, int numberOfBins) :
	JackknifeEstimators(sampleIn)
{
	checkIfJackknifeCanBePerformed(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	setValues( createJackknifeEstimatorsWithBinning(sampleIn, numberOfBins, binsize) );
}

double JackknifeEstimators::getJackknifeVariance()
{
	return ( createShiftedDataSample(2, getNthMoment(1)) ).getNthMoment(1) * getJackknifeNormalization();
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed(numberOfElements);
	return numberOfElements - 1;
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}
