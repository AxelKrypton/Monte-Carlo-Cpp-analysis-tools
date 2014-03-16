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
	double getFirstMoment();
	double getSecondMoment();
	double getThirdMoment();
	double getFourthMoment();
	double getNthMoment(int n);

private:
	std::valarray<double> values;
	double firstMoment;
	double secondMoment;
	double thirdMoment;
	double fourthMoment;
	int numberOfElements;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;

	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
};

#endif
