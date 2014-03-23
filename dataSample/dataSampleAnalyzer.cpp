#include <iostream>
#include "dataSampleAnalyzer.hpp"

void DataSampleAnalyzer::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<Moment>(numberOfMoments);
	centralMoments = std::vector<Moment>(numberOfMoments);
}

double DataSampleAnalyzer::getNthMoment(int n)
{
	checkIfNIsValid(n);
	if (!moments[n].calculated)
	{
		moments[n].set(calcNthMoment(n));
	}
	return moments[n].value;
}

double DataSampleAnalyzer::calcNthMoment(int n)
{
	if ( n == 1)
	{
		return calcFirstMomentExplicit();
	}
	else
	{
		return calcNthMomentExplicit(n);
	}
}

double DataSampleAnalyzer::calcNthMomentExplicit(int n)
{
	std::valarray<double> sampleToNthPower = std::pow(values, double(n));
	return sampleToNthPower.sum() / (double) numberOfElements;
}

double DataSampleAnalyzer::calcFirstMomentExplicit()
{
	return values.sum() / numberOfElements;
}

double DataSampleAnalyzer::getNthCentralMoment(int n)
{
	checkIfNIsValid(n);
	if (!centralMoments[n].calculated)
	{
		centralMoments[n].set(calcNthCentralMoment(n));
	}
	return centralMoments[n].value;
}

double DataSampleAnalyzer::calcNthCentralMoment(int n)
{
	if ( n == 0 || n == 1)
	{
		return 0.;
	}
	else
	{
		return calcNthCentralMomentExplicit(n);
	}
}

double DataSampleAnalyzer::calcNthCentralMomentExplicit(int n)
{
	return (createShiftedDataSample(2, getNthMoment(1))).sum()  / numberOfElements;
}


int DataSampleAnalyzer::calcBinsize(int numberOfBins)
{
	int discardedElements = numberOfElements % numberOfBins;
	if (discardedElements != 0)
	{
		std::cout << "Warning: numberOfBins is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements << " elements are discarded!" << std::endl;
	}
	return numberOfElements / numberOfBins;
}

int DataSampleAnalyzer::calcNumberOfBins(int binsize)
{
	int discardedElements = numberOfElements % binsize;
	if (discardedElements != 0)
	{
		std::cout << "Warning: binsize is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements << " elements are discarded!" << std::endl;
	}
	return numberOfElements / binsize;
}

//todo: refactor
DataSample DataSampleAnalyzer::performBinning(int numberOfBins, int binsize)
{
	std::cout << "perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
  std::valarray<double> binnedDataSample(numberOfBins);
  for(int iteration = 0; iteration < numberOfBins; iteration++)
  {
	  std::valarray<double> sliceOfData = values[std::slice(iteration*binsize, binsize, 1)];
	  DataSampleAnalyzer temporarySample(sliceOfData);
	  binnedDataSample[iteration] = temporarySample.getNthMoment(1);
  }
  DataSample dataSampleInstance(binnedDataSample);
  return dataSampleInstance;
}
int DataSampleAnalyzer::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
}


int DataSampleAnalyzer::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int DataSampleAnalyzer::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}

DataSample DataSampleAnalyzer::createBinnedDataSampleWithNumberOfBins(int numberOfBins)
{
	checkIfNumberOfBinsIsValid(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	return performBinning(numberOfBins, binsize);
}

DataSample DataSampleAnalyzer::createBinnedDataSampleWithBinsize(int binsize)
{
	checkIfBinsizeIsValid(binsize);
	int numberOfBins = calcNumberOfBins(binsize);
	return performBinning(numberOfBins, binsize);
}

void DataSampleAnalyzer::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}
void DataSampleAnalyzer::checkIfNumberOfBinsIsValid(int numberOfBins)
{
	if(numberOfBins <= 0)
		throw std::invalid_argument("Cannot perform binning with zero or less bins!");
	if(numberOfBins > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with number of bins bigger than number of datapoints!");
}

void DataSampleAnalyzer::checkIfBinsizeIsValid(int binsize)
{
	if(binsize <= 0)
		throw std::invalid_argument("Cannot perform binning with binsize of zero or less!");
	if(binsize > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with binsize bigger than number of datapoints!");
}
