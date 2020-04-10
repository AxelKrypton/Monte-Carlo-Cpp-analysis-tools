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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MultiplDataSample

#include "MultipleDataSample.hpp"

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
        DataSample sample1(100);
        MultipleDataSample data(std::vector<DataSample>{sample1, sample1});
        BOOST_REQUIRE_THROW(data.getNthMomentPerDataPoint(1), std::runtime_error);
    }

BOOST_AUTO_TEST_SUITE_END()
