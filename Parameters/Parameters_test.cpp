// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "Parameters.hpp"

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(invalidArgument)
	{
		int numberOfArguments = 1;
		const char * argumentWithoutFile[] = {"foo"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, argumentWithoutFile), std::invalid_argument );
	}

	BOOST_AUTO_TEST_CASE(help1)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "-h"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, argumentsWithHelp), Parameters::parse_aborted );
	}

	BOOST_AUTO_TEST_CASE(help2)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "--help"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, argumentsWithHelp), Parameters::parse_aborted );
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(defaults)

	static Parameters createParametersForDefaultCheck()
	{
		int numberOfArguments = 2;
		const char * arguments[] = {"foo", "-f dummyFile"};
		return Parameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(binsize)
	{
		int binsize_default = 100;
		BOOST_REQUIRE_EQUAL(binsize_default, createParametersForDefaultCheck().binsize);
	}

	BOOST_AUTO_TEST_CASE(numberOfBins)
	{
		int numberOfBins_default = 10;
		BOOST_REQUIRE_EQUAL(numberOfBins_default, createParametersForDefaultCheck().numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(offset)
	{
		int offset_default = 0;
		BOOST_REQUIRE_EQUAL(offset_default, createParametersForDefaultCheck().offset);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setArguments)

BOOST_AUTO_TEST_CASE(filename)
{
	int numberOfArguments = 2;
	std::string filename = "someName";
	std::string filenameArgument = "-f" + filename;
	const char * arguments[] = {"foo", filenameArgument.c_str()};
	Parameters parameters(numberOfArguments, arguments);

	BOOST_CHECK(filename == parameters.file);
}

BOOST_AUTO_TEST_SUITE_END()


