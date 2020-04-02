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
#include "Observables.hpp"

#include "../dataSampleTestUtilities.hpp"  // for realFloatPrecisionInPercent

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(QuantitiesTest)

    BOOST_AUTO_TEST_CASE(build)
    {
        BOOST_REQUIRE_NO_THROW(Observables observables);
        Observables observables;
        // All entry NAN by default
        BOOST_REQUIRE_NE(observables.mean.estimate, observables.mean.estimate);
        BOOST_REQUIRE_NE(observables.mean.error, observables.mean.error);
        BOOST_REQUIRE_NE(observables.susceptibility.estimate, observables.susceptibility.estimate);
        BOOST_REQUIRE_NE(observables.susceptibility.error, observables.susceptibility.error);
        BOOST_REQUIRE_NE(observables.skewness.estimate, observables.skewness.estimate);
        BOOST_REQUIRE_NE(observables.skewness.error, observables.skewness.error);
        BOOST_REQUIRE_NE(observables.kurtosis.estimate, observables.kurtosis.estimate);
        BOOST_REQUIRE_NE(observables.kurtosis.error, observables.kurtosis.error);
        // Test names
        BOOST_REQUIRE_EQUAL(observables.observableNames[0], "mean");
        BOOST_REQUIRE_EQUAL(observables.observableNames[1], "variance");
        BOOST_REQUIRE_EQUAL(observables.observableNames[2], "skewness");
        BOOST_REQUIRE_EQUAL(observables.observableNames[3], "kurtosis");
    }

BOOST_AUTO_TEST_SUITE_END()

// TODO: Develop tests for Mean, Variance, Skewness, Kurtosis classes with Jackknife from estimators!
