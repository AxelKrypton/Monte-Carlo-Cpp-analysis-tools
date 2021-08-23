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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MultiplDataSample

#include "MultipleDataSample.hpp"

#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        DataSample sample1(100);
        DataSample sample2(200);
        BOOST_REQUIRE_THROW(MultipleDataSample data(std::vector<DataSample>{sample1, sample2}), std::invalid_argument);
        BOOST_REQUIRE_NO_THROW(MultipleDataSample data(std::vector<DataSample>{sample1, sample1}));
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        BOOST_REQUIRE_NO_THROW(MultipleDataSample data("SampleDatafiles/datafileWithTwoColumns.example"));
        BOOST_REQUIRE_NO_THROW(MultipleDataSample data("SampleDatafiles/datafileWithTwoColumns.example", 1));
        BOOST_REQUIRE_NO_THROW(MultipleDataSample data("SampleDatafiles/datafileWithTwoColumns.example", 1, 2));
        BOOST_REQUIRE_NO_THROW(MultipleDataSample data("SampleDatafiles/datafileWithTwoColumns.example", 1, 2, 100));
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MomentCalculation)

    BOOST_AUTO_TEST_CASE(test1)
    {
        const auto numberOfElements = 10U;
        DataSample sample(makeValarrayWithOnes(numberOfElements));
        MultipleDataSample data(std::vector<DataSample>(2, sample));
        DataSample result1 = data.getNthMomentPerDataPoint(1);
        DataSample result2 = data.getNthMomentPerDataPoint(2);
        realFloat referenceValue = 1.0;
        for (auto i = 0U; i < numberOfElements; i++) {
            BOOST_REQUIRE_CLOSE(result1[i], referenceValue, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(result2[i], referenceValue, realFloatPrecisionInPercent);
        }
        BOOST_REQUIRE_THROW(data.getNthMomentPerDataPoint(3), std::logic_error);
        BOOST_REQUIRE_THROW(data.getNthMomentPerDataPoint(4), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        const auto numberOfElements = 10U;
        DataSample sample(makeValarrayWithOnes(numberOfElements));
        MultipleDataSample data(std::vector<DataSample>(4, sample));
        std::vector<DataSample> result(4);
        for (auto i = 0U; i < result.size(); i++)
            result[i] = data.getNthMomentPerDataPoint(i + 1);
        realFloat referenceValue = 1.0;
        for (auto moment = 0U; moment < result.size(); moment++)
            for (auto i = 0U; i < numberOfElements; i++)
                BOOST_REQUIRE_CLOSE(result[moment][i], referenceValue, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(test3)
    {
        const auto numberOfElements = 10U;
        DataSample sample(makeValarrayWithOnes(numberOfElements));
        MultipleDataSample data(std::vector<DataSample>(4, sample));
        data[1] *= -1;
        data[3] *= -1;
        std::vector<DataSample> result(4);
        for (auto i = 0U; i < result.size(); i++)
            result[i] = data.getNthMomentPerDataPoint(i + 1);
        std::vector<realFloat> referenceValues = {0.0, -1. / 3, 0.0, 1.0};
        for (auto moment = 0U; moment < result.size(); moment++)
            for (auto i = 0U; i < numberOfElements; i++)
                BOOST_REQUIRE_CLOSE(result[moment][i], referenceValues[moment], realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CentralMomentCalculation)

    BOOST_AUTO_TEST_CASE(test1)
    {
        DataSample sample(10);
        MultipleDataSample data(std::vector<DataSample>(2, sample));
        BOOST_REQUIRE_THROW(data.getNthCentralMomentPerDataPoint(1), std::logic_error);
    }

BOOST_AUTO_TEST_SUITE_END()
