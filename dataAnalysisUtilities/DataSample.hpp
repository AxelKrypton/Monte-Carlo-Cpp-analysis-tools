#ifndef DATASAMPLE_HPP_
#define DATASAMPLE_HPP_

#include "DataSampleBasic.hpp"

//TODO: Think whether Moment and MomentPerDataPoint should be inside the class DataSample
/*
 * Here Moment is a generic name for central and non central moment: <x> and <x-mu>, <x^2> and <(x-mu)^2>, etc.
 */
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

/*
 * MomentPerDataPoint is not an official name. It is referred to the content of the <...> in the Moment above.
 * For example: for the second MomentPerDataPoint the original data set will be considered and each data will
 * be shifted by the mean (if central) and squared, BUT the average will be not evaluated.
 */
class MomentPerDataPoint
{
public:
	MomentPerDataPoint()
	{
		calculated = false;
		value = DataSampleBasic();
	}

	void set(DataSampleBasic valueIn)
	{
		value = valueIn;
		calculated = true;
	};

	bool calculated;
	DataSampleBasic value;
};

class DataSample: public DataSampleBasic
{
public:
	DataSample(DataSampleBasic sampleIn):
		DataSampleBasic(sampleIn)
	{
		initMoments();
	}

	DataSample(int length = defaultSizeOfDataSample):
		DataSampleBasic(length)
	{
		initMoments();
	}

	DataSample(std::valarray<double> valuesIn):
		DataSampleBasic(valuesIn)
	{
		initMoments();
	}

	DataSample(std::string dataFilename, int column = 1, int offset = 0):
		DataSampleBasic(dataFilename, column, offset)
	{
		initMoments();
	}

	double getNthCentralMoment(int n);
	double getNthMoment(int n);
	DataSampleBasic getNthCentralMomentPerDataPoint(int n);
	DataSampleBasic getNthMomentPerDataPoint(int n);
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();

protected:
	int getNumberOfMoments();
	void initMoments();
	double calcNthMoment(int n);
	double calcNthCentralMoment(int n);
	DataSampleBasic calcNthMomentPerDataPoint(int n);
	DataSampleBasic calcNthCentralMomentPerDataPoint(int n);

	std::vector<Moment> moments;
	std::vector<Moment> centralMoments;
	std::vector<MomentPerDataPoint> momentsPerDataPoint;
	std::vector<MomentPerDataPoint> centralMomentsPerDataPoint;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
};

DataSample removeNElementsFromDataSample(DataSample sampleIn, int n);

#endif /* DATASAMPLEANALYZER_HPP_ */
