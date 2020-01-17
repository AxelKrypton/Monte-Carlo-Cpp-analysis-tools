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

/*
 * TestDataSample.hpp
 * Wrapper class to test DataSample.
 */

#ifndef TESTDATASAMPLE_HPP_
#define TESTDATASAMPLE_HPP_

#include "dataSampleTestUtilities.hpp"

#include <boost/test/unit_test.hpp>
#include <type_traits>

static std::valarray<realFloat>* initDataSampleBasedOnFillType(int, FillType);

/*
 * Declaring the template but NOT defining it allows to provide specializations that are the only ones
 * possible to be called. If one tries to declare this template with a different type, it will result in
 * a compilation error.
 */
template<class T> class TestDataSample;

// Case 1: realFloat
template<> class TestDataSample<realFloat> {
  public:
    TestDataSample(int length, FillType fillType = zeros, realFloat* referenceValueIn = NULL)
        : testPrecision(realFloatPrecisionInPercent)
        , fillType(fillType)
    {
        if (referenceValueIn == NULL)
            isReferenceValueGiven = false;
        else {
            isReferenceValueGiven = true;
            referenceValue = *referenceValueIn;
        }
        std::valarray<realFloat>* testValues = initDataSampleBasedOnFillType(length, fillType);
        dataSampleInstance = new DataSample(*testValues);
        delete testValues;
        actualValue = 12345.6789;  // It doesn't matter which number I put here, this is initialized in child class
    }

    TestDataSample(std::string dataFilename, int column = 1, int offset = 0, realFloat* referenceValueIn = NULL)
        : testPrecision(realFloatPrecisionInPercent)
    {
        if (referenceValueIn == NULL)
            isReferenceValueGiven = false;
        else {
            isReferenceValueGiven = true;
            referenceValue = *referenceValueIn;
        }
        dataSampleInstance = new DataSample(dataFilename, column, offset);
        fillType = zeros;
        actualValue = 12345.6789;  // It doesn't matter which number I put here, this is initialized in child class
    }

    virtual ~TestDataSample()
    {
        testActualValueAgainstReferenceValue();
        delete dataSampleInstance;
    }

    int getNumberOfElements() { return dataSampleInstance->getNumberOfElements(); }

    DataSample* getDataSample() { return dataSampleInstance; }

  protected:
    void testActualValueAgainstReferenceValue()
    {
        if (isReferenceValueGiven == true)
            BOOST_CHECK_CLOSE(actualValue, referenceValue, testPrecision);
    }

    DataSample* dataSampleInstance;
    bool isReferenceValueGiven;
    realFloat referenceValue;
    realFloat actualValue;
    realFloat testPrecision;
    FillType fillType;
};

// Case 2: DataSampleBasic
template<> class TestDataSample<DataSampleBasic> {
  public:
    TestDataSample(int length, FillType fillType = zeros, DataSampleBasic* referenceValueIn = NULL)
        : testPrecision(realFloatPrecisionInPercent)
        , fillType(fillType)
    {
        if (referenceValueIn == NULL)
            isReferenceValueGiven = false;
        else {
            isReferenceValueGiven = true;
            referenceValue = *referenceValueIn;
        }
        std::valarray<realFloat>* testValues = initDataSampleBasedOnFillType(length, fillType);
        dataSampleInstance = new DataSample(*testValues);
        delete testValues;
        actualValue = DataSampleBasic();  // It doesn't matter which number I put here, this is initialized in child class
    }

    TestDataSample(std::string dataFilename, int column = 1, int offset = 0, DataSampleBasic* referenceValueIn = NULL)
        : testPrecision(realFloatPrecisionInPercent)
    {
        if (referenceValueIn == NULL)
            isReferenceValueGiven = false;
        else {
            isReferenceValueGiven = true;
            referenceValue = *referenceValueIn;
        }
        dataSampleInstance = new DataSample(dataFilename, column, offset);
        fillType = zeros;
        actualValue = DataSampleBasic();  // It doesn't matter which number I put here, this is initialized in child class
    }

    virtual ~TestDataSample()
    {
        testActualValueAgainstReferenceValue();
        delete dataSampleInstance;
    }

    int getNumberOfElements() { return dataSampleInstance->getNumberOfElements(); }

    DataSample* getDataSample() { return dataSampleInstance; }

  protected:
    void testActualValueAgainstReferenceValue()
    {
        for (int i = 0; i < actualValue.getNumberOfElements(); i++)
            BOOST_CHECK_CLOSE(actualValue[i], referenceValue[i], testPrecision);
    }

    DataSample* dataSampleInstance;
    bool isReferenceValueGiven;
    DataSampleBasic referenceValue;
    DataSampleBasic actualValue;
    realFloat testPrecision;
    FillType fillType;
};

/***************************************************************************/

// Children classes

class TestDataSampleNthMoment : public TestDataSample<realFloat> {
  public:
    TestDataSampleNthMoment(int n, int length, FillType fillType, realFloat referenceValue)
        : TestDataSample<realFloat>(length, fillType, &referenceValue)
    {
        actualValue = dataSampleInstance->getNthMoment(n);
    };
    TestDataSampleNthMoment(int n, std::string dataFilename, realFloat referenceValue = 0, int column = 1, int offset = 0.)
        : TestDataSample<realFloat>(dataFilename, column, offset, &referenceValue)
    {
        actualValue = dataSampleInstance->getNthMoment(n);
    };
};

class TestDataSampleNthCentralMoment : public TestDataSample<realFloat> {
  public:
    TestDataSampleNthCentralMoment(int n, int length, FillType fillType, realFloat referenceValue)
        : TestDataSample<realFloat>(length, fillType, &referenceValue)
    {
        actualValue = dataSampleInstance->getNthCentralMoment(n);
    };
};

class TestDataSampleNthMomentPerDataPoint : public TestDataSample<DataSampleBasic> {
  public:
    TestDataSampleNthMomentPerDataPoint(int n, int length, FillType fillType, DataSampleBasic referenceValue)
        : TestDataSample<DataSampleBasic>(length, fillType, &referenceValue)
    {
        actualValue = dataSampleInstance->getNthMomentPerDataPoint(n);
    };
};

class TestDataSampleNthCentralMomentPerDataPoint : public TestDataSample<DataSampleBasic> {
  public:
    TestDataSampleNthCentralMomentPerDataPoint(int n, int length, FillType fillType, DataSampleBasic referenceValue)
        : TestDataSample<DataSampleBasic>(length, fillType, &referenceValue)
    {
        actualValue = dataSampleInstance->getNthCentralMomentPerDataPoint(n);
    };
};

/**************************************************************************************************/

static std::valarray<realFloat>* initDataSampleBasedOnFillType(int length, FillType fillType)
{
    if (fillType == zeros) {
        return new std::valarray<realFloat>(length);
    } else if (fillType == ones) {
        std::valarray<realFloat> tmp = makeValarrayWithOnes(length);
        return new std::valarray<realFloat>(tmp);
    } else if (fillType == onesMinusOnes) {
        std::valarray<realFloat> tmp = makeValarrayWithOnesAndMinusOnes(length);
        return new std::valarray<realFloat>(tmp);
    } else if (fillType == arrayPosition) {
        std::valarray<realFloat> tmp = makeValarrayWithArrayPosition(length);
        return new std::valarray<realFloat>(tmp);
    } else if (fillType == entriesSymmetricBetweenZeroAndOne) {
        std::valarray<realFloat> tmp = makeValarrayWithEntriesBetweenZeroAndOne(length);
        return new std::valarray<realFloat>(tmp);
    } else if (fillType == bigAndSmallEntries) {
        std::valarray<realFloat> tmp = makeValarrayWithBigAndSmallEntries(length / 2);
        return new std::valarray<realFloat>(tmp);
    } else {
        throw std::invalid_argument("Unknown fillType selected!");
    }
}

#endif /* TESTDATASAMPLE_HPP_ */
