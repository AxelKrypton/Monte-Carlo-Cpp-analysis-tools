// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "Parameters.hpp"

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(invalidArgument)
	{
		int numberOfArguments = 1;
		char * argumentWithoutFile[] = {"foo"};
		BOOST_REQUIRE_THROW(Parameters params(numberOfArguments, argumentWithoutFile), std::invalid_argument );
	}

BOOST_AUTO_TEST_SUITE_END()
