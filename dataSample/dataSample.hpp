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

class Moment
{
public:
	Moment()
	{
		calculated = false;
		value = 0.;
	}

	void set(double valueIn)
	{
		value = valueIn;
		calculated = true;
	};

	bool calculated;
	double value;
};

//todo: think about better name!
class DataSampleAnalyzer: public DataSample
{
public:
	DataSampleAnalyzer(DataSample sampleIn):
		DataSample(sampleIn)
	{
		initMoments();
	}

	DataSampleAnalyzer(int length = defaultSizeOfDataSample):
		DataSample(length)
	{
		initMoments();
	}

	DataSampleAnalyzer(std::valarray<double> valuesIn):
		DataSample(valuesIn)
	{
		initMoments();
	}

	DataSampleAnalyzer(std::string dataFilename, int column = 1, int offset = 0):
		DataSample(dataFilename, column, offset)
	{
		initMoments();
	}

	double getNthCentralMoment(int n);
	double getNthMoment(int n);
	DataSample createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSample createBinnedDataSampleWithBinsize(int binsize);
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();

protected:
	/**
	 * Following Berg, p.52.
	 */
	DataSample performBinning(int numberOfBins, int binsize);
	int getNumberOfMoments();
	int calcBinsize(int numberOfBins);
	int calcNumberOfBins(int binsize);
	void initMoments();
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcNthCentralMoment(int n);
	double calcNthCentralMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void checkIfBinsizeIsValid(int binsize);

	std::vector<Moment> moments;
	std::vector<Moment> centralMoments;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
};

#endif
