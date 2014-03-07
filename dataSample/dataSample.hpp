/**
 * Definition of the dataSample class
 */

#include<valarray>

class dataSample
{
public:
	dataSample(std::valarray<double> valuesIn);
	void printValuesToScreen();
	double getMean();

private:
	std::valarray<double> values;
	double mean = 0.;

	double calcFirstMoment();
};
