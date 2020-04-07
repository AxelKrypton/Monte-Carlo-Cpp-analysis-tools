/*
 *
 *  Copyright (c) 2020 Alessandro Sciarra
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
#define BOOST_TEST_MODULE Mean

#include "Mean.hpp"

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "TestUtilities.hpp"

#include <boost/test/unit_test.hpp>

static void testMeanAndError(DataSample sample, EstimateAndError expectedMeanAndError)
{
    Mean mean(sample, BinningParameters{}, (expectedMeanAndError.estimate == 0.0));
    checkEstimateAndError(expectedMeanAndError, mean.value, realFloatPrecisionInPercent);
}

static void testMeanAndErrorFromFile(std::string file, int binsizeOrNumberOfBins, bool useBinsize, EstimateAndError expectedMeanAndError,
                                     realFloat testPrecision)
{
    std::vector<std::string> options;
    options.push_back("foo");
    options.push_back("--file=" + file);
    options.push_back(useBinsize ? "--binsize=" : "--numberOfBins=");
    options.back() += std::to_string(binsizeOrNumberOfBins);
    options.push_back("-m" + std::to_string(expectedMeanAndError.estimate == 0.0));
    Parameters parameters(options);
    DataSample sample(file);
    Mean mean(sample, parameters.getBinningParametersForObservablesAnalysis(constants::observableName<Mean>),
              (expectedMeanAndError.estimate == 0.0));
    checkEstimateAndError(expectedMeanAndError, mean.value, testPrecision);
}

BOOST_AUTO_TEST_SUITE(meanAndError)

    static realFloat expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample)
    {
        realFloat secondMoment = sample.getNthMoment(2);
        realFloat firstMoment = sample.getNthMoment(1);
        realFloat prefactor = 1. / (sample.getNumberOfElements() - 1.);
        return prefactor * (secondMoment - pow(firstMoment, 2.));
    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        int numberOfElements = 795;
        DataSample sample(numberOfElements);
        EstimateAndError expectedMeanAndError(0, 0);
        testMeanAndError(sample, expectedMeanAndError);
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        int numberOfElements = 1542;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, ones);
        EstimateAndError expectedMeanAndError(1, 0);
        testMeanAndError(sample, expectedMeanAndError);
    }

    BOOST_AUTO_TEST_CASE(test3)
    {
        int numberOfElements = 795;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, arrayPosition);
        EstimateAndError expectedMeanAndError;
        expectedMeanAndError.estimate = 0.5 * (numberOfElements - 1);
        expectedMeanAndError.error = sqrt(expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(sample));
        testMeanAndError(sample, expectedMeanAndError);
    }

    BOOST_AUTO_TEST_CASE(test4)
    {
        int numberOfElements = 4;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, onesMinusOnes);
        EstimateAndError expectedMeanAndError;
        expectedMeanAndError.estimate = 0.;
        expectedMeanAndError.error = sqrt(1. / (numberOfElements - 1));
        testMeanAndError(sample, expectedMeanAndError);
    }

    BOOST_AUTO_TEST_CASE(test5)
    {
        int numberOfElements = 4;
        std::valarray<realFloat> v1 = makeValarrayWithBigAndSmallEntries(numberOfElements);
        std::valarray<realFloat> v2(2 * numberOfElements);
        for (int i = 0; i < numberOfElements; i++) {
            v2[i] = v1[i];
            v2[numberOfElements + i] = -v1[i];
        }
        // Here we know that the mean is zero, although if numerically it is not!
        DataSample sample(v2);
        EstimateAndError expectedMeanAndError;
        expectedMeanAndError.estimate = 0.0;
        expectedMeanAndError.error = sqrt(expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(sample));
        testMeanAndError(sample, expectedMeanAndError);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromBinsize)

    // This file has 1005 entries, from which 5 are discarded when binning with binsize 100
    std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
    realFloat precisionOfDataInFileInPercent = 1e-10;

    BOOST_AUTO_TEST_CASE(error1)
    {
        int binsize = 1;
        realFloat expectedMean = 5.61305299427553583e-01;
        realFloat expectedError = 3.44121381077520906E-004;

        EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
        testMeanAndErrorFromFile(fileThatDoesExist, binsize, true, expectedEstimateAndError, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error2)
    {
        int binsize = 100;

        realFloat expectedMean = 0.56125906512982415;
        realFloat expectedError = 1.1564370727055974e-03;

        EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
        testMeanAndErrorFromFile(fileThatDoesExist, binsize, true, expectedEstimateAndError, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error3)
    {
        int binsize = 1;
        DataSample sample(fileThatDoesExist);
        Mean mean1(sample, BinningParameters{});
        Mean mean2(sample, BinningParameters{true, false, false, false, binsize});

        // Trivial binning should not do anything
        checkEstimateAndError(mean1.value, mean2.value, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error4)
    {
        int binsize = 201;
        DataSample sample(fileThatDoesExist);
        Mean mean1(sample, BinningParameters{});
        Mean mean2(sample, BinningParameters{true, false, false, true, binsize});

        // Binning for mean should not change estimate as long as data are not discarded
        BOOST_CHECK_CLOSE(mean1.value.estimate, mean2.value.estimate, precisionOfDataInFileInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromNumberOfBins)

    // This file has 1005 entries, from which 5 are discarded when binning with number of bins 10
    std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
    realFloat precisionOfDataInFileInPercent = 1e-10;

    BOOST_AUTO_TEST_CASE(error1)
    {
        int numberOfBins = 1005;
        realFloat expectedMean = 5.61305299427553583e-01;
        realFloat expectedError = 3.44121381077520906E-004;
        EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
        testMeanAndErrorFromFile(fileThatDoesExist, numberOfBins, false, expectedEstimateAndError, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error2)
    {
        int numberOfBins = 10;
        realFloat expectedMean = 0.56125906512982415;
        realFloat expectedError = 1.1564370727055974e-03;
        EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
        testMeanAndErrorFromFile(fileThatDoesExist, numberOfBins, false, expectedEstimateAndError, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error3)
    {
        int numberOfBins = 1005;
        DataSample sample(fileThatDoesExist);
        Mean mean1(sample, BinningParameters{});
        Mean mean2(sample, BinningParameters{true, false, false, true, numberOfBins});

        // Trivial binning should not do anything
        checkEstimateAndError(mean1.value, mean2.value, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(error4)
    {
        int numberOfBins = 5;
        DataSample sample(fileThatDoesExist);
        Mean mean1(sample, BinningParameters{});
        Mean mean2(sample, BinningParameters{true, false, false, true, numberOfBins});

        // Binning for mean should not change estimate as long as data are not discarded
        BOOST_CHECK_CLOSE(mean1.value.estimate, mean2.value.estimate, precisionOfDataInFileInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FromMomentsAndEstimators)

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
    {
        EstimateAndError referenceValue(0.0, 0.0);
        Mean mean(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(mean.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
    {
        EstimateAndError referenceValue(0.5120788163699608, 0.0);
        Mean mean(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(mean.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
    {
        EstimateAndError referenceValue(0.5120788163699608, 1.1520239122276158e-03);
        Mean mean(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_CLOSE(mean.value.error, referenceValue.error, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
    {
        EstimateAndError referenceValue(0.0, 0.0);
        Mean mean(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_CLOSE(mean.value.error, referenceValue.error, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
    {
        EstimateAndError referenceValue(0.0, 0.0);
        Mean mean(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(mean.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator6)
    {
        EstimateAndError referenceValue(0.5120788163699608, 0.0);
        Mean mean(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
        BOOST_CHECK_CLOSE(mean.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(mean.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueNonZeroMean = 5.126236900933244e-01;
        DataSample resultNonZeroMean = Mean::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for (int i = 0; i < resultNonZeroMean.getNumberOfElements(); i++) {
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
