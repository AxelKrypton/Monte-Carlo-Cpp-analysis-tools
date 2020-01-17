/*
 *
 *  Copyright (c) 2014 Christopher Pinke
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
#define BOOST_TEST_MODULE dataSample
#include "io_utilities.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(filenameForObservables)
{
    std::string dummyFilename = "foo";
    const char* arguments[] = {"foo", dummyFilename.c_str()};
    Parameters parameters(2, arguments);

    std::string expectedName = parameters.analysisOutputFilePrefix + dummyFilename + parameters.analysisOutputFilePostfix;

    BOOST_CHECK(expectedName == getFilenameForObservables(parameters));
}

BOOST_AUTO_TEST_CASE(writeEstimateAndErrorToFile_invalidArgument)
{
    std::string fileThatCannotBeOpened = "";
    const char* arguments[] = {"foo", fileThatCannotBeOpened.c_str()};
    Parameters parameters(2, arguments);

    BOOST_REQUIRE_THROW(writeEstimateAndErrorToFile(fileThatCannotBeOpened, 0., 0., fileThatCannotBeOpened), std::invalid_argument);
}
