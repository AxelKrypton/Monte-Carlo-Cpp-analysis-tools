#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "dataSample.hpp"
#include "FileReader.hpp"

DataSample::DataSample(int length)
{
	setValues(std::valarray<double>(length));
}

DataSample::DataSample(std::valarray<double> valuesIn)
{
	setValues(valuesIn);
}

DataSample::DataSample(std::string dataFilename, int column, int offset)
{
	setValues(readDataFromFile(dataFilename, column, offset));
}

void DataSample::setValues(std::valarray<double> valuesIn)
{
	checkIfNumberOfElementsIsValid(valuesIn.size());
	values = valuesIn;
	numberOfElements = valuesIn.size();
}

void DataSample::setValues(DataSample sampleIn)
{
	numberOfElements = sampleIn.getNumberOfElements();
	values = sampleIn.values;
}

DataSample 	DataSample::pow(int n)
{
	return DataSample( std::pow(values, double(n)) );
}

DataSample DataSample::shiftAndPow(int n, double shift)
{
	return DataSample( std::pow((values - shift), double(n)) );
}

DataSample DataSample::shift(double shift)
{
	return DataSample( (values - shift) );
}

double DataSample::sum()
{
	return values.sum();
}

double& DataSample::operator[](size_t index)
{
	return values[index];
}

DataSample DataSample::operator*(double factor)
{
	return DataSample(values * factor);
}


int DataSample::getNumberOfElements()
{
	return numberOfElements;
}

void DataSample::checkIfNumberOfElementsIsValid(int length)
{
	if(length <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
	//todo: think about better warning!
	if(length > roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid)
		std::cout << "Warning: the datasize is such that double precision may not be valid anymore (depending on the data)!" << std::endl;
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

DataSample DataSample::readDataFromFile(std::string filename, int column, int offset)
{
	FileReader reader(filename, column, offset);
	return reader.readDataFromFile();
}

void DataSample::checkSliceParameters(int start, int size, int stride)
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

DataSample DataSample::sampleSlice(int start, int size, int stride)
{
	checkSliceParameters(start, size, stride);
	return DataSample(values[std::slice(start, size, stride)]);
}
