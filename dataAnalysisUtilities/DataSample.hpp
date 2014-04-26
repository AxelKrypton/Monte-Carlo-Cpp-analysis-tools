#ifndef DATASAMPLEANALYZER_HPP_
#define DATASAMPLEANALYZER_HPP_

#include "DataSampleBasic.hpp"

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

//todo: remove all binning functionality, this is at the moment interlinked with the jackknife estimator class

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
	DataSampleBasic createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSampleBasic createBinnedDataSampleWithBinsize(int binsize);
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();

protected:
	/**
	 * Binning.
	 * Following B.A. Berg,
	 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
	 * p.52.
	 */
	DataSampleBasic performBinning(int numberOfBins, int binsize);
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
	void checkDiscardedElements(int valueIn, std::string descriptionIn);
	void checkIfBinningParameterIsValid(int valueIn, std::string descriptionIn);

	std::vector<Moment> moments;
	std::vector<Moment> centralMoments;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
};

#endif /* DATASAMPLEANALYZER_HPP_ */
