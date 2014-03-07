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
	//TODO: why can I not init this here? (gives iso... error)
	double mean;

	double calcFirstMoment();
};
