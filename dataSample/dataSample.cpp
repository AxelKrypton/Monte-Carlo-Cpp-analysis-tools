#include <iostream>
#include <fstream>
#include <sstream>
#include <gmp.h>
#include "dataSample.hpp"

DataSample::DataSample(bool isJackknifeSample):
	isJackknifeSample(isJackknifeSample)
{
	values = std::valarray<double>(defaultSizeOfDataSample);
	initMembers();
}

DataSample::DataSample(std::valarray<double> valuesIn, bool isJackknifeSample):
	isJackknifeSample(isJackknifeSample)
{
	values = valuesIn;
	initMembers();
}

//todo: add offset parameter
DataSample::DataSample(std::string dataFilename, int column, bool isJackknifeSample):
	isJackknifeSample(isJackknifeSample)
{
	values = readDataFromFile(dataFilename, column);
	initMembers();
}

void DataSample::initMembers()
{
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
	return sampleToNthPower.sum() / (double) numberOfElements;
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

void DataSample::checkIfColumnIsValid(int column)
{
	if(column <= 0)
		throw std::invalid_argument("Numbers of columns must be bigger than zero!");
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

//todo: refactor
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

int DataSample::getJackknifeNormalization()
{
	if(numberOfElements <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
	return numberOfElements - 1;
}

//todo: refactor
DataSample DataSample::createJackknifeEstimators()
{
	int normalization = getJackknifeNormalization();
	double sumOfDataSampleElements = values.sum();
	std::valarray<double> jackknifeEstimators = (sumOfDataSampleElements - values) / normalization;
	DataSample dataSample(jackknifeEstimators, true);
	return dataSample;
}

DataSample DataSample::applyFunction(double (*function)(double))
{
	std::valarray<double> functionAppliedToArray = values.apply(function);
	DataSample dataSample(functionAppliedToArray);
	return dataSample;
}

void DataSample::checkIfSampleIsJackknifeSample()
{
	if ( ! isJackknifeSample )
		throw std::logic_error("DataSample is not based on jackknife estimate!");
}

//todo: examine why these two impl. give different results
double DataSample::getJackknifeVariance()
{
	checkIfSampleIsJackknifeSample();
	return (pow((values - getNthMoment(1)), 2.)).sum() * (numberOfElements  - 1.) / numberOfElements;
}

double DataSample::getJackknifeVariance_v2()
{
	checkIfSampleIsJackknifeSample();
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) ) * (numberOfElements - 1.);
}

double DataSample::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

double defaultFunction(double in)
{
	return in;
}

void DataSample::checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file does not exist!");
	file.close();
}

//todo: refactor, perhaps think about column numbering
std::valarray<double> DataSample::readDataFromFile(std::string filename, int column)
{
	std::ifstream infile;
	std::string line;
	std::vector<double> data;
	double aux;

	checkIfDatafileExists(filename);
	checkIfColumnIsValid(column);

	infile.open(filename.c_str());

	while (std::getline(infile, line))
	{
		if(line[0] != '#') //ignore lines beginning by # since they are comments for gnuplot
		{
			std::stringstream ss(line);
			for(int i=0; i<column; i++)
			{
				if(ss >> aux)
				{
					if(i==column-1)
						data.push_back(aux);
				}
				else
				{
					throw std::runtime_error("Error reading datafile");
				}
			}
		}
	}
	//todo: this is not covered in a test yet...
	if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
	{
		throw std::runtime_error("Error reading datafile");
	}
	infile.close();

	return std::valarray<double>(data.data(), data.size());
}
