#include<iostream>
#include "dataSample.hpp"

DataSample::DataSample()
{
	values = std::valarray<double>(defaultSizeOfDataSample);
	numberOfElements = values.size();
	initMoments();
}

DataSample::DataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	checkIfNumberOfElementsIsValid();
	initMoments();
}

void DataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<double>(numberOfMoments,0);
	//todo: calc moments on demand...
	for (int iteration = 0; iteration < moments.size(); iteration++)
	{
		moments[iteration] = calcNthMoment(iteration);
	}
}

int DataSample::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
}

int DataSample::getNumberOfElements()
{
	return numberOfElements;
}

void DataSample::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
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
	std::valarray<double> sampleToNthPower = std::pow(values, double(n));
	return sampleToNthPower.sum() / numberOfElements;
}

double DataSample::calcFirstMomentExplicit()
{
	return values.sum() / numberOfElements;
}

double DataSample::getMean()
{
	return getNthMoment(1);
}

double DataSample::getVariance()
{
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) );
}

double DataSample::getNthMoment(int n)
{
	checkIfNIsValid(n);
	return moments[n];
}

int DataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int DataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}

DataSample DataSample::createBinnedDataSampleWithNumberOfBins(int numberOfBins)
{
	checkIfNumberOfBinsIsValid(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	return performBinning(numberOfBins, binsize);
}

DataSample DataSample::createBinnedDataSampleWithBinsize(int binsize)
{
	checkIfBinsizeIsValid(binsize);
	int numberOfBins = calcBinsize(binsize);
	return performBinning(numberOfBins, binsize);
}

void DataSample::checkIfNumberOfElementsIsValid()
{
	if(numberOfElements <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
}

void DataSample::checkIfNumberOfBinsIsValid(int numberOfBins)
{
	if(numberOfBins <= 0)
		throw std::invalid_argument("Cannot perform binning with zero or less bins!");
	if(numberOfBins > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with number of bins bigger than number of datapoints!");
}

void DataSample::checkIfBinsizeIsValid(int binsize)
{
	if(binsize <= 0)
		throw std::invalid_argument("Cannot perform binning with binsize of zero or less!");
	if(binsize > numberOfElements)
		throw std::invalid_argument("Cannot perform binning with binsize bigger than number of datapoints!");
}

int DataSample::calcBinsize(int numberOfBins)
{
	return numberOfElements / numberOfBins;
}

int DataSample::calcNumberOfBins(int binsize)
{
	if (numberOfElements % binsize != 0)
		std::cout << "Warning: binsize is not a multiple of numberOfElements!" << std::endl;
	return numberOfElements / binsize;
}

DataSample DataSample::performBinning(int numberOfBins, int binsize)
{
  std::valarray<double> binnedDataSample(numberOfBins);
  for(int iteration = 0; iteration < numberOfBins; iteration++)
  {
	  std::valarray<double> sliceOfData = values[std::slice(iteration*binsize, binsize, 1)];
	  DataSample temporarySample(sliceOfData);
	  binnedDataSample[iteration] = temporarySample.getMean();
  }
  DataSample dataSampleInstance(binnedDataSample);
  return dataSampleInstance;
}

DataSample DataSample::createJackknifeEstimators()
{
	int normalization = numberOfElements - 1;
	double sumOfDataSampleElements = values.sum();
	std::valarray<double> jackknifeEstimators = (sumOfDataSampleElements - values) / normalization;
	DataSample dataSample(jackknifeEstimators);
	return dataSample;
}

DataSample DataSample::applyFunction(double (*function)(double))
{
	std::valarray<double> functionAppliedToArray = values.apply(function);
	DataSample dataSample(functionAppliedToArray);
	return dataSample;
}

double DataSample::getJackknifeVariance()
{
	bool isJackknifeSample = false;
	if ( ! isJackknifeSample )
		throw std::logic_error("DataSample is not based on jackknife estimate!");
	return 0;
}
