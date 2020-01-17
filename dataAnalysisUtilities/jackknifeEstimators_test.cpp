/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2015,2020 Alessandro Sciarra
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
#include "jackknifeEstimators.hpp"

#include "TestDataSample.hpp"
#include "binnedDataSample.hpp"
#include "jackknifeAnalysis.hpp"

#include <boost/test/unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_SUITE(jackknifeEstimators)

    BOOST_AUTO_TEST_CASE(build)
    {
        int enoughElementsForJackknife = 57;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_CHECK_NO_THROW(JackknifeEstimators jackSample(sample));
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        int enoughElementsForJackknife = 573;
        BOOST_CHECK_NO_THROW(JackknifeEstimators jackSample(enoughElementsForJackknife));
    }

    BOOST_AUTO_TEST_CASE(buildInvalidArgument)
    {
        int tooFewElementsForJackknife = 1;
        DataSampleBasic sample(tooFewElementsForJackknife);
        BOOST_REQUIRE_THROW(JackknifeEstimators jackSample(sample), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(elements)
    {
        int numberOfElements = 37;
        DataSampleBasic sample(numberOfElements);
        JackknifeEstimators jackknifeSample(sample);
        BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
    }

BOOST_AUTO_TEST_SUITE_END()

// todo: go through and delete/modify tests

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinnedDataSample)

    BOOST_AUTO_TEST_CASE(build_invalidArgument)
    {
        DataSampleBasic sample;
        BOOST_REQUIRE_THROW(JackknifeEstimators jackknifeSample(sample), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(elements)
    {
        int numberOfElements = 37;
        DataSampleBasic sample(numberOfElements);
        JackknifeEstimators jackknifeSample(sample);
        BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
    }

    BOOST_AUTO_TEST_CASE(firstMoment1)
    {
        int numberOfElements = 89;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(firstMoment2)
    {
        int numberOfElements = 1e3;
        TestDataSample<realFloat> testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), realFloatPrecisionInPercent);
    }

    realFloat calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
    {
        realFloat prefactor = pow(realFloat(numberOfElements), 2.) - 2. * numberOfElements;
        realFloat normalization = pow(realFloat(numberOfElements - 1), 2.);
        return (sample.getNthMoment(2) + pow(sample.getNthMoment(1), 2.) * prefactor) / normalization;
    }

    BOOST_AUTO_TEST_CASE(secondMoment1)
    {
        int numberOfElements = 45;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        realFloat expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(*sample, numberOfElements);
        BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(secondMoment2)
    {
        int numberOfElements = 5e3;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        realFloat expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(*sample, numberOfElements);
        BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), realFloatPrecisionInPercent);
    }

    realFloat expectedValueForJackknifeVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
    {
        realFloat secondMoment = sample.getNthMoment(2);
        realFloat firstMoment = sample.getNthMoment(1);
        realFloat prefactor = 1. / (numberOfElements - 1.);
        return prefactor * (secondMoment - pow(firstMoment, 2.));
    }

    BOOST_AUTO_TEST_CASE(variance)
    {
        int numberOfElements = 43;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        realFloat jackknifeVariance = pow(calculateJacknifeError(jackknifeSample), 2.);
        realFloat expectedValue = expectedValueForJackknifeVarianceBasedOnAnalyticExpression(*sample, numberOfElements);
        BOOST_CHECK_CLOSE(jackknifeVariance, expectedValue, realFloatPrecisionInPercent);
    }

    void checkVarianceErrorWithNumberOfBins(std::string file, int numberOfBins, realFloat expectedValue, realFloat testPrecision)
    {
        DataSample sample(file);
        DataSample varSample = (sample - sample.getNthMoment(1)) ^ 2;
        BinnedDataSampleFromNumberOfBins binnedSample(varSample, numberOfBins);
        JackknifeEstimators jackSample(binnedSample);
        BOOST_CHECK_CLOSE(calculateJacknifeError(jackSample), expectedValue, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(withNumberOfBins_varianceError1)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int numberOfBins = 1005;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 4.98147373492720661E-006;

        checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(withNumberOfBins_varianceError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int numberOfBins = 10;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 1.0424604327986017e-05;

        checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinningWithNumberOfBins)

    BOOST_AUTO_TEST_CASE(build_invalidArg1)
    {
        int enoughElementsForJackknife = 57;
        int numberOfBinsTooLowForJackknife = 1;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_REQUIRE_THROW(
            JackknifeEstimatorsFromBinningWithNumberOfBins jackSample(sample, numberOfBinsTooLowForJackknife), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build_invalidArg2)
    {
        int enoughElementsForJackknife = 57;
        int numberOfBinsTooHighForJackknife = enoughElementsForJackknife + 1;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_REQUIRE_THROW(
            JackknifeEstimatorsFromBinningWithNumberOfBins jackSample(sample, numberOfBinsTooHighForJackknife), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(firstMoment1)
    {
        int numberOfElements = 1e3;
        int numberOfBins = 1e3;
        TestDataSample<realFloat> testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample(*sample, numberOfBins);
        BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(jackknifeVariance)
    {
        int numberOfElements = 43;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSampleBasic* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample2(*sample, numberOfElements);
        realFloat jackknifeVariance1 = pow(calculateJacknifeError(jackknifeSample), 2.);
        realFloat jackknifeVariance2 = pow(calculateJacknifeError(jackknifeSample2), 2.);
        BOOST_CHECK_CLOSE(jackknifeVariance1, jackknifeVariance2, realFloatPrecisionInPercent);
    }

    void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, realFloat expectedValue, realFloat testPrecision)
    {
        DataSampleBasic sample(file);
        JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample(sample, numberOfBins);
        BOOST_CHECK_CLOSE(calculateJacknifeError(jackknifeSample), expectedValue, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(meanError1)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int numberOfBins = 1005;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 3.44121381077520906E-004;

        checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(meanError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int numberOfBins = 10;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 1.1564370727055974e-03;
        checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(meanError3)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int numberOfBins = 10;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 1.14688734781786292E-003;

        checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
    }

    void checkVarianceErrorWithNumberOfBins(std::string file, int numberOfBins, realFloat expectedValue, realFloat testPrecision)
    {
        DataSample sample(file);
        DataSample varSample = (sample - sample.getNthMoment(1)) ^ 2;
        JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample(varSample, numberOfBins);
        BOOST_CHECK_CLOSE(calculateJacknifeError(jackknifeSample), expectedValue, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(varianceError1)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int numberOfBins = 10;
        realFloat expectedValue = 1.0424604327986017e-05;

        checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(varianceError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int numberOfBins = 10;
        realFloat expectedValue = 1.19735074528675533E-005;

        checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinningWithBinsize)

    BOOST_AUTO_TEST_CASE(build_invalidArg1)
    {
        int enoughElementsForJackknife = 57;
        int binsizeTooLowForJackknife = 0;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeTooLowForJackknife), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build_invalidArg2)
    {
        int enoughElementsForJackknife = 57;
        int binsizeTooHighForJackknife = enoughElementsForJackknife;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeTooHighForJackknife), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build)
    {
        int enoughElementsForJackknife = 57;
        int binsizeThatAlwaysWorks = 1;
        DataSampleBasic sample(enoughElementsForJackknife);
        BOOST_CHECK_NO_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeThatAlwaysWorks));
    }

    BOOST_AUTO_TEST_CASE(firstMoment1)
    {
        int numberOfElements = 1e3;
        int binsize = 1;
        TestDataSample<realFloat> testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
        DataSample* sample = testSample.getDataSample();
        JackknifeEstimatorsFromBinningWithBinsize jackknifeSample(*sample, binsize);
        BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(jackknifeVariance)
    {
        int numberOfElements = 43;
        int binsize = 1;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSampleBasic* sample = testSample.getDataSample();
        JackknifeEstimators jackknifeSample(*sample);
        JackknifeEstimatorsFromBinningWithBinsize jackknifeSample2(*sample, binsize);
        realFloat jackknifeVariance1 = pow(calculateJacknifeError(jackknifeSample), 2.);
        realFloat jackknifeVariance2 = pow(calculateJacknifeError(jackknifeSample2), 2.);
        BOOST_CHECK_CLOSE(jackknifeVariance1, jackknifeVariance2, realFloatPrecisionInPercent);
    }

    void checkMeanErrorWithBinsize(std::string file, int numberOfBins, realFloat expectedValue, realFloat testPrecision)
    {
        DataSampleBasic sample(file);
        JackknifeEstimatorsFromBinningWithBinsize jackknifeSample(sample, numberOfBins);
        BOOST_CHECK_CLOSE(calculateJacknifeError(jackknifeSample), expectedValue, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(meanError1)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int binsize = 1;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 3.44121381077520906E-004;

        checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(meanError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int binsize = 100;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 1.1564370727055974e-03;
        checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(meanError3)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int binsize = 100;
        realFloat precisionOfDataInFileInPercent = 1e-10;
        realFloat expectedValue = 1.14688734781786292E-003;

        checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
    }

    void checkVarianceErrorWithBinsize(std::string file, int numberOfBins, realFloat expectedValue, realFloat testPrecision)
    {
        DataSample sample(file);
        DataSample varSample = (sample - sample.getNthMoment(1)) ^ 2;
        JackknifeEstimatorsFromBinningWithBinsize jackknifeSample(varSample, numberOfBins);
        BOOST_CHECK_CLOSE(calculateJacknifeError(jackknifeSample), expectedValue, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(varianceError1)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        int binsize = 100;
        realFloat expectedValue = 1.0424604327986017e-05;

        checkVarianceErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(varianceError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int binsize = 100;
        realFloat expectedValue = 1.19735074528675533E-005;

        checkVarianceErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsExplicitAgainstCombinedMethod)

    BOOST_AUTO_TEST_CASE(jackknifeEstimatorsFromBinningWithNumberOfBins_varianceError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int numberOfBins = 50;

        DataSample sample(fileThatDoesExist);
        DataSample varSample = (sample - sample.getNthMoment(1)) ^ 2;
        JackknifeEstimatorsFromBinningWithNumberOfBins jackSample1(varSample, numberOfBins);
        BinnedDataSampleFromNumberOfBins binnedSample(varSample, numberOfBins);
        JackknifeEstimators jackSample2(binnedSample);

        realFloat varianceError1 = calculateJacknifeError(jackSample1);
        realFloat varianceError2 = calculateJacknifeError(jackSample2);

        BOOST_CHECK_CLOSE(varianceError1, varianceError2, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(jackknifeEstimatorsFromBinningWithBinsize_varianceError2)
    {
        std::string fileThatDoesExist = "SampleDatafiles/datafile2.example";
        int binsize = 20;

        DataSample sample(fileThatDoesExist);
        DataSample varSample = (sample - sample.getNthMoment(1)) ^ 2;
        JackknifeEstimatorsFromBinningWithBinsize jackSample1(varSample, binsize);
        BinnedDataSampleFromBinsize binnedSample(varSample, binsize);
        JackknifeEstimators jackSample2(binnedSample);

        realFloat varianceError1 = calculateJacknifeError(jackSample1);
        realFloat varianceError2 = calculateJacknifeError(jackSample2);

        BOOST_CHECK_CLOSE(varianceError1, varianceError2, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()
