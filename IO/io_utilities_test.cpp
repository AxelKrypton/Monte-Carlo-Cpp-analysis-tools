// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "io_utilities.hpp"

BOOST_AUTO_TEST_CASE(filenameForObservables)
{
	std::string dummyFilename = "foo";
	const char * arguments[] = {"foo", dummyFilename.c_str()};
	Parameters parameters(2, arguments);

	std::string expectedName = parameters.analysisOutputFilePrefix + dummyFilename + parameters.analysisOutputFilePostfix;

	BOOST_CHECK(expectedName == getFilenameForObservables(parameters));
}

BOOST_AUTO_TEST_CASE(writeEstimateAndErrorToFile_invalidArgument)
{
	std::string fileThatCannotBeOpened = "";
	const char * arguments[] = {"foo", fileThatCannotBeOpened.c_str()};
	Parameters parameters(2, arguments);

	BOOST_REQUIRE_THROW(writeEstimateAndErrorToFile(fileThatCannotBeOpened, 0., 0., fileThatCannotBeOpened), std::invalid_argument);
}


