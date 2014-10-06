#include "binnedDataSample.hpp"

#include "../IO/io_utilities.hpp"
#include "../executables/exceptions.hpp"

static void printBinningInformation(int numberOfBins, int binsize)
{
	std::cout << "# Perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
}

DataSampleBasic BinnedDataSample::performBinning(DataSampleBasic sampleIn)
{
	if(warningOutput)
		printBinningInformation(numberOfBins, binsize);
	DataSampleBasic binnedDataSample(numberOfBins);
	for(int iteration = 0; iteration < numberOfBins; iteration++)
	{
		binnedDataSample[iteration] = DataSample(sampleIn.sampleSlice(iteration*binsize, binsize, 1)).getNthMoment(1);
	}
	return binnedDataSample;
}

void BinnedDataSample::checkDiscardedElements(int valueIn, std::string descriptionIn, int elementsOfSample)
{
	discardedElements = elementsOfSample % valueIn;
	if (discardedElements != 0)
	{
		if(warningOutput){
			std::cout << "Warning: " << descriptionIn << " is not a multiple of numberOfElements!" << std::endl;
			std::cout << discardedElements<< " elements are discarded!" << std::endl;
		}
		binningFitsBinsize = false;
		if ( binningMustFitSize )
		{
			throw wrongBinningParameter();
		}
	}
	else
	{
		binningFitsBinsize = true;
	}
}

BinnedDataSampleFromNumberOfBins::BinnedDataSampleFromNumberOfBins(DataSampleBasic sampleIn, int numberOfBinsIn, bool requireBinningToMatchSize, bool warningOutputIn)
{
	binningMustFitSize = requireBinningToMatchSize;
	warningOutput = warningOutputIn;
	numberOfBins = numberOfBinsIn;
	checkIfNumberOfBinsIsValid(sampleIn.getNumberOfElements());
	calcBinsize(sampleIn.getNumberOfElements());
	setValues(performBinning(sampleIn));
}

static void checkIfBinningParameterIsValid(int valueIn, std::string descriptionIn, int numberOfElements)
{
	if(valueIn <= 0)
		throw std::invalid_argument("Cannot perform binning with " +  descriptionIn + " zero or less!");
	if(valueIn > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with " + descriptionIn + " bigger than number of datapoints!");
}

void BinnedDataSampleFromNumberOfBins::checkIfNumberOfBinsIsValid(int elementsOfSample)
{
	checkIfBinningParameterIsValid(numberOfBins, "numberOfBins", elementsOfSample);
}

void BinnedDataSampleFromNumberOfBins::calcBinsize(int elementsOfSample)
{
	checkDiscardedElements(numberOfBins, "numberOfBins", elementsOfSample);
	binsize = elementsOfSample / numberOfBins;
}

void BinnedDataSampleFromBinsize::checkIfBinsizeIsValid(int elementsOfSample)
{
	checkIfBinningParameterIsValid(binsize, "binsize", elementsOfSample);
}

void BinnedDataSampleFromBinsize::calcNumberOfBins(int elementsOfSample)
{
	checkDiscardedElements(binsize, "binsize", elementsOfSample);
	numberOfBins = elementsOfSample / binsize;
}

BinnedDataSampleFromBinsize::BinnedDataSampleFromBinsize(DataSampleBasic sampleIn, int binsizeIn, bool requireBinningToMatchSize, bool warningOutputIn)
{
	binningMustFitSize = requireBinningToMatchSize;
	warningOutput = warningOutputIn;
	binsize = binsizeIn;
	checkIfBinsizeIsValid(sampleIn.getNumberOfElements());
	calcNumberOfBins(sampleIn.getNumberOfElements());
	setValues(performBinning(sampleIn));
}

