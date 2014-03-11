/**
 * Definition of the dataSample class
 */

//todo: put security to headers

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
