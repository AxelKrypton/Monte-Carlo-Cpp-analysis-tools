// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LqcdReweightingParameters
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "LqcdReweightingParameters.hpp"

BOOST_AUTO_TEST_SUITE(LqcdReweightingParameters_build)

	BOOST_AUTO_TEST_CASE(invalidArgument)
	{
		int numberOfArguments = 1;
		const char * argumentWithoutFile[] = {"foo"};
		BOOST_REQUIRE_THROW(LqcdReweightingParameters parameters(numberOfArguments, argumentWithoutFile), std::invalid_argument );
	}
	
	BOOST_AUTO_TEST_CASE(build1)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "someFilename"};
		BOOST_CHECK_NO_THROW(LqcdReweightingParameters parameters(numberOfArguments, argumentWithFile));
	}
	
	BOOST_AUTO_TEST_CASE(help1)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "-h"};
		BOOST_REQUIRE_THROW(LqcdReweightingParameters parameters(numberOfArguments, argumentsWithHelp), LqcdReweightingParameters::parse_aborted );
	}

	BOOST_AUTO_TEST_CASE(help2)
	{
		int numberOfArguments = 2;
		const char * argumentsWithHelp[] = {"foo", "--help"};
		BOOST_REQUIRE_THROW(LqcdReweightingParameters parameters(numberOfArguments, argumentsWithHelp), LqcdReweightingParameters::parse_aborted );
	}
	
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(defaults)

	static LqcdReweightingParameters createParametersForDefaultCheck()
	{
		int numberOfArguments = 2;
		const char * arguments[] = {"foo", "-f dummyFile"};
		return LqcdReweightingParameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(numberOfNewPoints)
	{
		uint defaultValue = 2;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNumberOfNewBetaPoints() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_high)
	{
		double defaultValue = 2.;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNewBetaRange_high() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_low)
	{
		double defaultValue = 1.;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getNewBetaRange_low() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForMean)
	{
		bool defaultValue = false;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getDeactivateReweightingForMean() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForVariance)
	{
		bool defaultValue = false;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getDeactivateReweightingForVariance() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForSkewness)
	{
		bool defaultValue = false;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getDeactivateReweightingForSkewness() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForBinder)
	{
		bool defaultValue = false;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getDeactivateReweightingForBinder() );
	}
	
	BOOST_AUTO_TEST_CASE(outputfilePrefix)
	{
		std::string defaultValue = "reweightedData";
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().getOutputfilePrefix() );
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setArguments)

	BOOST_AUTO_TEST_CASE(filename1)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "-f" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		LqcdReweightingParameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.getInputfile());
	}

	BOOST_AUTO_TEST_CASE(filename2)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "--file=" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		LqcdReweightingParameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.getInputfile());
	}

	static LqcdReweightingParameters createParametersForArgumentSettingCheck_longOption(std::string argumentName, int newValue)
	{
		std::string argument = argumentName + "=" + boost::lexical_cast<std::string>(newValue);
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return LqcdReweightingParameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(numberOfNewPoints)
	{
		uint newValue = 2;
		std::string argumentName = "--numberOfNewBetaPoints";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNumberOfNewBetaPoints() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_high)
	{
		double newValue = 65.;
		std::string argumentName = "--newBetaRange_high";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNewBetaRange_high() );
	}
	
	BOOST_AUTO_TEST_CASE(newRange_low)
	{
		double newValue = 14.;
		std::string argumentName = "--newBetaRange_low";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getNewBetaRange_low() );
	}
	
	static LqcdReweightingParameters createLqcdReweightingParametersForArgumentSettingCheck_implicitOption(std::string argumentName)
	{
		std::string argument = argumentName;
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return LqcdReweightingParameters(numberOfArguments, arguments);
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForMean_implicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForMean";
		BOOST_REQUIRE_EQUAL(newValue, createLqcdReweightingParametersForArgumentSettingCheck_implicitOption(argumentName).getDeactivateReweightingForMean() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForMean_explicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForMean";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getDeactivateReweightingForMean() );
	}

	BOOST_AUTO_TEST_CASE(deactivateReweightingForVariance_implicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForVariance";
		BOOST_REQUIRE_EQUAL(newValue, createLqcdReweightingParametersForArgumentSettingCheck_implicitOption(argumentName).getDeactivateReweightingForVariance() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForVariance_explicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForVariance";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getDeactivateReweightingForVariance() );
	}

	BOOST_AUTO_TEST_CASE(deactivateReweightingForSkewness_implicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForSkewness";
		BOOST_REQUIRE_EQUAL(newValue, createLqcdReweightingParametersForArgumentSettingCheck_implicitOption(argumentName).getDeactivateReweightingForSkewness() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForSkewness_explicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForSkewness";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getDeactivateReweightingForSkewness() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForBinder_implicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForBinder";
		BOOST_REQUIRE_EQUAL(newValue, createLqcdReweightingParametersForArgumentSettingCheck_implicitOption(argumentName).getDeactivateReweightingForBinder() );
	}
	
	BOOST_AUTO_TEST_CASE(deactivateReweightingForBinder_explicit)
	{
		bool newValue = true;
		std::string argumentName = "--deactivateReweightingForBinder";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).getDeactivateReweightingForBinder() );
	}
	
	static LqcdReweightingParameters createParametersForArgumentSettingCheck_string(std::string argumentName, std::string newValue)
	{
        std::string argument = argumentName + "=" +  newValue;
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return LqcdReweightingParameters(numberOfArguments, arguments);
	}
	
	BOOST_AUTO_TEST_CASE(outputfilePrefix)
	{
		std::string newValue = "abcdefg";
		std::string argumentName = "--outputfilePrefix";
		BOOST_REQUIRE_EQUAL(newValue, createParametersForArgumentSettingCheck_string(argumentName, newValue).getOutputfilePrefix() );
	}

    BOOST_AUTO_TEST_CASE(observablesMultipleColumns)
    {
        std::string newValues = "5";
        std::string argumentName = "--obsMultipleColumns";
        std::vector<unsigned int> refValues;
        refValues.push_back(5);
        BOOST_REQUIRE(refValues == createParametersForArgumentSettingCheck_string(argumentName, newValues).getColumnsToBeReweightedUsingMultipleColumns() );
    }

    BOOST_AUTO_TEST_CASE(observablesZeroMean)
	{
		std::string newValues = "5";
		std::string argumentName = "--obsWithZeroMean";
		std::vector<unsigned int> refValues;
		refValues.push_back(5);
		BOOST_REQUIRE(refValues == createParametersForArgumentSettingCheck_string(argumentName, newValues).getColumnsWhoseMeanIsKnownToBeZero() );
	}
	
BOOST_AUTO_TEST_SUITE_END()
