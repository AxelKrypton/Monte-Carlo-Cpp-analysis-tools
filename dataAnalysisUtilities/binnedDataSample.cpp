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
			std::cout << "# \033[0;33m\033[1mWarning:\033[21m " << descriptionIn << " is not a multiple of \033[1m"<< elementsOfSample << "\033[21m: \033[1m";
			std::cout << discardedElements << "\033[21m elements are \033[1mdiscarded!\033[0m" << std::endl;
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

void resizeRawDataSample(DataSample & rawData, BinnedDataSample & binnedData, bool adjustDataSampleSizeToBinning)
{
	if( adjustDataSampleSizeToBinning && !binnedData.doesBinningFitBinsize() )
	{
		std::cout << "# Adjusting data sample size..." << std::endl;
		rawData = rawData.removeLastNElements(binnedData.getNumberOfDiscardedElements() );
	}
	else if (!binnedData.doesBinningFitBinsize() )
	{
		std::cout << "# WARNING: Elements are discarded for binned quantities only!" << std::endl;
	}
}
