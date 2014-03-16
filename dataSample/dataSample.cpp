#include<iostream>
#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
	numberOfElements = values.size();
	initMoments();
}

void dataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<double>(numberOfMoments,0);
	//todo: calc moments on demand...
	for (int iteration = 0; iteration < moments.size(); iteration++)
	{
		moments[iteration] = calcNthMoment(iteration);
	}
}

int dataSample::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
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
	return getNthMoment(1);
}

double dataSample::getVariance()
{
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) );
}

double dataSample::getNthMoment(int n)
{
	checkIfNIsValid(n);
	return moments[n];
}

int dataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int dataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}
