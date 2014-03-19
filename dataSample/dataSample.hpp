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
	//todo: think about creating more constructors because of branches!
	//todo: create jackknife child of DataSample
	DataSample(int length = defaultSizeOfDataSample, bool isJackknifeSample = false);
	DataSample(std::valarray<double> valuesIn, bool isJackknifeSample = false);
	DataSample(std::string dataFilename, int column = 1, int offset = 0, bool isJackknifeSample = false);
	int getNumberOfElements();
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getMean();
	double getNthCentralMoment();
	double getNthMoment(int n);
	DataSample createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSample createBinnedDataSampleWithBinsize(int binsize);
	/**
	 * Following Berg, equation (2.160).
	 */
	DataSample createJackknifeEstimators();
	DataSample applyFunction(double (*function)(double) = defaultFunction);
	double getJackknifeVariance();
	double getJackknifeVariance_v2();
	double getJackknifeError();

private:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void checkIfBinsizeIsValid(int binsize);
	void checkIfNumberOfElementsIsValid(int length);
	void checkIfSampleIsJackknifeSample();
	void checkIfDatafileExists(std::string filename);
	void checkIfColumnIsValid(int column);
	void checkIfOffsetIsValid(int offset);
	int calcBinsize(int numberOfBins);
	int calcNumberOfBins(int binsize);
	void initMembers();
	void initMoments();
	int getNumberOfMoments();
	int getJackknifeNormalization();
	/**
	 * Following Berg, p.52.
	 */
	DataSample performBinning(int numberOfBins, int binsize);
	std::valarray<double> readDataFromFile(std::string filename, int column, int offset);

	std::valarray<double> values;
	std::vector<double> moments;
	int numberOfElements;
	bool isJackknifeSample;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
	const static int defaultSizeOfDataSample = 1;
};

#endif
