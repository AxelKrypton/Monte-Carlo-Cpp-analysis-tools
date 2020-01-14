/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2014-2015,2018,2020 Alessandro Sciarra
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
#define BOOST_TEST_MODULE parameters
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

	BOOST_AUTO_TEST_CASE(build1)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "-f foo"};
		BOOST_CHECK_NO_THROW(Parameters parameters(numberOfArguments, argumentWithFile) );
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "--file=foo"};
		BOOST_CHECK_NO_THROW(Parameters parameters(numberOfArguments, argumentWithFile) );
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		int numberOfArguments = 2;
		const char * argumentWithFile[] = {"foo", "foo"};
		BOOST_CHECK_NO_THROW(Parameters parameters(numberOfArguments, argumentWithFile) );
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

	BOOST_AUTO_TEST_CASE(autocorrelation1)
	{
		int numberOfArguments = 3;
		const char * argumentsWithAutocorrelation[] = {"foo", "foo", "-a"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, argumentsWithAutocorrelation), std::invalid_argument );
	}

	BOOST_AUTO_TEST_CASE(autocorrelation2)
	{
		int numberOfArguments = 3;
		const char * argumentsWithAutocorrelation[] = {"foo", "foo", "--calcAutocorrelation"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, argumentsWithAutocorrelation), std::invalid_argument );
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

	BOOST_AUTO_TEST_CASE(numberOfBinsForAutocorrelation)
	{
		int numberOfBinsForAutocorrelation_default = 10;
		BOOST_REQUIRE_EQUAL(numberOfBinsForAutocorrelation_default, createParametersForDefaultCheck().numberOfBinsForAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(offset)
	{
		int offset_default = 0;
		BOOST_REQUIRE_EQUAL(offset_default, createParametersForDefaultCheck().offset);
	}
	
	BOOST_AUTO_TEST_CASE(column)
	{
		int column_default = 1;
		BOOST_REQUIRE_EQUAL(column_default, createParametersForDefaultCheck().column);
	}

	BOOST_AUTO_TEST_CASE(isMeanKnownToBeZero)
	{
		bool isMeanKnownToBeZero_default = false;
		BOOST_REQUIRE_EQUAL(isMeanKnownToBeZero_default, createParametersForDefaultCheck().isMeanKnownToBeZero);
	}

	BOOST_AUTO_TEST_CASE(doNotUseBinning)
	{
		bool doNotUseBinning_default = false;
		BOOST_REQUIRE_EQUAL(doNotUseBinning_default, createParametersForDefaultCheck().doNotUseBinning);
	}

	BOOST_AUTO_TEST_CASE(useNumberOfBinsForBinning)
	{
		   bool defaultValue = false;
		   BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().useNumberOfBinsForBinning);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation)
	{
		bool calcAutocorrelation_default = false;
		BOOST_REQUIRE_EQUAL(calcAutocorrelation_default, createParametersForDefaultCheck().calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean)
	{
		bool doNotAnalyzeMean_default = false;
		BOOST_REQUIRE_EQUAL(doNotAnalyzeMean_default, createParametersForDefaultCheck().doNotAnalyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance)
	{
		bool doNotAnalyzeVariance_default = false;
		BOOST_REQUIRE_EQUAL(doNotAnalyzeVariance_default, createParametersForDefaultCheck().doNotAnalyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness)
	{
		bool doNotAnalyzeSkewness_default = false;
		BOOST_REQUIRE_EQUAL(doNotAnalyzeSkewness_default, createParametersForDefaultCheck().doNotAnalyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis)
	{
		bool doNotAnalyzeKurtosis_default = false;
		BOOST_REQUIRE_EQUAL(doNotAnalyzeKurtosis_default, createParametersForDefaultCheck().doNotAnalyzeKurtosis);
	}

	BOOST_AUTO_TEST_CASE(analysisOutputFilePrefix)
	{
		std::string defaultValue = "";
		const char * arguments[] = {"foo", "foo"};
		Parameters parameters(2, arguments);

		BOOST_CHECK(defaultValue == parameters.analysisOutputFilePrefix);
	}

	BOOST_AUTO_TEST_CASE(analysisOutputFilePostfix)
	{
		std::string defaultValue = "_stat";
		const char * arguments[] = {"foo", "foo"};
		Parameters parameters(2, arguments);

		BOOST_CHECK(defaultValue == parameters.analysisOutputFilePostfix);
	}

	BOOST_AUTO_TEST_CASE(binningMustFitDataSampleSize)
	{
		bool defaultValue = false;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().binningMustFitDataSampleSize);
	}
	
	BOOST_AUTO_TEST_CASE(adjustDataSampleSizeToBinning)
	{
		bool defaultValue = true;
		BOOST_REQUIRE_EQUAL(defaultValue, createParametersForDefaultCheck().adjustDataSampleSizeToBinning);
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

	BOOST_AUTO_TEST_CASE(numberOfBinsForAutocorrelation)
	{
		int newValue = 999;
		std::string argumentName = "--numberOfBinsForAutocorrelation";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).numberOfBinsForAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(timeMaxAutocorrelationFunction)
	{
		int newValue = 999;
		std::string argumentName = "--timeMaxAutocorrelationFunction";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).timeMaxAutocorrelationFunction);
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
	
	BOOST_AUTO_TEST_CASE(column1)
	{
		int newValue = 999;
		std::string argumentName = "--column";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).column);
	}

	BOOST_AUTO_TEST_CASE(column2)
	{
		int newValue = 999;
		std::string argumentName = "-c";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).column);
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
		bool newValue = true;
		std::string argumentName = "--doNotUseBinning";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).doNotUseBinning);
	}

	BOOST_AUTO_TEST_CASE(binning2)
	{
		bool newValue = true;
		std::string argumentName = "--doNotUseBinning";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).doNotUseBinning);
	}

	BOOST_AUTO_TEST_CASE(useNumberOfBinsForBinning_implicitByNumberOfbins)
	{
		bool newValue = true;
		int newValueBinsize = 99;
		std::string argumentName = "--numberOfBins";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValueBinsize).useNumberOfBinsForBinning);
	}

	BOOST_AUTO_TEST_CASE(useNumberOfBinsForBinning_invalidArgument1)
	{
		int newValueBinsize = 99;

		std::string argument1 = "--binsize=" + boost::lexical_cast<std::string>(newValueBinsize);
		std::string argument2 = "--numberOfBins=" + boost::lexical_cast<std::string>(newValueBinsize);
		int numberOfArguments = 4;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), argument2.c_str()};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(useNumberOfBinsForBinning_invalidArgument2)
	{
		int newValueBinsize = 99;

		std::string argument1 = "--binsize=" + boost::lexical_cast<std::string>(newValueBinsize);
		int numberOfArguments = 5;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), "--numberOfBinsMoments=2", "20"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(useNumberOfBinsForBinning_invalidArgument3)
	{
		int newValueNumberOfBins = 9;

		std::string argument1 = "--numberOfBins=" + boost::lexical_cast<std::string>(newValueNumberOfBins);
		int numberOfArguments = 5;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), "--binsizeCentralMoments=3", "500"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(binsizeMoments1)
	{
		std::string argument1 = "--binsizeMoments";
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str()};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(binsizeMoments2)
	{
		/*
		 * It seems that I have to separate the arguments to test a multitoken option!
		 *    http://stackoverflow.com/questions/2539077/boost-program-options-parsing-multiple-argument-list
		 */
		int numberOfArguments = 7;
		const char * arguments[] = {"foo", "-f dummyFile", "--binsizeMoments=1", "100", "2", "200", "3"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(binsizeMoments3)
	{
		std::vector<int> referenceValue;
		referenceValue.push_back(100);
		referenceValue.push_back(200);
		referenceValue.push_back(100);
		referenceValue.push_back(100);
		referenceValue.push_back(400);
		int numberOfArguments = 6;
		const char * arguments[] = {"foo", "-f dummyFile", "--binsizeMoments=1", "200", "4", "400"};
		BOOST_REQUIRE_NO_THROW(Parameters parameters(numberOfArguments, arguments));
		Parameters parameters(numberOfArguments, arguments);
		BOOST_REQUIRE(parameters.binsizeMoments == referenceValue);
	}

	BOOST_AUTO_TEST_CASE(binsizeCentralMoments1)
	{
		std::string argument1 = "--binsizeCentralMoments";
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str()};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(binsizeCentralMoments2)
	{
		int numberOfArguments = 7;
		const char * arguments[] = {"foo", "-f dummyFile", "--binsizeCentralMoments=1", "100", "2", "200", "3"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(binsizeCentralMoments3)
	{
		std::vector<int> referenceValue;
		referenceValue.push_back(100);
		referenceValue.push_back(200);
		referenceValue.push_back(100);
		referenceValue.push_back(100);
		referenceValue.push_back(400);
		int numberOfArguments = 6;
		const char * arguments[] = {"foo", "-f dummyFile", "--binsizeCentralMoments=1", "200", "4", "400"};
		BOOST_REQUIRE_NO_THROW(Parameters parameters(numberOfArguments, arguments));
		Parameters parameters(numberOfArguments, arguments);
		BOOST_REQUIRE(parameters.binsizeCentralMoments == referenceValue);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsMoments1)
	{
		std::string argument1 = "--numberOfBinsMoments";
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str()};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsMoments2)
	{
		int numberOfArguments = 7;
		const char * arguments[] = {"foo", "-f dummyFile", "--numberOfBinsMoments=1", "10", "2", "20", "3"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsMoments3)
	{
		std::vector<int> referenceValue;
		referenceValue.push_back(10);
		referenceValue.push_back(20);
		referenceValue.push_back(10);
		referenceValue.push_back(10);
		referenceValue.push_back(40);
		int numberOfArguments = 6;
		const char * arguments[] = {"foo", "-f dummyFile", "--numberOfBinsMoments=1", "20", "4", "40"};
		BOOST_REQUIRE_NO_THROW(Parameters parameters(numberOfArguments, arguments));
		Parameters parameters(numberOfArguments, arguments);
		BOOST_REQUIRE(parameters.numberOfBinsMoments == referenceValue);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsCentralMoments1)
	{
		std::string argument1 = "--numberOfBinsCentralMoments";
		int numberOfArguments = 3;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str()};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsCentralMoments2)
	{
		int numberOfArguments = 7;
		const char * arguments[] = {"foo", "-f dummyFile", "--numberOfBinsCentralMoments=1", "10", "2", "20", "3"};
		BOOST_REQUIRE_THROW(Parameters parameters(numberOfArguments, arguments), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(numberOfBinsCentralMoments3)
	{
		std::vector<int> referenceValue;
		referenceValue.push_back(10);
		referenceValue.push_back(20);
		referenceValue.push_back(10);
		referenceValue.push_back(10);
		referenceValue.push_back(40);
		int numberOfArguments = 6;
		const char * arguments[] = {"foo", "-f dummyFile", "--numberOfBinsCentralMoments=1", "20", "4", "40"};
		BOOST_REQUIRE_NO_THROW(Parameters parameters(numberOfArguments, arguments));
		Parameters parameters(numberOfArguments, arguments);
		BOOST_REQUIRE(parameters.numberOfBinsCentralMoments == referenceValue);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation1)
	{
		bool newValue = true;

		std::string argument1 = "--calcAutocorrelation=" + boost::lexical_cast<std::string>(newValue);
		std::string argument2 = "--timeMaxAutocorrelationFunction=10";
		int numberOfArguments = 4;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), argument2.c_str()};
		Parameters parameters(numberOfArguments, arguments);
		BOOST_CHECK_EQUAL(newValue, parameters.calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation2)
	{
		bool newValue = true;

		std::string argument1 = "--calcAutocorrelation";
		std::string argument2 = "--timeMaxAutocorrelationFunction=10";
		int numberOfArguments = 4;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), argument2.c_str()};
		Parameters parameters(numberOfArguments, arguments);
		BOOST_CHECK_EQUAL(newValue, parameters.calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(calcAutocorrelation3)
	{
		bool newValue = true;

		std::string argument1 = "-a" + boost::lexical_cast<std::string>(newValue);
		std::string argument2 = "--timeMaxAutocorrelationFunction=10";
		int numberOfArguments = 4;
		const char * arguments[] = {"foo", "-f dummyFile", argument1.c_str(), argument2.c_str()};
		Parameters parameters(numberOfArguments, arguments);
		BOOST_CHECK_EQUAL(newValue, parameters.calcAutocorrelation);
	}

	BOOST_AUTO_TEST_CASE(isMeanKnownToBeZero1)
	{
		bool newValue = true;
		std::string argumentName = "--isMeanKnownToBeZero";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).isMeanKnownToBeZero);
	}

	BOOST_AUTO_TEST_CASE(isMeanKnownToBeZero2)
	{
		bool newValue = false;
		std::string argumentName = "-m";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_shortOption(argumentName, newValue).isMeanKnownToBeZero);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean1)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeMean";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).doNotAnalyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeMean2)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeMean";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).doNotAnalyzeMean);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance1)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeVariance";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).doNotAnalyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeVariance2)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeVariance";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).doNotAnalyzeVariance);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness1)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeSkewness";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).doNotAnalyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeSkewness2)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeSkewness";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).doNotAnalyzeSkewness);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis1)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeKurtosis";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).doNotAnalyzeKurtosis);
	}

	BOOST_AUTO_TEST_CASE(analyzeKurtosis2)
	{
		bool newValue = true;
		std::string argumentName = "--doNotAnalyzeKurtosis";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).doNotAnalyzeKurtosis);
	}

	BOOST_AUTO_TEST_CASE(analysisOutputFilePrefix)
	{
		std::string newValue = "test";
		std::string argument = "--analysisOutputFilePrefix=" + newValue;
		const char * arguments[] = {"foo", "foo", argument.c_str()};
		Parameters parameters(3, arguments);

		BOOST_CHECK(newValue == parameters.analysisOutputFilePrefix);
	}

	BOOST_AUTO_TEST_CASE(analysisOutputFilePostfix)
	{
		std::string newValue = "test";
		std::string argument = "--analysisOutputFilePostfix=" + newValue;
		const char * arguments[] = {"foo", "foo", argument.c_str()};
		Parameters parameters(3, arguments);

		BOOST_CHECK(newValue == parameters.analysisOutputFilePostfix);
	}

	BOOST_AUTO_TEST_CASE(binningMustFitDataSampleSize1)
	{
		bool newValue = true;
		std::string argumentName = "--binningMustFitDataSampleSize";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).binningMustFitDataSampleSize);
	}

	BOOST_AUTO_TEST_CASE(binningMustFitDataSampleSize2)
	{
		bool newValue = true;
		std::string argumentName = "--binningMustFitDataSampleSize";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_implicitOption(argumentName).binningMustFitDataSampleSize);
	}

	BOOST_AUTO_TEST_CASE(adjustDataSampleSizeToBinning)
	{
		bool newValue = false;
		std::string argumentName = "--adjustDataSampleSizeToBinning";
		BOOST_CHECK_EQUAL(newValue, createParametersForArgumentSettingCheck_longOption(argumentName, newValue).binningMustFitDataSampleSize);
	}
BOOST_AUTO_TEST_SUITE_END()
