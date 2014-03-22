#include <iostream>
#include <fstream>
#include <sstream>
#include "dataSample.hpp"

DataSample::DataSample(int length)
{
	checkIfNumberOfElementsIsValid(length);
	values = std::valarray<double>(length);
	initMembers();
}

DataSample::DataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	initMembers();
	checkIfNumberOfElementsIsValid(numberOfElements);
}

DataSample::DataSample(std::string dataFilename, int column, int offset)
{
	values = readDataFromFile(dataFilename, column, offset);
	initMembers();
	checkIfNumberOfElementsIsValid(numberOfElements);
}

void DataSample::initMembers()
{
	numberOfElements = values.size();
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

void DataSample::checkIfOffsetIsValid(int offset)
{
	if(offset < 0)
		throw std::invalid_argument("Offset must be greater than or equal to zero!");
	if(offset > 0)
		throw std::invalid_argument("Usage of offset parameter is not implemented yet. Aborting!");
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

double DataSample::getNthCentralMoment(int n)
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

DataSample DataSample::createShiftedDataSample(int order, double shift)
{
	return DataSample( pow((values - shift), double(order)) );
}

double DataSample::sum()
{
	return values.sum();
}

double DataSample::calcNthCentralMomentExplicit(int n)
{
	checkIfNIsValid(n);
	return (createShiftedDataSample(2, getNthMoment(1))).sum()  / numberOfElements;
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
	int numberOfBins = calcNumberOfBins(binsize);
	return performBinning(numberOfBins, binsize);
}

void DataSample::checkIfNumberOfElementsIsValid(int length)
{
	if(length <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
	//todo: think about better warning!
	if(length > roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid)
		std::cout << "Warning: the datasize is such that double precision may not be valid anymore (depending on the data)!" << std::endl;
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
	int discardedElements = numberOfElements % numberOfBins;
	if (discardedElements != 0)
	{
		std::cout << "Warning: numberOfBins is not a multiple of numberOfElements!" << std::endl;
		std::cout << discardedElements << " elements are discarded!" << std::endl;
	}
	return numberOfElements / numberOfBins;
}

int DataSample::calcNumberOfBins(int binsize)
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
DataSample DataSample::performBinning(int numberOfBins, int binsize)
{
	std::cout << "perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;
  std::valarray<double> binnedDataSample(numberOfBins);
  for(int iteration = 0; iteration < numberOfBins; iteration++)
  {
	  std::valarray<double> sliceOfData = values[std::slice(iteration*binsize, binsize, 1)];
	  DataSample temporarySample(sliceOfData);
	  binnedDataSample[iteration] = temporarySample.getNthMoment(1);
  }
  DataSample dataSampleInstance(binnedDataSample);
  return dataSampleInstance;
}

double defaultFunction(double in)
{
	return in;
}

DataSample DataSample::applyFunction(double (*function)(double))
{
	std::valarray<double> functionAppliedToArray = values.apply(function);
	DataSample dataSample(functionAppliedToArray);
	return dataSample;
}

//todo: print filename, perhaps path
void DataSample::checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file does not exist!");
	file.close();
}

//todo: refactor, perhaps think about column numbering
//todo: implement offset
std::valarray<double> DataSample::readDataFromFile(std::string filename, int column, int offset)
{
	std::ifstream infile;
	std::string line;
	std::vector<double> data;
	double aux;

	checkIfDatafileExists(filename);
	checkIfColumnIsValid(column);
	checkIfOffsetIsValid(offset);

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
	//Note: this happens for example if an empty line is contained in the file
	if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
	{
		throw std::runtime_error("Error reading datafile");
	}
	infile.close();

	return std::valarray<double>(data.data(), data.size());
}


//todo: examine why these two impl. give different results
double JackknifeEstimators::getJackknifeVariance()
{
	return (pow((values - getNthMoment(1)), 2.)).sum() * (numberOfElements  - 1.) / numberOfElements;
}

double JackknifeEstimators::getJackknifeVariance_v2()
{
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) ) * (numberOfElements - 1.);
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed()
{
	if(numberOfElements <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed();
	return numberOfElements - 1;
}

void JackknifeEstimatorsFromBinning::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}
