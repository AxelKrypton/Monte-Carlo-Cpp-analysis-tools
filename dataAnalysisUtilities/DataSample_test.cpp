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

// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include "DataSample.hpp"

#include "TestDataSample.hpp"
#include "dataSampleTestUtilities.hpp"

#include <boost/test/unit_test.hpp>

// todo: add build tests

BOOST_AUTO_TEST_SUITE(zerothMoment)

    BOOST_AUTO_TEST_CASE(ZerothMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(0, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(ZerothMoment2)
    {
        int numberOfElements = 23e2;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(0, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(ZerothMoment3)
    {
        int numberOfElements = 24e3;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(0, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(ZerothMoment4)
    {
        int numberOfElements = 24e4;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(ZerothMoment5)
    {
        int numberOfElements = 32e2;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthMomentPerDataPoint tester1(0, numberOfElements, zeros, referenceValue);
        TestDataSampleNthMomentPerDataPoint tester2(0, numberOfElements, ones, referenceValue);
        TestDataSampleNthMomentPerDataPoint tester3(0, numberOfElements, arrayPosition, referenceValue);
        TestDataSampleNthMomentPerDataPoint tester4(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(firstMoment)

    BOOST_AUTO_TEST_CASE(firstMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 0.;
        TestDataSampleNthMoment tester(1, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment2)
    {
        int numberOfElements = 23;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(1, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment3)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 11.5;
        TestDataSampleNthMoment tester(1, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment4)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 0.5;
        TestDataSampleNthMoment tester(1, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment5)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        realFloat referenceValue = 0.56130529942755358;
        TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment6)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafileWithTwoColumns.example";
        realFloat referenceValue = 0.56130529942755358;
        TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue, 2);
    }

    BOOST_AUTO_TEST_CASE(firstMoment7)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafileWithTwoColumns.example";
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue, 1);
    }

    BOOST_AUTO_TEST_CASE(firstMoment8)
    {
        int numberOfElements = 1e6;
        realFloat referenceValue = 499999.5;
        TestDataSampleNthMoment tester(1, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(firstMoment9, 1)
    BOOST_AUTO_TEST_CASE(firstMoment9)
    {
        int numberOfElements = roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid;
        realFloat referenceValue = 1250000.00000125;
        TestDataSampleNthMoment tester(1, numberOfElements, bigAndSmallEntries, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstMoment10)
    {
        int numberOfElements = 8e2;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthMomentPerDataPoint tester(1, numberOfElements, ones, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondMoment)

    BOOST_AUTO_TEST_CASE(secondMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 0.;
        TestDataSampleNthMoment tester(2, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondMoment2)
    {
        int numberOfElements = 23;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(2, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondMoment3)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 180.166666666667;
        TestDataSampleNthMoment tester(2, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondMoment4)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 0.340579710144927;
        TestDataSampleNthMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondMoment5)
    {
        int numberOfElements = 124;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthMomentPerDataPoint tester(2, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(thirdMoment)

    BOOST_AUTO_TEST_CASE(thirdMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 0.;
        TestDataSampleNthMoment tester(3, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(thirdMoment2)
    {
        int numberOfElements = 23;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(3, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(thirdMoment3)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 3174.;
        TestDataSampleNthMoment tester(3, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(thirdMoment4)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 0.260869565217391;
        TestDataSampleNthMoment tester(3, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(sthirdMoment5)
    {
        int numberOfElements = 1240;
        DataSampleBasic referenceValue(makeValarrayWithOnesAndMinusOnes(numberOfElements));
        TestDataSampleNthMomentPerDataPoint tester(3, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fourthMoment)

    BOOST_AUTO_TEST_CASE(fourthMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 0.;
        TestDataSampleNthMoment tester(4, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(fourthMoment2)
    {
        int numberOfElements = 23;
        realFloat referenceValue = 1.;
        TestDataSampleNthMoment tester(4, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(fourthMoment3)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 59635.1666666667;
        TestDataSampleNthMoment tester(4, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(fourthMoment4)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 0.213103750582176;
        TestDataSampleNthMoment tester(4, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(fourthMoment5)
    {
        int numberOfElements = 2124;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthMomentPerDataPoint tester(4, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(zerothCentralMoment)

    BOOST_AUTO_TEST_CASE(zerothCentralMoment1)
    {
        int numberOfElements = 1e3;
        realFloat referenceValue = 1.;
        TestDataSampleNthCentralMoment tester(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(zerothCentralMoment2)
    {
        int numberOfElements = 1e3;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthCentralMomentPerDataPoint tester(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(firstCentralMoment)

    BOOST_AUTO_TEST_CASE(firstCentralMoment1)
    {
        int numberOfElements = 1e3;
        realFloat referenceValue = 0.;
        TestDataSampleNthCentralMoment tester(1, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(firstCentralMoment2)
    {
        int numberOfElements = 1e3;
        DataSampleBasic referenceValue(numberOfElements);
        TestDataSampleNthCentralMomentPerDataPoint tester(1, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondCentralMoment)

    BOOST_AUTO_TEST_CASE(secondCentralMoment1)
    {
        int numberOfElements = 1;
        realFloat referenceValue = 0.;
        TestDataSampleNthCentralMoment tester(2, numberOfElements, zeros, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondCentralMoment2)
    {
        int numberOfElements = 23;
        realFloat referenceValue = 0.;
        TestDataSampleNthCentralMoment tester(2, numberOfElements, ones, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondCentralMoment3)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 47.91666666666667;
        TestDataSampleNthCentralMoment tester(2, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondCentralMoment4)
    {
        int numberOfElements = 24;
        realFloat referenceValue = 0.0905797101449274;
        TestDataSampleNthCentralMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    // todo: perhaps add tests where the values between 0 and 1 are repeated...
    BOOST_AUTO_TEST_CASE(secondCentralMoment5)
    {
        int numberOfElements = 1e5;
        realFloat referenceValue = 0.0833350000166668;
        TestDataSampleNthCentralMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(secondCentralMoment6)
    {
        int numberOfElements = 988;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthCentralMomentPerDataPoint tester(2, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(thirdCentralMoment)

    BOOST_AUTO_TEST_CASE(thirdCentralMoment1)
    {
        int numberOfElements = 432;
        realFloat referenceValue = 0.;
        TestDataSampleNthCentralMoment tester(3, numberOfElements, onesMinusOnes, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(thirdCentralMoment2)
    {
        int numberOfElements = 654;
        DataSampleBasic referenceValue(makeValarrayWithOnesAndMinusOnes(numberOfElements));
        TestDataSampleNthCentralMomentPerDataPoint tester(3, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fourthCentralMoment)

    BOOST_AUTO_TEST_CASE(fourthCentralMoment1)
    {
        int numberOfElements = 12;
        realFloat referenceValue = 253.2291666666667;
        TestDataSampleNthCentralMoment tester(4, numberOfElements, arrayPosition, referenceValue);
    }

    BOOST_AUTO_TEST_CASE(fourthCentralMoment2)
    {
        int numberOfElements = 65e4;
        DataSampleBasic referenceValue(makeValarrayWithOnes(numberOfElements));
        TestDataSampleNthCentralMomentPerDataPoint tester(4, numberOfElements, onesMinusOnes, referenceValue);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthMoment)

    BOOST_AUTO_TEST_CASE(getNthMomentValidArgument1)
    {
        DataSample dataSampleInstance;
        int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(highestValueAllowed));
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMomentPerDataPoint(highestValueAllowed));
    }

    BOOST_AUTO_TEST_CASE(getNthMomentValidArgument2)
    {
        DataSample dataSampleInstance;
        int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed));
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMomentPerDataPoint(lowestValueAllowed));
    }

    BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument1)
    {
        DataSample dataSampleInstance;
        int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(highestValueAllowed + 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthMomentPerDataPoint(highestValueAllowed + 1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument2)
    {
        DataSample dataSampleInstance;
        int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed - 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthMomentPerDataPoint(lowestValueAllowed - 1), std::invalid_argument);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthCentralMoment)

    BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument1)
    {
        DataSample dataSampleInstance;
        int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed));
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMomentPerDataPoint(highestValueAllowed));
    }

    BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument2)
    {
        DataSample dataSampleInstance;
        int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed));
        BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMomentPerDataPoint(lowestValueAllowed));
    }

    BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument1)
    {
        DataSample dataSampleInstance;
        int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed + 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMomentPerDataPoint(highestValueAllowed + 1), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument2)
    {
        DataSample dataSampleInstance;
        int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed - 1), std::invalid_argument);
        BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMomentPerDataPoint(lowestValueAllowed - 1), std::invalid_argument);
    }

BOOST_AUTO_TEST_SUITE_END()

// todo: add tests for 3. and 4. central moments!

BOOST_AUTO_TEST_SUITE(removeElements)

    BOOST_AUTO_TEST_CASE(removeElements1)
    {
        int numberOfElements = 100;
        int numberOfElementsToRemove = 10;
        DataSample tmp(numberOfElements);
        DataSample tmp2 = removeNElementsFromDataSample(tmp, numberOfElementsToRemove);

        BOOST_REQUIRE_EQUAL(tmp2.getNumberOfElements(), numberOfElements - numberOfElementsToRemove);
    }

BOOST_AUTO_TEST_SUITE_END()
