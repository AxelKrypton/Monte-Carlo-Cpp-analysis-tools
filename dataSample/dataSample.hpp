/**
 * Definition of the dataSample class
 */

#ifndef _dataSample_H_
#define _dataSample_H_

#include<valarray>
#include<vector>
#include<stdexcept>

class dataSample
{
public:
	dataSample();
	dataSample(std::valarray<double> valuesIn);
	int getNumberOfElements();
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getMean();
	double getVariance();
	double getNthMoment(int n);
	dataSample createBinnedDataSample(int numberOfBins);

private:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void initMoments();
	int getNumberOfMoments();

	std::valarray<double> values;
	std::vector<double> moments;
	int numberOfElements;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
	const static int defaultSizeOfDataSample = 1;
};

#endif
