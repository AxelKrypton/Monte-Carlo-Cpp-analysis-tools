// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE reweightingParameters
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "ReweightingParameters.hpp"

BOOST_AUTO_TEST_SUITE(ReweightingParameters_build)

	BOOST_AUTO_TEST_CASE(invalidArgument)
	{
		int numberOfArguments = 1;
		const char * argumentWithoutFile[] = {"foo"};
		BOOST_REQUIRE_THROW(ReweightingParameters parameters(numberOfArguments, argumentWithoutFile), std::invalid_argument );
	}
	
	BOOST_AUTO_TEST_CASE(build1)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "someFilename"};
		BOOST_CHECK_NO_THROW(ReweightingParameters parameters(numberOfArguments, argumentWithFile));
	}
	
	BOOST_AUTO_TEST_CASE(help1)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "-h"};
		BOOST_REQUIRE_THROW(ReweightingParameters parameters(numberOfArguments, argumentsWithHelp), ReweightingParameters::parse_aborted );
	}

	BOOST_AUTO_TEST_CASE(help2)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "--help"};
		BOOST_REQUIRE_THROW(ReweightingParameters parameters(numberOfArguments, argumentsWithHelp), ReweightingParameters::parse_aborted );
	}
	
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(defaults)

	static ReweightingParameters createParametersForDefaultCheck()
	{
		int numberOfArguments = 2;
		const char * arguments[] = {"foo", "-f dummyFile"};
		return ReweightingParameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(numberOfNewPoints)
	{
		uint defaultValue = 2;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNumberOfNewPoints() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_high)
	{
		double defaultValue = 2.;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNewRange_high() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_low)
	{
		double defaultValue = 1.;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNewRange_low() );
	}

BOOST_AUTO_TEST_SUITE_END()
