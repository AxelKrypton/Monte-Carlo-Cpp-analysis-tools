#include "binnedDataSample.hpp"

DataSampleBasic BinnedDataSample::performBinning(DataSampleBasic sampleIn)
{
	std::cout << "perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
	DataSampleBasic binnedDataSample(numberOfBins);
	for(int iteration = 0; iteration < numberOfBins; iteration++)
	{
		binnedDataSample[iteration] = DataSampleAnalyzer(sampleIn.sampleSlice(iteration*binsize, binsize, 1)).getNthMoment(1);
	}
	return binnedDataSample;
}

void BinnedDataSample::checkDiscardedElements(int valueIn, std::string descriptionIn, int elementsOfSample)
{
	int discardedElements = elementsOfSample % valueIn;
	if (discardedElements != 0)
	{
		std::cout << "Warning: " << descriptionIn << " is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements<< " elements are discarded!" << std::endl;
	}
}

BinnedDataSampleFromNumberOfBins::BinnedDataSampleFromNumberOfBins(DataSampleBasic sampleIn, int numberOfBinsIn)
{
	numberOfBins = numberOfBinsIn;
	checkIfNumberOfBinsIsValid(sampleIn.getNumberOfElements());
	calcBinsize(sampleIn.getNumberOfElements());
	setValues(performBinning(sampleIn));
}

//todo: rename again!
static void checkIfBinningParameterIsValid_toberenamed(int valueIn, std::string descriptionIn, int numberOfElements)
{
	if(valueIn <= 0)
		throw std::invalid_argument("Cannot perform binning with " +  descriptionIn + " zero or less!");
	if(valueIn > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with " + descriptionIn + " bigger than number of datapoints!");
}

void BinnedDataSampleFromNumberOfBins::checkIfNumberOfBinsIsValid(int elementsOfSample)
{
	checkIfBinningParameterIsValid_toberenamed(numberOfBins, "numberOfBins", elementsOfSample);
}

void BinnedDataSampleFromNumberOfBins::calcBinsize(int elementsOfSample)
{
	checkDiscardedElements(numberOfBins, "numberOfBins", elementsOfSample);
	binsize = elementsOfSample / numberOfBins;
}

void BinnedDataSampleFromBinsize::checkIfBinsizeIsValid(int elementsOfSample)
{
	checkIfBinningParameterIsValid_toberenamed(binsize, "binsize", elementsOfSample);
}

void BinnedDataSampleFromBinsize::calcNumberOfBins(int elementsOfSample)
{
	checkDiscardedElements(binsize, "binsize", elementsOfSample);
	numberOfBins = elementsOfSample / binsize;
}

BinnedDataSampleFromBinsize::BinnedDataSampleFromBinsize(DataSampleBasic sampleIn, int binsizeIn)
{
	binsize = binsizeIn;
	checkIfBinsizeIsValid(sampleIn.getNumberOfElements());
	calcNumberOfBins(sampleIn.getNumberOfElements());
	setValues(performBinning(sampleIn));
}

