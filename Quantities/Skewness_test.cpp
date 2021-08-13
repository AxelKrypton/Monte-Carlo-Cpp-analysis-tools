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
#define BOOST_TEST_MODULE Skewness

#include "Skewness.hpp"

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "TestUtilities.hpp"

BOOST_AUTO_TEST_SUITE(SkewnessAndError)

    const std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

    BOOST_AUTO_TEST_CASE(withBinning1)
    {
        DataSample sample(gaussianData);
        Skewness skewness1(sample, BinningParameters{}, QuantityAttributes{});
        Skewness skewness2(sample, BinningParameters{true, false, false, false, 5000}, QuantityAttributes{});

        // Binning should change estimate
        BOOST_REQUIRE_NE(skewness1.value.estimate, skewness2.value.estimate);
    }

    BOOST_AUTO_TEST_CASE(withBinning2)
    {
        realFloat expectedSkewness = 1.;
        DataSample sample(gaussianData);
        Skewness skewness(sample, BinningParameters{true, false, false, false, 100}, QuantityAttributes{});
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = std::fabs(skewness.value.estimate - expectedSkewness) / skewness.value.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(std::fabs(skewness.value.error / skewness.value.estimate) < 0.001);
    }

    BOOST_AUTO_TEST_CASE(withBinning3)
    {
        realFloat expectedSkewness = 1.;
        DataSample sample(gaussianData);
        Skewness skewness(sample, BinningParameters{true, false, false, false, 100}, QuantityAttributes{true, false});
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = std::fabs(skewness.value.estimate - expectedSkewness) / skewness.value.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(std::fabs(skewness.value.error / skewness.value.estimate) < 0.001);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FromMomentsAndEstimator)

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
    {
        EstimateAndError referenceValue(1.000072760979389, 0.0);
        Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), QuantityAttributes{true, false}, bootstrap);
        BOOST_CHECK_CLOSE(skewness.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(skewness.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
    {
        EstimateAndError referenceValue(0.5694793357428045, 0.0);
        Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), QuantityAttributes{false, false}, bootstrap);
        BOOST_CHECK_CLOSE(skewness.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(skewness.value.error, 1.e-7);
    }

    // TODO: Test for Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
    // TODO: Test for Skewness variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
    {
        EstimateAndError referenceValue(1.000072760979389, 0.0);
        Skewness skewness(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(),
                          QuantityAttributes{true, true}, bootstrap);
        BOOST_CHECK_CLOSE(skewness.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(skewness.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
    {
        EstimateAndError referenceValue(0.5694793357428045, 0.0);
        Skewness skewness(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(),
                          QuantityAttributes{false, true}, bootstrap);
        BOOST_CHECK_CLOSE(skewness.value.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(skewness.value.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueZeroMean = 1.0000586137849100;
        double referenceValueNonZeroMean = 0.5728362972699335;
        DataSample resultZeroMean = Skewness::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Skewness::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for (int i = 0; i < resultZeroMean.getNumberOfElements(); i++) {
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
