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

//TODO: implement offset
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
	double sum();
	double& operator[](size_t index);
	DataSample pow(int n);
	DataSample sampleSlice(int start, int size, int stride);

protected:
	void setValues(std::valarray<double> valuesIn);
	void setValues(DataSample sampleIn);
	void checkIfNumberOfElementsIsValid(int length);
	void checkIfDatafileExists(std::string filename);
	void checkIfColumnIsValid(int column);
	void checkIfOffsetIsValid(int offset);
	void checkSliceParameters(int start, int size, int stride);
	std::valarray<double> readDataFromFile(std::string filename, int column, int offset);

	std::valarray<double> values;
	int numberOfElements;
	const static int defaultSizeOfDataSample = 1;
};

#endif
