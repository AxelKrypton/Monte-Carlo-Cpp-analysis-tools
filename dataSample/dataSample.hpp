/**
 * Definition of the dataSample class
 */

#ifndef _dataSample_H_
#define _dataSample_H_

#include<valarray>
#include<vector>
#include<stdexcept>

class DataSample
{
public:
	DataSample(bool isJackknifeSample = false);
	DataSample(std::valarray<double> valuesIn, bool isJackknifeSample = false);
	int getNumberOfElements();
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getMean();
	double getVariance();
	double getNthMoment(int n);
	DataSample createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSample createBinnedDataSampleWithBinsize(int binsize);
	/**
	 * Following Berg, equation (2.160).
	 */
	DataSample createJackknifeEstimators();
	// todo: check again this construction
	DataSample applyFunction(double (*function)(double));
	double getJackknifeVariance();

private:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void checkIfBinsizeIsValid(int binsize);
	void checkIfNumberOfElementsIsValid();
	void checkIfSampleIsJackknifeSample();
	int calcBinsize(int numberOfBins);
	int calcNumberOfBins(int binsize);
	void initMoments();
	int getNumberOfMoments();
	/**
	 * Following Berg, p.52.
	 */
	DataSample performBinning(int numberOfBins, int binsize);

	std::valarray<double> values;
	std::vector<double> moments;
	int numberOfElements;
	bool isJackknifeSample;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
	const static int defaultSizeOfDataSample = 1;
};

#endif
