#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "DataSampleBasic.hpp"
#include "FileReader.hpp"

DataSampleBasic::DataSampleBasic(int length)
{
	setValues(std::valarray<double>(length));
}

DataSampleBasic::DataSampleBasic(std::valarray<double> valuesIn)
{
	setValues(valuesIn);
}

DataSampleBasic::DataSampleBasic(std::string dataFilename, int column, int offset, bool* isAnyEntryBad)
{
	setValues(readDataFromFile(dataFilename, column, offset, isAnyEntryBad));
}

void DataSampleBasic::setValues(std::valarray<double> valuesIn)
{
	checkIfNumberOfElementsIsValid(valuesIn.size());
	values = valuesIn;
	numberOfElements = valuesIn.size();
}

void DataSampleBasic::setValues(DataSampleBasic sampleIn)
{
	numberOfElements = sampleIn.getNumberOfElements();
	values = sampleIn.values;
}

static void checkDivisionFactor(double factorIn)
{
	if(factorIn == 0.)
	{
		throw std::invalid_argument("Cannot divide by zero!");
	}
}

void checkNumberOfElements(int lhs, int rhs)
{
	if (lhs != rhs)
		throw std::invalid_argument("DataSamples have different number of elements!");
}

double DataSampleBasic::sum()
{
	return values.sum();
}

double DataSampleBasic::max()
{
    return values.max();
}

double DataSampleBasic::min()
{
    return values.min();
}

double& DataSampleBasic::operator[](size_t index)
{
	return values[index];
}

DataSampleBasic& DataSampleBasic::operator+=(double factor)
{
	values += factor;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator+=(DataSampleBasic sampleIn)
{
	checkNumberOfElements(numberOfElements, sampleIn.getNumberOfElements());
	values += sampleIn.values;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator-=(double factor)
{
	values -= factor;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator-=(DataSampleBasic sampleIn)
{
	checkNumberOfElements(numberOfElements, sampleIn.getNumberOfElements());
	values -= sampleIn.values;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator*=(double factor)
{
	values *= factor;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator*=(DataSampleBasic sampleIn)
{
	checkNumberOfElements(numberOfElements, sampleIn.getNumberOfElements());
	values *= sampleIn.values;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator/=(double factor)
{
	checkDivisionFactor(factor);
	values /= factor;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator/=(DataSampleBasic sampleIn)
{
	checkNumberOfElements(numberOfElements, sampleIn.getNumberOfElements());
	values /= sampleIn.values;
	return *this;
}

DataSampleBasic& DataSampleBasic::operator^=(int n)
{
	values = std::pow(values, double(n));
	return *this;
}

DataSampleBasic& DataSampleBasic::operator^=(double n)
{
	values = std::pow(values, n);
	return *this;
}

DataSampleBasic operator+(DataSampleBasic sampleIn, double factor)
{
	sampleIn += factor;
	return sampleIn;
}

DataSampleBasic operator+(double factor, DataSampleBasic sampleIn)
{
	sampleIn += factor;
	return sampleIn;
}

DataSampleBasic operator+(DataSampleBasic lhs, DataSampleBasic rhs)
{
	checkNumberOfElements(lhs.getNumberOfElements(), rhs.getNumberOfElements());
	lhs += rhs;
	return lhs;
}

DataSampleBasic operator-(DataSampleBasic sampleIn, double factor)
{
	sampleIn -= factor;
	return sampleIn;
}

DataSampleBasic operator-(double factor, DataSampleBasic sampleIn)
{
	sampleIn -= factor;
	sampleIn *= -1.;
	return sampleIn;
}

DataSampleBasic operator-(DataSampleBasic lhs, DataSampleBasic rhs)
{
	checkNumberOfElements(lhs.getNumberOfElements(), rhs.getNumberOfElements());
	lhs -= rhs;
	return lhs;
}

DataSampleBasic operator*(DataSampleBasic sampleIn, double factor)
{
	sampleIn *= factor;
	return sampleIn;
}

DataSampleBasic operator*(double factor, DataSampleBasic sampleIn)
{
	sampleIn *= factor;
	return sampleIn;
}

DataSampleBasic operator*(DataSampleBasic lhs, DataSampleBasic rhs)
{
	checkNumberOfElements(lhs.getNumberOfElements(), rhs.getNumberOfElements());
	lhs *= rhs;
	return lhs;
}

DataSampleBasic operator/(DataSampleBasic sampleIn, double factor)
{
	checkDivisionFactor(factor);
	return sampleIn /= factor;
}

DataSampleBasic operator/(double factor, DataSampleBasic sampleIn)
{
	for(int i=0; i<sampleIn.getNumberOfElements(); i++)
		checkDivisionFactor(sampleIn[i]);
	sampleIn ^= -1.;
	return sampleIn *= factor;
}

DataSampleBasic operator/(DataSampleBasic lhs, DataSampleBasic rhs)
{
	checkNumberOfElements(lhs.getNumberOfElements(), rhs.getNumberOfElements());
	lhs/=rhs;
	return lhs;
}

DataSampleBasic operator^(DataSampleBasic sampleIn, int n)
{
	return sampleIn ^= n;
}

DataSampleBasic operator^(DataSampleBasic sampleIn, double n)
{
	return sampleIn ^= n;
}

int DataSampleBasic::getNumberOfElements()
{
	return numberOfElements;
}

void DataSampleBasic::checkIfNumberOfElementsIsValid(int length)
{
	if(length <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
	//todo: think about better warning!
	//if(length > roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid)
		//std::cout << "Warning: the datasize is such that double precision may not be valid anymore (depending on the data)!" << std::endl;
}

double defaultFunction(double in)
{
	return in;
}

DataSampleBasic DataSampleBasic::applyFunction(double (*function)(double))
{
	std::valarray<double> functionAppliedToArray = values.apply(function);
	DataSampleBasic dataSample(functionAppliedToArray);
	return dataSample;
}

DataSampleBasic DataSampleBasic::readDataFromFile(std::string filename, int column, int offset, bool* isAnyEntryBad)
{
	FileReader reader(filename, column, offset);
	return reader.readDataFromFile(isAnyEntryBad);
}

void DataSampleBasic::checkSliceParameters(int start, int size, int stride)
{
	if( start < 0 || start >= numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"start\" must be between 0 and (number of datapoints -1)!");
	if( size < 1 || size > numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"size\" must be between 1 and number of datapoints!");
	if( stride < 1 || stride >= numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"stride\" must be between 1 and number of datapoints!");
	if( (start + (stride*size) ) > numberOfElements)
		throw std::invalid_argument("product of sampleSlice parameters \"slice\" and \"stride\" must be between 1 and number of datapoints!");
}

DataSampleBasic DataSampleBasic::sampleSlice(int start, int size, int stride)
{
	checkSliceParameters(start, size, stride);
	return DataSampleBasic(values[std::slice(start, size, stride)]);
}

static void checkRemoveParameter(int position, int numberOfElements)
{
	if (position < 0 || position >= numberOfElements)
			throw std::invalid_argument("Can only remove element greater than or equal to zero and smaller than the number of entries!");
}

DataSampleBasic DataSampleBasic::removeIthElement(int i)
{
	checkRemoveParameter(i, numberOfElements);
	DataSampleBasic tmp(numberOfElements - 1);
	for (int iteration = 0; iteration < numberOfElements; iteration ++)
	{
		if (iteration == i)
		{
			continue;
		}
		int index = (iteration < i) ? iteration : iteration - 1;

		tmp[index] = values[iteration];
	}
	return tmp;
}

DataSampleBasic DataSampleBasic::removeLastNElements(int n)
{
	checkRemoveParameter(n, numberOfElements);
	return DataSampleBasic(values[std::slice(0, numberOfElements - n, 1)]);
}

