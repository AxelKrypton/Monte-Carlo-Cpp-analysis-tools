#include <iostream>
#include <sstream>
#include "DataSample.hpp"

static double calcNthMomentExplicit(DataSample & sampleIn, int n);
static double calcFirstMomentExplicit(DataSample & sampleIn);
static double calcNthCentralMomentExplicit(DataSample & sampleIn, int n);
static void checkIfNIsValid(int n, int upperLimit, int lowerLimit);

void DataSample::initMoments()
{
	int numberOfMoments = getNumberOfMoments();
	moments = std::vector<Moment>(numberOfMoments);
	centralMoments = std::vector<Moment>(numberOfMoments);
}

double DataSample::getNthMoment(int n)
{
	checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
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
		return calcFirstMomentExplicit(*this);
	}
	else
	{
		return calcNthMomentExplicit(*this, n);
	}
}

double DataSample::getNthCentralMoment(int n)
{
	checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
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
		return calcNthCentralMomentExplicit(*this, n);
	}
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

static void checkIfNIsValid(int n, int upperLimit, int lowerLimit)
{
	if(n < lowerLimit || n > upperLimit)
		throw std::invalid_argument("The requested moment is not implemented yet!");
}

static double calcNthMomentExplicit(DataSample & sampleIn, int n)
{
	return (sampleIn^n).sum() / (double) sampleIn.getNumberOfElements();
}

static double calcFirstMomentExplicit(DataSample & sampleIn)
{
	return sampleIn.sum() / sampleIn.getNumberOfElements();
}

static double calcNthCentralMomentExplicit(DataSample & sampleIn, int n)
{
	return ( (sampleIn - sampleIn.getNthMoment(1) )^( (double(n)) )  ).sum()  / sampleIn.getNumberOfElements();
}
