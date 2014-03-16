#include<iostream>
#include "dataSample.hpp"

dataSample::dataSample()
{
	values = std::valarray<double>(defaultSizeOfDataSample);
	numberOfElements = values.size();
	initMoments();
}

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	checkIfNumberOfElementsIsValid();
	initMoments();
}

void dataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<double>(numberOfMoments,0);
	//todo: calc moments on demand...
	for (int iteration = 0; iteration < moments.size(); iteration++)
	{
		moments[iteration] = calcNthMoment(iteration);
	}
}

int dataSample::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
}

int dataSample::getNumberOfElements()
{
	return numberOfElements;
}

void dataSample::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}

double dataSample::calcNthMoment(int n)
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

double dataSample::calcNthMomentExplicit(int n)
{
	std::valarray<double> sampleToNthPower = std::pow(values, double(n));
	return sampleToNthPower.sum() / numberOfElements;
}

double dataSample::calcFirstMomentExplicit()
{
	return values.sum() / numberOfElements;
}

double dataSample::getMean()
{
	return getNthMoment(1);
}

double dataSample::getVariance()
{
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) );
}

double dataSample::getNthMoment(int n)
{
	checkIfNIsValid(n);
	return moments[n];
}

int dataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int dataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}

dataSample dataSample::createBinnedDataSampleWithNumberOfBins(int numberOfBins)
{
	checkIfNumberOfBinsIsValid(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	return performBinning(numberOfBins, binsize);
}

dataSample dataSample::createBinnedDataSampleWithBinsize(int binsize)
{
	checkIfBinsizeIsValid(binsize);
	int numberOfBins = calcBinsize(binsize);
	return performBinning(numberOfBins, binsize);
}

void dataSample::checkIfNumberOfBinsIsValid(int numberOfBins)
{
	if(numberOfBins <= 0)
		throw std::invalid_argument("Cannot perform binning with zero or less bins!");
	if(numberOfBins > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with number of bins bigger than number of datapoints!");
}

void dataSample::checkIfBinsizeIsValid(int binsize)
{
	if(binsize <= 0)
		throw std::invalid_argument("Cannot perform binning with binsize of zero or less!");
	if(binsize > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with binsize bigger than number of datapoints!");
}

int dataSample::calcBinsize(int numberOfBins)
{
	return numberOfElements / numberOfBins;
}

int dataSample::calcNumberOfBins(int binsize)
{
	if (numberOfElements % binsize != 0)
		std::cout << "Warning: binsize is not a multiple of numberOfElements!" << std::endl;
	return numberOfElements / binsize;
}

dataSample dataSample::performBinning(int numberOfBins, int binsize)
{
  std::valarray<double> binnedDataSample(numberOfBins);
//  for(int i=0; i<nbins; i++)
//    binned_data[i]=mean_value(x[std::slice(i*binsize, binsize, 1)]);
  dataSample dataSampleInstance(binnedDataSample);
  return dataSampleInstance;
}

void dataSample::checkIfNumberOfElementsIsValid()
{
	if(numberOfElements <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
}
