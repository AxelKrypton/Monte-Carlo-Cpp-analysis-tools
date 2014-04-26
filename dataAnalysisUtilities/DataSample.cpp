#include <iostream>
#include <sstream>
#include "DataSample.hpp"

void DataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<Moment>(numberOfMoments);
	centralMoments = std::vector<Moment>(numberOfMoments);
}

double DataSample::getNthMoment(int n)
{
	checkIfNIsValid(n);
	if (!moments[n].calculated)
	{
		moments[n].set(calcNthMoment(n));
	}
	return moments[n].value;
}

double DataSample::calcNthMoment(int n)
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

double DataSample::calcNthMomentExplicit(int n)
{
	return (*this^n).sum() / (double) numberOfElements;
}

double DataSample::calcFirstMomentExplicit()
{
	return sum() / numberOfElements;
}

double DataSample::getNthCentralMoment(int n)
{
	checkIfNIsValid(n);
	if (!centralMoments[n].calculated)
	{
		centralMoments[n].set(calcNthCentralMoment(n));
	}
	return centralMoments[n].value;
}

double DataSample::calcNthCentralMoment(int n)
{
	if ( n == 0 )
	{
		return 1.;
	}
	else if (n == 1)
	{
		return 0.;
	}
	else
	{
		return calcNthCentralMomentExplicit(n);
	}
}

double DataSample::calcNthCentralMomentExplicit(int n)
{
	return ( (*this - getNthMoment(1) )^( (double(n)) )  ).sum()  / numberOfElements;
}

int DataSample::getNumberOfMoments()
{
	return upperLimitForNthMoment - lowerLimitForNthMoment + 1;
}

int DataSample::getUpperLimitForNthMoment()
{
	return upperLimitForNthMoment;
}

int DataSample::getLowerLimitForNthMoment()
{
	return lowerLimitForNthMoment;
}

void DataSample::checkIfNIsValid(int n)
{
	if(n < lowerLimitForNthMoment || n > upperLimitForNthMoment)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}

