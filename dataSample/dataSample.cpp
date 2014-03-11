#include<iostream>

#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	mean = calcFirstMoment();
	variance = calcVariance();
}

int dataSample::getNumberOfElements()
{
	return numberOfElements;
}

void dataSample::printValuesToScreen()
{
	for (int iteration = 0; iteration < numberOfElements; iteration ++)
	{
		std::cout << values[iteration] << "\t";
	}
	std::cout << std::endl;
}

double dataSample::getMean()
{
	return mean;
}

double dataSample::getVariance()
{
	return variance;
}

double dataSample::calcFirstMoment()
{
	return values.sum() / numberOfElements;
}

double dataSample::calcVariance()
{
	//TODO: this fct. seems to be wrong in Alessandros code
	return pow(( values-mean ), 2.).sum()/( numberOfElements );
	//TODO: check if this implementation is better
	/*
	  std::valarray<double> sampleSquared = std::pow(values, 2.0);
	  return sampleSquared.sum() / numberOfElements - mean * mean;
	*/
}


