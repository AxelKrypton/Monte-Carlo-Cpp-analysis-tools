/**
 * Definition of the dataSample class
 */

#ifndef DATASAMPLEBASIC_HPP_
#define DATASAMPLEBASIC_HPP_

#include<valarray>
#include<vector>
#include<stdexcept>
#include<iostream>

const static int roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid = 1e4;

//TODO: move this to better place
double defaultFunction(double in);

class DataSampleBasic
{
public:
	DataSampleBasic(int length = defaultSizeOfDataSample);
	DataSampleBasic(std::valarray<double> valuesIn);
	//todo: think about creating two or three distinct constructors
	DataSampleBasic(std::string dataFilename, int column = 1, int offset = 0, bool* isAnyEntryBad = NULL);
	//todo: make the args and fcts. constant where possible
	int getNumberOfElements();
	DataSampleBasic applyFunction(double (*function)(double) = defaultFunction);
	DataSampleBasic& operator+=(double factor);
	DataSampleBasic& operator+=(DataSampleBasic sampleIn);
	DataSampleBasic& operator-=(double factor);
	DataSampleBasic& operator-=(DataSampleBasic sampleIn);
	DataSampleBasic& operator*=(double factor);
	DataSampleBasic& operator*=(DataSampleBasic sampleIn);
	DataSampleBasic& operator/=(double factor);
	DataSampleBasic& operator/=(DataSampleBasic sampleIn);
	DataSampleBasic& operator^=(int n);
	DataSampleBasic& operator^=(double n);
	DataSampleBasic sampleSlice(int start, int size, int stride);
	double sum();
    double min();
    double max();
	double& operator[](size_t index);
	DataSampleBasic removeIthElement(int i);

protected:
	void setValues(std::valarray<double> valuesIn);
	void setValues(DataSampleBasic sampleIn);
	void checkIfNumberOfElementsIsValid(int length);
	void checkSliceParameters(int start, int size, int stride);
	DataSampleBasic readDataFromFile(std::string filename, int column, int offset, bool* isAnyEntryBad = NULL);

	std::valarray<double> values;
	int numberOfElements;
	const static int defaultSizeOfDataSample = 1;
};

DataSampleBasic operator+(DataSampleBasic sampleIn, double factor);
DataSampleBasic operator+(double factor, DataSampleBasic sampleIn);
DataSampleBasic operator+(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator-(DataSampleBasic sampleIn, double factor);
DataSampleBasic operator-(double factor, DataSampleBasic sampleIn);
DataSampleBasic operator-(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator*(DataSampleBasic sampleIn, double factor);
DataSampleBasic operator*(double factor, DataSampleBasic sampleIn);
DataSampleBasic operator*(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator/(DataSampleBasic sampleIn, double factor);
DataSampleBasic operator/(double factor, DataSampleBasic sampleIn);
DataSampleBasic operator/(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator^(DataSampleBasic sampleIn, int n);
DataSampleBasic operator^(DataSampleBasic sampleIn, double n);

#endif /* DATASAMPLEBASIC_HPP_ */
