/**
 * Definition of the dataSample class
 */

#ifndef DATASAMPLE_HPP_
#define DATASAMPLE_HPP_

#include<valarray>
#include<vector>
#include<stdexcept>

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
	DataSample createShiftedDataSample(int order, double shift);
	double sum();
	double& operator[](size_t index);

protected:
	void setValues(std::valarray<double> valuesIn);
	void checkIfNumberOfElementsIsValid(int length);
	void checkIfDatafileExists(std::string filename);
	void checkIfColumnIsValid(int column);
	void checkIfOffsetIsValid(int offset);
	std::valarray<double> readDataFromFile(std::string filename, int column, int offset);

	std::valarray<double> values;
	int numberOfElements;
	const static int defaultSizeOfDataSample = 1;
};

#endif
