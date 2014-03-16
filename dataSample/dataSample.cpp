#include<iostream>

#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	firstMoment = calcFirstMoment();
	secondMoment = calcSecondMoment();
	thirdMoment = calcThirdMoment();
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
	return firstMoment;
}

double dataSample::getVariance()
{
	return ( secondMoment - pow(firstMoment,2.) );
}

double dataSample::calcFirstMoment()
{
	return values.sum() / numberOfElements;
}

double dataSample::calcSecondMoment()
{
	std::valarray<double> sampleSquared = std::pow(values, 2.0);
	return sampleSquared.sum() / numberOfElements;
}

double dataSample::calcThirdMoment()
{
	std::valarray<double> sampleSquared = std::pow(values, 3.0);
	return sampleSquared.sum() / numberOfElements;
}

double dataSample::getSecondMoment()
{
	return secondMoment;
}

double dataSample::getThirdMoment()
{
	return thirdMoment;
}

