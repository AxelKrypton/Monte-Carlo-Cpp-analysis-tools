/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018,2020 Alessandro Sciarra
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
#define BOOST_TEST_MODULE Observables

#include "Quantities.hpp"

#include "../Parameters/Parameters.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(QuantitiesTest)

    const std::vector<std::string> labels = {"MEAN", "VARIANCE", "SKEWNESS", "KURTOSIS"};
    std::string fileThatDoesExist = "SampleDatafiles/datafile.example";

    BOOST_AUTO_TEST_CASE(build1)
    {
        BOOST_REQUIRE_NO_THROW(Quantities quantities);
        Quantities quantities;
        // All entry NAN by default
        for (auto label : labels) {
            EstimateAndError quantity = quantities[label].value;
            BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
            BOOST_REQUIRE_NE(quantity.error, quantity.error);
        }
        // Test names
        BOOST_REQUIRE_EQUAL(constants::observableName<Mean>, labels[0]);
        BOOST_REQUIRE_EQUAL(constants::observableName<Variance>, labels[1]);
        BOOST_REQUIRE_EQUAL(constants::observableName<Skewness>, labels[2]);
        BOOST_REQUIRE_EQUAL(constants::observableName<Kurtosis>, labels[3]);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        std::vector<std::string> options
            = {"foo", "file", "--doNotAnalyzeMean", "--doNotAnalyzeVariance", "--doNotAnalyzeSkewness", "--doNotAnalyzeKurtosis"};
        Parameters parameters(options);
        DataSample sample;  // empty, anyway nothing should be calculated
        Quantities quantities(sample, parameters);
        // All entry NAN by default
        for (auto label : labels) {
            EstimateAndError quantity = quantities[label].value;
            BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
            BOOST_REQUIRE_NE(quantity.error, quantity.error);
        }
    }

    BOOST_AUTO_TEST_CASE(build3)
    {
        std::vector<std::string> options = {"foo", "file", "--doNotAnalyzeMean", "--isMeanKnownToBeZero"};
        Parameters parameters(options);
        DataSample sample(fileThatDoesExist);
        Quantities quantities(sample, parameters);
        // Not calculated entry NAN by default
        EstimateAndError quantity = quantities[labels[0]].value;
        BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
        BOOST_REQUIRE_NE(quantity.error, quantity.error);
    }

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::vector<std::string> options = {"foo", "file", "--doNotAnalyzeVariance"};
        Parameters parameters(options);
        DataSample sample(fileThatDoesExist);
        Quantities quantities(sample, parameters);
        // Not calculated entry NAN by default
        EstimateAndError quantity = quantities[labels[1]].value;
        BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
        BOOST_REQUIRE_NE(quantity.error, quantity.error);
    }
    BOOST_AUTO_TEST_CASE(build5)
    {
        std::vector<std::string> options = {"foo", "file", "--doNotAnalyzeSkewness"};
        Parameters parameters(options);
        DataSample sample(fileThatDoesExist);
        Quantities quantities(sample, parameters);
        // Not calculated entry NAN by default
        EstimateAndError quantity = quantities[labels[2]].value;
        BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
        BOOST_REQUIRE_NE(quantity.error, quantity.error);
    }

    BOOST_AUTO_TEST_CASE(build6)
    {
        std::vector<std::string> options = {"foo", "file", "--doNotAnalyzeKurtosis"};
        Parameters parameters(options);
        DataSample sample(fileThatDoesExist);
        Quantities quantities(sample, parameters);
        // Not calculated entry NAN by default
        EstimateAndError quantity = quantities[labels[3]].value;
        BOOST_REQUIRE_NE(quantity.estimate, quantity.estimate);
        BOOST_REQUIRE_NE(quantity.error, quantity.error);
    }

    BOOST_AUTO_TEST_CASE(accessOperator)
    {
        Quantities quantities;
        const Quantities quantities_const;

        for (auto label : labels) {
            BOOST_REQUIRE_NO_THROW(quantities[label]);
            BOOST_REQUIRE_NO_THROW(quantities_const[label]);
        }
        BOOST_REQUIRE_THROW(quantities["WRONG_LABEL"], std::out_of_range);
        BOOST_REQUIRE_THROW(quantities_const["WRONG_LABEL"], std::out_of_range);
    }

BOOST_AUTO_TEST_SUITE_END()

// TODO: Develop tests for Mean, Variance, Skewness, Kurtosis classes with Jackknife from estimators!
