/*
 *
 *  Copyright (c) 2020-2021 Alessandro Sciarra
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
#define BOOST_TEST_MODULE Variance

#include "Variance.hpp"

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "TestUtilities.hpp"

BOOST_AUTO_TEST_SUITE(VarianceAndError)

    realFloat expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
    {
        realFloat secondMoment = sample.getNthMoment(2);
        realFloat firstMoment = sample.getNthMoment(1);
        realFloat prefactor = numberOfElements / (numberOfElements - 1.);
        return prefactor * (secondMoment - pow(firstMoment, 2.));
    }

    static void testVarianceAndError(DataSample sample, EstimateAndError expected, realFloat testPrecision, bool isMeanKnownToBeZero = false)
    {
        Variance variance(sample, BinningParameters{}, QuantityAttributes{isMeanKnownToBeZero, false});
        checkEstimateAndError(expected, variance.value, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        int numberOfElements = 2674;
        DataSample sample(numberOfElements);
        realFloat expectedVariance = 0.;
        realFloat expectedError = 0.;
        testVarianceAndError(sample, {expectedVariance, expectedError}, realFloatPrecisionInPercent, true);
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        int numberOfElements = 1542;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, ones);
        realFloat expectedVariance = 0.;
        realFloat expectedError = 0.;
        testVarianceAndError(sample, {expectedVariance, expectedError}, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(test3)
    {
        int numberOfElements = 2345;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, arrayPosition);
        realFloat expectedVariance = 458447.5;
        realFloat expectedError = 8465.84748859;
        // TODO: check this again!
        // the difference in the error estimate exceeds 1e-13, most likely due to rounding errors.
        testVarianceAndError(sample, {expectedVariance, expectedError}, realFloatPrecisionInPercent * 1e3);
    }

    BOOST_AUTO_TEST_CASE(test4)
    {
        int numberOfElements = 2742;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, onesMinusOnes);
        realFloat expectedVariance = numberOfElements / (numberOfElements - 1.);
        realFloat expectedError = 0;
        testVarianceAndError(sample, {expectedVariance, expectedError}, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(test5)
    {
        /*
         * This test would fail if binning was done before calculating the second central moment,
         * because of rounding errors. The error is the standard deviation of
         *    {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0}
         * in the correct scenario, while it is the standard deviation of
         *    {0.2, -0.2, 0.2, -0.2, 0.2, -0.2, 0.2, -0.2, 0.2, -0.2}
         * in the wrong scenario and numerically this gives something in e-18.
         */
        int numberOfElements = 50;
        DataSample sample = getDataSampleBasedOnFillType(numberOfElements, onesMinusOnes);
        Variance variance(sample, BinningParameters{true, false, false, false, 5}, QuantityAttributes{});
        BOOST_REQUIRE_EQUAL(variance.value.error, 0);
    }
    BOOST_AUTO_TEST_CASE(withBinning1)
    {
        // This file has 1005 entries, from which 5 are discarded when binning with binsize 100
        std::string fileThatDoesExist = "SampleDatafiles/datafile.example";
        realFloat precisionOfDataInFileInPercent = 1e-10;
        DataSample sample(fileThatDoesExist);
        Variance variance1(sample, BinningParameters{}, QuantityAttributes{});
        Variance variance2(sample, BinningParameters{true, false, false, false, 201}, QuantityAttributes{});

        // Binning should not change estimate as long as data are not discarded
        BOOST_REQUIRE_CLOSE(variance1.value.estimate, variance2.value.estimate, precisionOfDataInFileInPercent);
    }

    BOOST_AUTO_TEST_CASE(withBinning2)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_0_3.dat";
        realFloat expectedVariance = 1.;
        realFloat expectedError = 5e-3;
        realFloat expectedPrecisionInPercent = 1;
        DataSample sample(gaussianData);
        Variance variance(sample, {true, false, false, false, 1000}, QuantityAttributes{});
        BOOST_CHECK_CLOSE(variance.value.estimate, expectedVariance, expectedPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.value.error, expectedError);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FromMomentsAndEstimator)

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
    {
        EstimateAndError referenceValue(0.2622374015645983, 0.0);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), QuantityAttributes{true, false}, bootstrap);
        BOOST_CHECK_CLOSE(variance.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), QuantityAttributes{false, false}, bootstrap);
        BOOST_CHECK_CLOSE(variance.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 3.0259317682406541e-06);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), QuantityAttributes{false, false}, bootstrap);
        BOOST_CHECK_CLOSE(variance.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_CLOSE(variance.value.error, referenceValue.error, realFloatPrecisionInPercent);
    }

    // TODO: Test for Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
    {
        EstimateAndError referenceValue(0.2622374015645983, 0.0);
        Variance variance(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(),
                          QuantityAttributes{true, true}, bootstrap);
        BOOST_CHECK_CLOSE(variance.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
        Variance variance(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(),
                          QuantityAttributes{false, true}, bootstrap);
        BOOST_CHECK_CLOSE(variance.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueZeroMean = 2.627932923896761e-01;
        double referenceValueNonZeroMean = 1.024474477934190e-05;
        DataSample resultZeroMean = Variance::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Variance::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for (int i = 0; i < resultZeroMean.getNumberOfElements(); i++) {
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
