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
#define BOOST_TEST_MODULE XXXXX_to_be_completed_XXXXX

#include "Variance.hpp"

#include "../dataSampleTestUtilities.hpp"  // for realFloatPrecisionInPercent
#include "TestUtilities.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(VarianceTest)

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
    {
        EstimateAndError referenceValue(0.2622374015645983, 0.0);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
        BOOST_CHECK_CLOSE(variance.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(variance.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 3.0259317682406541e-06);
        Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
        BOOST_CHECK_CLOSE(variance.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_CLOSE(variance.error, referenceValue.error, realFloatPrecisionInPercent);
    }

    // TODO: Test for Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
    {
        EstimateAndError referenceValue(0.2622374015645983, 0.0);
        Variance variance(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
        BOOST_CHECK_CLOSE(variance.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.error, 1.e-7);
    }

    BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
    {
        EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
        Variance variance(
            buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
        BOOST_CHECK_CLOSE(variance.estimate, referenceValue.estimate, realFloatPrecisionInPercent);
        BOOST_CHECK_SMALL(variance.error, 1.e-7);
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
