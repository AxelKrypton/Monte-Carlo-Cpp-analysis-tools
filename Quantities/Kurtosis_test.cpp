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
#define BOOST_TEST_MODULE Kurtosis

#include "Kurtosis.hpp"

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "TestUtilities.hpp"

BOOST_AUTO_TEST_SUITE(KurtosisAndError)

    const std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

    BOOST_AUTO_TEST_CASE(withBinning1)
    {
        DataSample sample(gaussianData);
        Kurtosis kurtosis1(sample, {});
        Kurtosis kurtosis2(sample, {true, false, false, false, 5000});

        // Binning should change kurtosis estimate
        BOOST_REQUIRE_NE(kurtosis1.value.estimate, kurtosis2.value.estimate);
    }

    BOOST_AUTO_TEST_CASE(withBinning2)
    {
        realFloat expectedKurtosis = 3.;
        DataSample sample(gaussianData);
        Kurtosis kurtosis(sample, {true, false, false, false, 100});
        std::cout << "Kurtosis = " << kurtosis.value.estimate << " +/- " << kurtosis.value.error << "\n";
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = std::fabs(kurtosis.value.estimate - expectedKurtosis) / kurtosis.value.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(kurtosis.value.error / kurtosis.value.estimate) < 0.001);
    }

    BOOST_AUTO_TEST_CASE(withBinning3)
    {
        realFloat expectedKurtosis = 3.;
        DataSample sample(gaussianData);
        Kurtosis kurtosis(sample, {true, false, false, false, 100}, true);
        std::cout << "Kurtosis = " << kurtosis.value.estimate << " +/- " << kurtosis.value.error << "\n";
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = std::fabs(kurtosis.value.estimate - expectedKurtosis) / kurtosis.value.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(kurtosis.value.error / kurtosis.value.estimate) < 0.001);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(KurtosisTest)

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
    {
        EstimateAndError referenceValue(1.000194288875983, 0.0);
        Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
        BOOST_CHECK_CLOSE(kurtosis.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(kurtosis.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
    {
        EstimateAndError referenceValue(3.7478114121524830, 0.0);
        Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(kurtosis.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(kurtosis.value.error, 3.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
    {
        EstimateAndError referenceValue(3.7478114121524830, 7.2132403917648602e-01);
        Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(kurtosis.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_CLOSE(kurtosis.value.error, referenceValue.error, realFloatPrecisionInPercent);
    }

    // TODO: Test for Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
    {
        EstimateAndError referenceValue(1.000194288875983, 0.0);
        Kurtosis kurtosis(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
        BOOST_CHECK_CLOSE(kurtosis.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(kurtosis.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
    {
        EstimateAndError referenceValue(3.7478114121524830, 0.0);
        Kurtosis kurtosis(
            buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
        BOOST_CHECK_CLOSE(kurtosis.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(kurtosis.value.error, 3.e-7);
    }

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueZeroMean = 1.00015649139308;
        double referenceValueNonZeroMean = 3.36129098137853;
        DataSample resultZeroMean = Kurtosis::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Kurtosis::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for (int i = 0; i < resultZeroMean.getNumberOfElements(); i++) {
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
