#include<iostream>

#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	mean = calcFirstMoment();
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

double dataSample::calcFirstMoment()
{
	return values.sum() / numberOfElements;
}

