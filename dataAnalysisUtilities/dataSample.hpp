/**
 * Definition of the dataSample class
 */

#ifndef DATASAMPLE_HPP_
#define DATASAMPLE_HPP_

#include<valarray>
#include<vector>
#include<stdexcept>
#include<iostream>

const static int roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid = 1e4;

//TODO: move this to better place
double defaultFunction(double in);

class DataSample
{
public:
	DataSample(int length = defaultSizeOfDataSample);
	DataSample(std::valarray<double> valuesIn);
	//todo: think about creating two or three distinct constructors
	DataSample(std::string dataFilename, int column = 1, int offset = 0);
	int getNumberOfElements();
	DataSample applyFunction(double (*function)(double) = defaultFunction);
	DataSample shiftAndPow(int n, double shift);
	DataSample shift(double shift);
	//todo: make the arg. constant?
	DataSample& operator*=(double factor);
	DataSample& operator*=(DataSample sampleIn);
	DataSample operator/(double factor);
	DataSample operator/(DataSample sampleIn);
	DataSample pow(int n);
	DataSample sampleSlice(int start, int size, int stride);
	double sum();
	double& operator[](size_t index);

protected:
	void setValues(std::valarray<double> valuesIn);
	void setValues(DataSample sampleIn);
	void checkIfNumberOfElementsIsValid(int length);
	void checkSliceParameters(int start, int size, int stride);
	DataSample readDataFromFile(std::string filename, int column, int offset);

	std::valarray<double> values;
	int numberOfElements;
	const static int defaultSizeOfDataSample = 1;
};

DataSample operator*(DataSample sampleIn, double factor);
DataSample operator*(DataSample lhs, DataSample rhs);

#endif
