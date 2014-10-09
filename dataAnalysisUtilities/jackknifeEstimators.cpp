#include "jackknifeEstimators.hpp"

#include "binnedDataSample.hpp"

JackknifeEstimators::JackknifeEstimators(int numberOfElementsIn) :
DataSample(numberOfElementsIn)
{
	checkIfJackknifeCanBePerformed(numberOfElements);
}

JackknifeEstimators::JackknifeEstimators(DataSample sampleIn) :
DataSample(sampleIn)
{
	int normalization = getJackknifeNormalization();
	double sumOfDataSampleElements = sampleIn.sum();
	//todo: do this removing specific elements -> less rounding errors
	setValues( (*this - sumOfDataSampleElements) * (-1./normalization) );
}

//todo: the following fcts. are just copied here during refactoring!

void checkDiscardedElements(int valueIn, std::string descriptionIn, int numberOfElements)
{
	int discardedElements = numberOfElements % valueIn;
	if (discardedElements != 0)
	{
		std::cout << "Warning: " << descriptionIn << " is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements<< " elements are discarded!" << std::endl;
	}
}

int calcBinsize(int numberOfBins, int numberOfElements)
{
	checkDiscardedElements(numberOfBins, "numberOfBins", numberOfElements);
	return numberOfElements / numberOfBins;
}

int calcNumberOfBins(int binsize, int numberOfElements)
{
	checkDiscardedElements(binsize, "binsize", numberOfElements);
	return numberOfElements / binsize;
}

double JackknifeEstimators::getJackknifeVariance()
{
	DataSample tmp ( (*this - getNthMoment(1) )^( (double(2)) )  );
	return tmp.getNthMoment(1) * getJackknifeNormalization();
}

double calculateJacknifeError(DataSample & sampleIn)
{
	int jackknifeNormalization = sampleIn.getNumberOfElements() - 1;
	DataSample tmp ( (sampleIn - sampleIn.getNthMoment(1) )^( (double(2)) )  );
	return tmp.getNthMoment(1) * jackknifeNormalization;
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

double calculateJacknifeEstimator(DataSample & sampleIn)
{
	return sampleIn.getNthMoment(1);
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed(numberOfElements);
	return numberOfElements - 1;
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1 || n > numberOfElements)
		throw std::invalid_argument("Cannot create jackknifeEstimators with these parameters!");
}

DataSampleBasic JackknifeEstimators::createJackknifeEstimatorsWithBinning(int numberOfBins, int binsize)
{
	std::cout << "create binned jackknife estimators with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;

	DataSampleBasic cutSample = sampleSlice(0, binsize*numberOfBins, 1);
	double sumOfAllElementsInBinnedSample = cutSample.sum();
	int numberOfElementsInBinndedSample = cutSample.getNumberOfElements();

	DataSampleBasic binnedDataSample(numberOfBins);
	for (int iteration = 0; iteration < numberOfBins; iteration++)
	{
		double sumOfAllElementsInBin = ( sampleSlice(iteration * binsize,	binsize, 1) ).sum();
		binnedDataSample[iteration] = (sumOfAllElementsInBinnedSample - sumOfAllElementsInBin) / (numberOfElementsInBinndedSample - binsize);
	}
	return binnedDataSample;
}

//TODO: remove both "setValues(sampleIn)". They are "undoing" the setting from the constructor of JackknifeEstimators.
JackknifeEstimatorsFromBinningWithNumberOfBins::JackknifeEstimatorsFromBinningWithNumberOfBins(DataSample sampleIn, int numberOfBins) :
	JackknifeEstimators(sampleIn)
{
	setValues(sampleIn);
	checkIfJackknifeCanBePerformed(numberOfBins);
	int binsize = calcBinsize(numberOfBins, sampleIn.getNumberOfElements());
	setValues( createJackknifeEstimatorsWithBinning(numberOfBins, binsize) );
}

JackknifeEstimatorsFromBinningWithBinsize::JackknifeEstimatorsFromBinningWithBinsize(DataSample sampleIn, int binsize) :
	JackknifeEstimators(sampleIn)
{
	setValues(sampleIn);
	checkIfJackknifeCanBePerformedWithBinsize(binsize);
	int numberOfBins = calcBinsize(binsize, sampleIn.getNumberOfElements());
	setValues( createJackknifeEstimatorsWithBinning(numberOfBins, binsize) );
}

void JackknifeEstimatorsFromBinningWithBinsize::checkIfJackknifeCanBePerformedWithBinsize(int binsize)
{
	if(binsize < 1 || binsize >= numberOfElements)
		throw std::invalid_argument("Cannot create jackknifeEstimators with this binsize!");
}

