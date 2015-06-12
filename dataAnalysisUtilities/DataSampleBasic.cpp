#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "DataSampleBasic.hpp"
#include "FileReader.hpp"

DataSampleBasic::DataSampleBasic(int length)
{
	setValues(std::valarray<realFloat>(length));
}

DataSampleBasic::DataSampleBasic(std::valarray<realFloat> valuesIn)
{
	setValues(valuesIn);
}

DataSampleBasic::DataSampleBasic(std::string dataFilename, int column, int offset, bool* isAnyEntryBad)
{
	setValues(readDataFromFile(dataFilename, column, offset, isAnyEntryBad));
}

void DataSampleBasic::setValues(std::valarray<realFloat> valuesIn)
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

static void checkDivisionFactor(realFloat factorIn)
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

realFloat DataSampleBasic::sum()
{
	return values.sum();
}

realFloat DataSampleBasic::max()
{
    return values.max();
}

realFloat DataSampleBasic::min()
{
    return values.min();
}

realFloat& DataSampleBasic::operator[](size_t index)
{
	return values[index];
}

DataSampleBasic& DataSampleBasic::operator+=(realFloat factor)
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

DataSampleBasic& DataSampleBasic::operator-=(realFloat factor)
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

DataSampleBasic& DataSampleBasic::operator*=(realFloat factor)
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

DataSampleBasic& DataSampleBasic::operator/=(realFloat factor)
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
	values = std::pow(values, realFloat(n));
	return *this;
}

DataSampleBasic& DataSampleBasic::operator^=(realFloat n)
{
	values = std::pow(values, n);
	return *this;
}

DataSampleBasic operator+(DataSampleBasic sampleIn, realFloat factor)
{
	sampleIn += factor;
	return sampleIn;
}

DataSampleBasic operator+(realFloat factor, DataSampleBasic sampleIn)
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

DataSampleBasic operator-(DataSampleBasic sampleIn, realFloat factor)
{
	sampleIn -= factor;
	return sampleIn;
}

DataSampleBasic operator-(realFloat factor, DataSampleBasic sampleIn)
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

DataSampleBasic operator*(DataSampleBasic sampleIn, realFloat factor)
{
	sampleIn *= factor;
	return sampleIn;
}

DataSampleBasic operator*(realFloat factor, DataSampleBasic sampleIn)
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

DataSampleBasic operator/(DataSampleBasic sampleIn, realFloat factor)
{
	checkDivisionFactor(factor);
	return sampleIn /= factor;
}

DataSampleBasic operator/(realFloat factor, DataSampleBasic sampleIn)
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

DataSampleBasic operator^(DataSampleBasic sampleIn, realFloat n)
{
	return sampleIn ^= n;
}

bool operator==(DataSampleBasic lhs, DataSampleBasic rhs)
{
    if(lhs.getNumberOfElements() != rhs.getNumberOfElements())
        return false;

   std::valarray<bool> comp(lhs.getNumberOfElements());
   for(int i=0; i<lhs.getNumberOfElements(); i++)
       comp[i] = (lhs[i]==rhs[i]);

   return comp.min();
}

bool operator!=(DataSampleBasic lhs, DataSampleBasic rhs)
{
   return !(lhs==rhs);
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
		//std::cout << "Warning: the datasize is such that realFloat precision may not be valid anymore (depending on the data)!" << std::endl;
}

realFloat defaultFunction(realFloat in)
{
	return in;
}

DataSampleBasic DataSampleBasic::applyFunction(realFloat (*function)(realFloat))
{
	std::valarray<realFloat> functionAppliedToArray = values.apply(function);
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
    if( size < 1)
        throw std::invalid_argument("sampleSlice parameter \"size\" must be at least 1!");
    if( (start + (stride*(size-1)) ) >= numberOfElements || (start + (stride*(size-1))) < 0)
        throw std::invalid_argument("incorrect sampleSlice parameters: (start + (stride*(size-1))) must be between 0 and number of datapoints!");
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

