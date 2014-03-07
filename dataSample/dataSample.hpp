/**
 * Definition of the dataSample class
 */

#include<valarray>

class dataSample
{
public:
	dataSample(std::valarray<double> valuesIn);
	void printValuesToScreen();

private:
	std::valarray<double> values;
	double mean;

	double calcFirstMoment();
};
