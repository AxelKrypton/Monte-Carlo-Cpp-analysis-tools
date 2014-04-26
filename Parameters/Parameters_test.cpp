// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

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

	BOOST_AUTO_TEST_CASE(useBinning)
	{
		bool useBinning_default = true;
		BOOST_REQUIRE_EQUAL(useBinning_default, createParametersForDefaultCheck().useBinning);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation)
	{
		bool calcAutocorrelation_default = false;
		BOOST_REQUIRE_EQUAL(calcAutocorrelation_default, createParametersForDefaultCheck().calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean)
	{
		bool analyzeMean_default = true;
		BOOST_REQUIRE_EQUAL(analyzeMean_default, createParametersForDefaultCheck().analyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance)
	{
		bool analyzeVariance_default = true;
		BOOST_REQUIRE_EQUAL(analyzeVariance_default, createParametersForDefaultCheck().analyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness)
	{
		bool analyzeSkewness_default = true;
		BOOST_REQUIRE_EQUAL(analyzeSkewness_default, createParametersForDefaultCheck().analyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis)
	{
		bool analyzeKurtosis_default = true;
		BOOST_REQUIRE_EQUAL(analyzeKurtosis_default, createParametersForDefaultCheck().analyzeKurtosis);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setArguments)

	BOOST_AUTO_TEST_CASE(filename1)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "-f" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		Parameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.file);
	}

	BOOST_AUTO_TEST_CASE(filename2)
	{
		int numberOfArguments = 2;
		std::string filename = "someName";
		std::string filenameArgument = "--file=" + filename;
		const char * arguments[] = {"foo", filenameArgument.c_str()};
		Parameters parameters(numberOfArguments, arguments);

		BOOST_CHECK(filename == parameters.file);
	}

	static Parameters createParametersForArgumentSettingCheck_longOption(std::string argumentName, int newValue)
	{
		std::string argument = argumentName + "=" + boost::lexical_cast<std::string>(newValue);
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return Parameters(numberOfArguments, arguments);
	}

	static Parameters createParametersForArgumentSettingCheck_shortOption(std::string argumentName, int newValue)
	{
		std::string argument = argumentName + boost::lexical_cast<std::string>(newValue);
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return Parameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(binsize1)
	{
		int newValue = 999;
		std::string argumentName = "--binsize";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).binsize);
	}

	BOOST_AUTO_TEST_CASE(binsize2)
	{
		int newValue = 999;
		std::string argumentName = "-b";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).binsize);
	}

	BOOST_AUTO_TEST_CASE(numberOfBins1)
	{
		int newValue = 999;
		std::string argumentName = "--numberOfBins";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(numberOfBins2)
	{
		int newValue = 999;
		std::string argumentName = "-n";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(offset1)
	{
		int newValue = 999;
		std::string argumentName = "--offset";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).offset);
	}

	BOOST_AUTO_TEST_CASE(offset2)
	{
		int newValue = 999;
		std::string argumentName = "-o";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).offset);
	}

	static Parameters createParametersForArgumentSettingCheck_implicitOption(std::string argumentName)
	{
		std::string argument = argumentName;
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument.c_str()};
		return Parameters(numberOfArguments, arguments);
	}

	BOOST_AUTO_TEST_CASE(binning1)
	{
		bool newValue = false;
		std::string argumentName = "--useBinning";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).useBinning);
	}

	BOOST_AUTO_TEST_CASE(binning2)
	{
		bool newValue = false;
		std::string argumentName = "--useBinning";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).useBinning);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation1)
	{
		bool newValue = true;
		std::string argumentName = "--calcAutocorrelation";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation2)
	{
		bool newValue = true;
		std::string argumentName = "--calcAutocorrelation";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation3)
	{
		bool newValue = true;
		std::string argumentName = "-a";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean1)
	{
		bool newValue = true;
		std::string argumentName = "--analyzeMean";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).analyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean2)
	{
		bool newValue = false;
		std::string argumentName = "--analyzeMean";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).analyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance1)
	{
		bool newValue = true;
		std::string argumentName = "--analyzeVariance";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).analyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance2)
	{
		bool newValue = false;
		std::string argumentName = "--analyzeVariance";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).analyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness1)
	{
		bool newValue = true;
		std::string argumentName = "--analyzeSkewness";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).analyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness2)
	{
		bool newValue = false;
		std::string argumentName = "--analyzeSkewness";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).analyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis1)
	{
		bool newValue = true;
		std::string argumentName = "--analyzeKurtosis";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).analyzeKurtosis);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis2)
	{
		bool newValue = false;
		std::string argumentName = "--analyzeKurtosis";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).analyzeKurtosis);
	}

BOOST_AUTO_TEST_SUITE_END()
