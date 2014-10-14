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

BOOST_AUTO_TEST_SUITE(setArguments)

	BOOST_AUTO_TEST_CASE(filename1)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "-f" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		ReweightingParameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.getInputfile());
	}

	BOOST_AUTO_TEST_CASE(filename2)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "--file=" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		ReweightingParameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.getInputfile());
	}

	static ReweightingParameters createParametersForArgumentSettingCheck_longOption(std::string argumentName, int newValue)
	{
		std::string argument = argumentName + "=" + boost::lexical_cast<std::string>(newValue);
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return ReweightingParameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(numberOfNewPoints)
	{
		uint newValue = 2;
		std::string argumentName = "--numberOfNewPoints";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNumberOfNewPoints() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_high)
	{
		double newValue = 65.;
		std::string argumentName = "--newRange_high";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNewRange_high() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_low)
	{
		double newValue = 14.;
		std::string argumentName = "--newRange_low";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNewRange_low() );
	}

BOOST_AUTO_TEST_SUITE_END()