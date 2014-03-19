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
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getNthCentralMoment(int n);
	double getNthMoment(int n);
	DataSample createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSample createBinnedDataSampleWithBinsize(int binsize);
	/**
	 * Following Berg, equation (2.160).
	 */
	//todo: make this return an JackknifeDataSample!
	DataSample createJackknifeEstimators();
	DataSample applyFunction(double (*function)(double) = defaultFunction);

protected:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcNthCentralMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void checkIfBinsizeIsValid(int binsize);
	void checkIfNumberOfElementsIsValid(int length);
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
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
	const static int defaultSizeOfDataSample = 1;
};

class JackknifeDataSample: public DataSample
{
public:
	JackknifeDataSample(DataSample sampleIn) :
		DataSample(sampleIn)
	{
	}
	double getJackknifeVariance();
	double getJackknifeVariance_v2();
	double getJackknifeError();
};

#endif
