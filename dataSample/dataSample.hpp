/**
 * Definition of the dataSample class
 */

#ifndef _dataSample_H_
#define _dataSample_H_

#include<valarray>

class dataSample
{
public:
	dataSample(std::valarray<double> valuesIn);
	void printValuesToScreen();
	int getNumberOfElements();
	double getMean();
	double getVariance();

private:
	std::valarray<double> values;
	//TODO: why can I not init this here? (gives iso... error)
	double mean;
	int numberOfElements;

	double calcFirstMoment();
};

#endif
