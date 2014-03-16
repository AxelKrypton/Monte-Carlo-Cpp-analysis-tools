#include<iostream>
#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	firstMoment = calcNthMoment(1);
	secondMoment = calcNthMoment(2);
	thirdMoment = calcNthMoment(3);
	fourthMoment = calcNthMoment(4);
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

void dataSample::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}

double dataSample::calcNthMoment(int n)
{
	//todo: return member variables if already calculated...
	checkIfNIsValid(n);
	if ( n == 1)
	{
		return calcFirstMomentExplicit();
	}
	else
	{
		return calcNthMomentExplicit(n);
	}
}

double dataSample::calcNthMomentExplicit(int n)
{
	std::valarray<double> sampleToNthPower = std::pow(values, double(n));
	return sampleToNthPower.sum() / numberOfElements;
}

double dataSample::calcFirstMomentExplicit()
{
	return values.sum() / numberOfElements;
}

double dataSample::getMean()
{
	return firstMoment;
}

double dataSample::getVariance()
{
	return ( secondMoment - pow(firstMoment,2.) );
}

double dataSample::getNthMoment(int n)
{
	return calcNthMoment(n);
}

int dataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int dataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}
