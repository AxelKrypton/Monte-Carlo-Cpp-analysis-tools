/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2016,2018,2020 Alessandro Sciarra
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
#include "dataAnalysisUtilities.hpp"

#include "Quantities/TestUtilities.hpp"
#include "TestDataSample.hpp"
#include "binnedDataSample.hpp"
#include "binning.hpp"

#include <boost/test/unit_test.hpp>
#include <iomanip>

BOOST_AUTO_TEST_SUITE(varianceAndError)

    realFloat expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
    {
        realFloat secondMoment = sample.getNthMoment(2);
        realFloat firstMoment = sample.getNthMoment(1);
        realFloat prefactor = numberOfElements / (numberOfElements - 1.);
        return prefactor * (secondMoment - pow(firstMoment, 2.));
    }

    static void testVarianceAndError(DataSample * sample, EstimateAndError expected, realFloat testPrecision, bool isMeanKnownToBeZero = false)
    {
        if (isMeanKnownToBeZero) {
            EstimateAndError varianceAndError_zeroMean = calcVarianceAndErrorOfUncorrelatedDataSample(*sample, true);
            // Trivial test, but better than nothing
            checkEstimateAndError(expected, varianceAndError_zeroMean, testPrecision);
        }
        // Leave in any case this test, more significant
        EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
        checkEstimateAndError(expected, varianceAndError, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(variance)
    {
        int numberOfElements = 2674;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();
        realFloat expectedValue = expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements);

        EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
        BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedValue, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        int numberOfElements = 2674;
        DataSample sample(numberOfElements);

        realFloat expectedVariance = 0.;
        realFloat expectedError = 0.;

        EstimateAndError expected(expectedVariance, expectedError);
        testVarianceAndError(&sample, expected, realFloatPrecisionInPercent, true);
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        int numberOfElements = 1542;
        TestDataSample<realFloat> testSample(numberOfElements, ones);
        DataSample* sample = testSample.getDataSample();

        realFloat expectedVariance = 0.;
        realFloat expectedError = 0.;

        EstimateAndError expected(expectedVariance, expectedError);
        testVarianceAndError(sample, expected, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(test3)
    {
        int numberOfElements = 2345;
        TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
        DataSample* sample = testSample.getDataSample();

        realFloat expectedVariance = 458447.5;
        realFloat expectedError = 8465.84748859;

        // todo: check this again!
        // the difference in the error estimate exceeds 1e-13, most likely due to rounding errors.
        realFloat testPrecision = realFloatPrecisionInPercent * 1e3;

        EstimateAndError expected(expectedVariance, expectedError);
        testVarianceAndError(sample, expected, testPrecision);
    }

    BOOST_AUTO_TEST_CASE(test4)
    {
        int numberOfElements = 2742;
        TestDataSample<realFloat> testSample(numberOfElements, onesMinusOnes);
        DataSample* sample = testSample.getDataSample();

        realFloat expectedVariance = numberOfElements / (numberOfElements - 1.);
        realFloat expectedError = 0;

        EstimateAndError expected(expectedVariance, expectedError);
        testVarianceAndError(sample, expected, realFloatPrecisionInPercent, true);
    }

    BOOST_AUTO_TEST_CASE(withBinning)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_0_3.dat";

        realFloat expectedVariance = 1.;
        realFloat expectedError = 5e-3;

        realFloat expectedPrecisionInPercent = 1;

        const char* arguments[] = {"foo", "--binsize=1000", gaussianData.c_str()};
        Parameters parameters(3, arguments);

        DataSample sample(gaussianData);

        EstimateAndError varianceAndError = calcVarianceAndErrorOfDataSample(sample, parameters);
        BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedVariance, expectedPrecisionInPercent);
        BOOST_CHECK_SMALL(varianceAndError.error, expectedError);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(skewnessAndError)

    BOOST_AUTO_TEST_CASE(withBinning1)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

        realFloat expectedSkewness = 1.;

        const char* arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
        Parameters parameters(3, arguments);

        DataSample sample(gaussianData);

        EstimateAndError skewnessAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = fabs(skewnessAndError.estimate - expectedSkewness) / skewnessAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(skewnessAndError.error / skewnessAndError.estimate) < 0.001);
    }

    BOOST_AUTO_TEST_CASE(withBinning2)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

        realFloat expectedSkewness = 1.;

        const char* arguments[] = {"foo", "--isMeanKnownToBeZero", "--binsize=100", gaussianData.c_str()};
        Parameters parameters(4, arguments);

        DataSample sample(gaussianData);

        EstimateAndError skewnessAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = fabs(skewnessAndError.estimate - expectedSkewness) / skewnessAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(skewnessAndError.error / skewnessAndError.estimate) < 0.001);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(kurtosisAndError)

    BOOST_AUTO_TEST_CASE(withBinning1)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

        realFloat expectedKurtosis = 3.;

        const char* arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
        Parameters parameters(3, arguments);

        DataSample sample(gaussianData);

        EstimateAndError kurtosisAndError = calcKurtosisAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = fabs(kurtosisAndError.estimate - expectedKurtosis) / kurtosisAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(kurtosisAndError.error / kurtosisAndError.estimate) < 0.001);
    }

    BOOST_AUTO_TEST_CASE(withBinning2)
    {
        std::string gaussianData = "SampleDatafiles/gaussianNumbers_0_1_1_3.dat";

        realFloat expectedKurtosis = 3.;

        const char* arguments[] = {"foo", "--isMeanKnownToBeZero", "--binsize=100", gaussianData.c_str()};
        Parameters parameters(4, arguments);

        DataSample sample(gaussianData);

        EstimateAndError kurtosisAndError = calcKurtosisAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue
            = fabs(kurtosisAndError.estimate - expectedKurtosis) / kurtosisAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN(fabs(kurtosisAndError.error / kurtosisAndError.estimate) < 0.001);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(autocorrelation)

    BOOST_AUTO_TEST_CASE(corrFuncTestVsBerg)
    {
        const char* arguments[] = {"foo", "--file=SampleDatafiles/gaussianNumbers_Berg.dat", "-a", "--timeMaxAutocorrelationFunction=128",
                                   "--numberOfBinsForAutocorrelation=32"};
        Parameters parameters(5, arguments);
        DataSample gaussianCorrelatedBergData(parameters.file, 2);
        DataSample referenceCorrFuncValues("SampleDatafiles/autFuncBergRefResult32bins.dat", 2);
        DataSample referenceCorrFuncErrors("SampleDatafiles/autFuncBergRefResult32bins.dat", 3);

        std::vector<EstimateAndError> result
            = calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(gaussianCorrelatedBergData, parameters);

        /*
         * Since in the Berg Code the jackknife estimators are always calculated without
         * any analytic simplification, the reference result are too different from ours.
         * Then we just implemented this analytic simplification in his code.
         * [the modification consists in introducing after line "CALL DATJACK(NBINS,WORK,ACORJ)"
         *  of the file autcorj.f the for loop
         *     DO IBINS=1,NBINS
         *       ACORJ(IBINS)=WORK(IBINS)
         *     END DO
         *  and in the file at21.f divide the ACE output variable of STEBJ0 by (NBINS-1) when
         *  it is print to screen]
         *
         * Nevertheless there are some rounding errors because the calculation is carried out differently
         * from how we do. This is the reason why here we do not use "realFloatPrecisionInPercent"
         * but only 3.e-10 in the boost check of the error.
         */
        for (unsigned int i = 0; i < result.size(); i++) {
            BOOST_REQUIRE_CLOSE(result[i].estimate, referenceCorrFuncValues[i], 3.e-10);
            BOOST_REQUIRE_CLOSE(result[i].error, referenceCorrFuncErrors[i], 3.e-10);
        }
    }

    BOOST_AUTO_TEST_CASE(tauTestVsBerg)
    {
        const char* arguments[] = {"foo", "--file=SampleDatafiles/gaussianNumbers_Berg.dat", "-a", "--timeMaxAutocorrelationFunction=128",
                                   "--numberOfBinsForAutocorrelation=32"};
        Parameters parameters(5, arguments);
        DataSample gaussianCorrelatedBergData(parameters.file, 2);
        DataSample referenceTauValues("SampleDatafiles/intTauBergRefResult32bins.dat", 2);
        DataSample referenceTauErrors("SampleDatafiles/intTauBergRefResult32bins.dat", 3);

        std::vector<EstimateAndError> result
            = calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(gaussianCorrelatedBergData, parameters);

        /*
         * See the comment above for the reason why we use 1.e-10 instead
         * of "realFloatPrecisionInPercent" in the boost check.
         */
        for (unsigned int i = 0; i < result.size(); i++) {
            BOOST_CHECK_CLOSE(result[i].estimate, referenceTauValues[i], 1.e-10);
            BOOST_CHECK_CLOSE(result[i].error, referenceTauErrors[i], 1.e-10);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
