/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2015,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "DataSample.hpp"

#include <iostream>
#include <sstream>

static realFloat calcNthMomentExplicit(DataSample& sampleIn, int n);
static realFloat calcFirstMomentExplicit(DataSample& sampleIn);
static realFloat calcNthCentralMomentExplicit(DataSample& sampleIn, int n);
static DataSampleBasic calcNthMomentPerDataPointExplicit(DataSample& sampleIn, int n);
static DataSampleBasic calcFirstMomentPerDataPointExplicit(DataSample& sampleIn);
static DataSampleBasic calcNthCentralMomentPerDataPointExplicit(DataSample& sampleIn, int n);
static void checkIfNIsValid(int n, int upperLimit, int lowerLimit);

void DataSample::initMoments()
{
    int numberOfMoments = getNumberOfMoments();
    moments = std::vector<Moment>(numberOfMoments);
    centralMoments = std::vector<Moment>(numberOfMoments);
    momentsPerDataPoint = std::vector<MomentPerDataPoint>(numberOfMoments);
    centralMomentsPerDataPoint = std::vector<MomentPerDataPoint>(numberOfMoments);
}

realFloat DataSample::getNthMoment(int n)
{
    checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
    if (! moments[n].calculated) {
        moments[n].set(calcNthMoment(n));
    }
    return moments[n].value;
}

realFloat DataSample::calcNthMoment(int n)
{
    if (n == 0) {
        return 1.;
    } else if (n == 1) {
        return calcFirstMomentExplicit(*this);
    } else {
        return calcNthMomentExplicit(*this, n);
    }
}

realFloat DataSample::getNthCentralMoment(int n)
{
    checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
    if (! centralMoments[n].calculated) {
        centralMoments[n].set(calcNthCentralMoment(n));
    }
    return centralMoments[n].value;
}

realFloat DataSample::calcNthCentralMoment(int n)
{
    if (n == 0) {
        return 1.;
    } else if (n == 1) {
        return 0.;
    } else {
        return calcNthCentralMomentExplicit(*this, n);
    }
}

DataSampleBasic DataSample::getNthMomentPerDataPoint(int n)
{
    checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
    if (! momentsPerDataPoint[n].calculated) {
        momentsPerDataPoint[n].set(calcNthMomentPerDataPoint(n));
    }
    return momentsPerDataPoint[n].value;
}

DataSampleBasic DataSample::calcNthMomentPerDataPoint(int n)
{
    if (n == 0) {
        return DataSampleBasic(std::valarray<realFloat>(1.0, this->getNumberOfElements()));
    } else if (n == 1) {
        return calcFirstMomentPerDataPointExplicit(*this);
    } else {
        return calcNthMomentPerDataPointExplicit(*this, n);
    }
}

DataSampleBasic DataSample::getNthCentralMomentPerDataPoint(int n)
{
    checkIfNIsValid(n, upperLimitForNthMoment, lowerLimitForNthMoment);
    if (! centralMomentsPerDataPoint[n].calculated) {
        centralMomentsPerDataPoint[n].set(calcNthCentralMomentPerDataPoint(n));
    }
    return centralMomentsPerDataPoint[n].value;
}

DataSampleBasic DataSample::calcNthCentralMomentPerDataPoint(int n)
{
    if (n == 0) {
        return DataSampleBasic(std::valarray<realFloat>(1.0, this->getNumberOfElements()));
    } else if (n == 1) {
        return DataSampleBasic(std::valarray<realFloat>(0.0, this->getNumberOfElements()));
    } else {
        return calcNthCentralMomentPerDataPointExplicit(*this, n);
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

DataSample removeNElementsFromDataSample(DataSample sampleIn, int n)
{
    int numberOfElements = sampleIn.getNumberOfElements();
    DataSampleBasic tmp = sampleIn;
    for (int iteration = numberOfElements - 1; iteration >= numberOfElements - n; iteration--) {
        tmp = tmp.removeIthElement(iteration);
    }
    return DataSample(tmp);
}

/**************************** STATIC FUNCTIONS ****************************/

static void checkIfNIsValid(int n, int upperLimit, int lowerLimit)
{
    if (n < lowerLimit || n > upperLimit)
        throw std::invalid_argument("The requested moment is not implemented yet!");
}

static realFloat calcNthMomentExplicit(DataSample& sampleIn, int n)
{
    return (sampleIn ^ n).sum() / (realFloat)sampleIn.getNumberOfElements();
}

static realFloat calcFirstMomentExplicit(DataSample& sampleIn)
{
    return sampleIn.sum() / sampleIn.getNumberOfElements();
}

static realFloat calcNthCentralMomentExplicit(DataSample& sampleIn, int n)
{
    return ((sampleIn - sampleIn.getNthMoment(1)) ^ n).sum() / sampleIn.getNumberOfElements();
}

static DataSampleBasic calcNthMomentPerDataPointExplicit(DataSample& sampleIn, int n)
{
    return (sampleIn ^ n);
}

static DataSampleBasic calcFirstMomentPerDataPointExplicit(DataSample& sampleIn)
{
    return sampleIn;
}

static DataSampleBasic calcNthCentralMomentPerDataPointExplicit(DataSample& sampleIn, int n)
{
    return ((sampleIn - sampleIn.getNthMoment(1)) ^ n);
}
