// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "io_utilities.hpp"

BOOST_AUTO_TEST_CASE(FilenameForObservables)
{
	std::string dummyFilename = "foo";
	const char * arguments[] = {"foo", dummyFilename.c_str()};
	Parameters parameters(2, arguments);

	std::string expectedName = parameters.analysisOutputFilePrefix + dummyFilename + parameters.analysisOutputFilePostfix;

	BOOST_CHECK(expectedName == getFilenameForObservables(parameters));
}


