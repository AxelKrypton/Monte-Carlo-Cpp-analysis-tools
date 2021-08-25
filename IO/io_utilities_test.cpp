/*
 *
 *  Copyright (c) 2014 Christopher Pinke
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
#define BOOST_TEST_MODULE dataSample
#include "io_utilities.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(filenameForObservable)
{
    std::string dummyFilename = "foo.dat";
    std::string expectedName = "foo_quantities.dat";
    Parameters parameters({"foo", dummyFilename.c_str(), "--calcAutocorrelation", "--timeMaxAutocorrelationFunction=10"});

    BOOST_CHECK(expectedName == getFilenameForObservables(parameters));
}

BOOST_AUTO_TEST_CASE(filenameForObservableNoExtension)
{
    std::string dummyFilename = "foo";
    std::string expectedName = "foo_quantities";
    Parameters parameters({"foo", dummyFilename.c_str(), "--calcAutocorrelation", "--timeMaxAutocorrelationFunction=10"});

    BOOST_CHECK(expectedName == getFilenameForObservables(parameters));
}

BOOST_AUTO_TEST_CASE(filenameForAutocorrelation)
{
    std::string dummyFilename = "foo.dat";
    std::string expectedName = "foo_autocorrelation.dat";
    Parameters parameters({"foo", dummyFilename.c_str(), "--calcAutocorrelation", "--timeMaxAutocorrelationFunction=10"});

    BOOST_CHECK(expectedName == getFilenameForAutocorrelation(parameters));
}

BOOST_AUTO_TEST_CASE(writeToFile_invalidArgument)
{
    std::string fileThatCannotBeOpened = "";
    Parameters parameters({"foo", fileThatCannotBeOpened.c_str()});

    BOOST_REQUIRE_THROW(writeEstimateAndErrorArraysToFile({}, fileThatCannotBeOpened), std::invalid_argument);
}
