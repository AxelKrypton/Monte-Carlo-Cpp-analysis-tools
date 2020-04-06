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

#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"  // for realFloatPrecisionInPercent

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(QuantitiesTest)

    const std::vector<std::string> labels = {"MEAN", "VARIANCE", "SKEWNESS", "KURTOSIS"};

    BOOST_AUTO_TEST_CASE(build)
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
        BOOST_REQUIRE_EQUAL(Mean::observableName, labels[0]);
        BOOST_REQUIRE_EQUAL(Variance::observableName, labels[1]);
        BOOST_REQUIRE_EQUAL(Skewness::observableName, labels[2]);
        BOOST_REQUIRE_EQUAL(Kurtosis::observableName, labels[3]);
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
