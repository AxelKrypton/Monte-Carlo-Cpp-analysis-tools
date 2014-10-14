// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE reweightingParameters
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "ReweightingParameters.hpp"

BOOST_AUTO_TEST_SUITE(ReweightingParameters_build)

	BOOST_AUTO_TEST_CASE(invalidArgument)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "someFilename"};
		BOOST_CHECK_NO_THROW(ReweightingParameters parameters(numberOfArguments, argumentWithFile));
	}
	
BOOST_AUTO_TEST_SUITE_END()