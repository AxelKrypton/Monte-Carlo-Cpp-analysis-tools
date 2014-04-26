#include <iostream>
#include <sstream>
#include "DataSample.hpp"

void DataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<Moment>(numberOfMoments);
	centralMoments = std::vector<Moment>(numberOfMoments);
}

double DataSample::getNthMoment(int n)
{
	checkIfNIsValid(n);
	if (!moments[n].calculated)
	{
		moments[n].set(calcNthMoment(n));
	}
	return moments[n].value;
}

double DataSample::calcNthMoment(int n)
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

double DataSample::calcNthMomentExplicit(int n)
{
	return (*this^n).sum() / (double) numberOfElements;
}

double DataSample::calcFirstMomentExplicit()
{
	return sum() / numberOfElements;
}

double DataSample::getNthCentralMoment(int n)
{
	checkIfNIsValid(n);
	if (!centralMoments[n].calculated)
	{
		centralMoments[n].set(calcNthCentralMoment(n));
	}
	return centralMoments[n].value;
}

double DataSample::calcNthCentralMoment(int n)
{
	if ( n == 0 )
	{
		return 1.;
	}
	else if (n == 1)
	{
		return 0.;
	}
	else
	{
		return calcNthCentralMomentExplicit(n);
	}
}

double DataSample::calcNthCentralMomentExplicit(int n)
{
	return ( (*this - getNthMoment(1) )^( (double(n)) )  ).sum()  / numberOfElements;
}

void DataSample::checkDiscardedElements(int valueIn, std::string descriptionIn)
{
	int discardedElements = numberOfElements % valueIn;
	if (discardedElements != 0)
	{
		std::cout << "Warning: " << descriptionIn << " is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements<< " elements are discarded!" << std::endl;
	}
}

int DataSample::calcBinsize(int numberOfBins)
{
	checkDiscardedElements(numberOfBins, "numberOfBins");
	return numberOfElements / numberOfBins;
}

int DataSample::calcNumberOfBins(int binsize)
{
	checkDiscardedElements(binsize, "binsize");
	return numberOfElements / binsize;
}

DataSampleBasic DataSample::performBinning(int numberOfBins, int binsize)
{
	std::cout << "perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
	DataSampleBasic binnedDataSample(numberOfBins);
	for(int iteration = 0; iteration < numberOfBins; iteration++)
	{
		binnedDataSample[iteration] = DataSample(sampleSlice(iteration*binsize, binsize, 1)).getNthMoment(1);
	}
	return binnedDataSample;
}
int DataSample::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
}

int DataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int DataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}

DataSampleBasic DataSample::createBinnedDataSampleWithNumberOfBins(int numberOfBins)
{
	checkIfNumberOfBinsIsValid(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	return performBinning(numberOfBins, binsize);
}

DataSampleBasic DataSample::createBinnedDataSampleWithBinsize(int binsize)
{
	checkIfBinsizeIsValid(binsize);
	int numberOfBins = calcNumberOfBins(binsize);
	return performBinning(numberOfBins, binsize);
}

void DataSample::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}

void DataSample::checkIfBinningParameterIsValid(int valueIn, std::string descriptionIn)
{
	if(valueIn <= 0)
		throw std::invalid_argument("Cannot perform binning with " +  descriptionIn + " zero or less!");
	if(valueIn > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with " + descriptionIn + " bigger than number of datapoints!");
}

void DataSample::checkIfNumberOfBinsIsValid(int numberOfBins)
{
	checkIfBinningParameterIsValid(numberOfBins, "numberOfBins");
}

void DataSample::checkIfBinsizeIsValid(int binsize)
{
	checkIfBinningParameterIsValid(binsize, "binsize");
}
