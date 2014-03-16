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

	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcFirstMomentExplicit();
};

#endif
