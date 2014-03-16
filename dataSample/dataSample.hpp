/**
 * Definition of the dataSample class
 */

#ifndef _dataSample_H_
#define _dataSample_H_

#include<valarray>
#include<stdexcept>

class dataSample
{
public:
	dataSample(std::valarray<double> valuesIn);
	void printValuesToScreen();
	int getNumberOfElements();
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getMean();
	double getVariance();
	double getNthMoment(int n);

private:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);

	std::valarray<double> values;
	double firstMoment;
	double secondMoment;
	double thirdMoment;
	double fourthMoment;
	int numberOfElements;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
};

#endif
