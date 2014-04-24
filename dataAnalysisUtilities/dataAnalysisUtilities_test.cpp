// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataAnalysisUtilities.hpp"

#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(meanAndError)

	static void testMeanAndError(DataSampleAnalyzer * sample, double expectedMean, double expectedError)
	{
		MeanAndError meanAndError = calcMeanAndErrorOfDataSample(*sample);
		BOOST_CHECK_CLOSE(meanAndError.mean, expectedMean, doublePrecisionInPercent);
		BOOST_CHECK_CLOSE(meanAndError.error, expectedError, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 795;
		DataSampleAnalyzer sample(numberOfElements);

		double expectedMean = 0.;
		double expectedError = 0.;

		testMeanAndError(&sample, expectedMean, expectedError);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 1542;
		TestDataSample testSample(numberOfElements, ones);
		DataSampleAnalyzer* sample = testSample.getDataSample();

		double expectedMean = 1.;
		double expectedError = 0.;

		testMeanAndError(sample, expectedMean, expectedError);
	}

	double expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSampleAnalyzer sample, int numberOfElements)
	{
		double secondMoment = sample.getNthMoment(2);
		double firstMoment = sample.getNthMoment(1);
		double prefactor = 1. / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	BOOST_AUTO_TEST_CASE(test3)
	{
		int numberOfElements = 795;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();

		double expectedError = sqrt( expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements) );
		double expectedMean = 0.5  * (numberOfElements - 1);

		testMeanAndError(sample, expectedMean, expectedError);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromBinsize)

	static void checkMeanErrorWithBinsize(std::string file, int binsize, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		MeanAndError meanAndError = calcMeanAndErrorOfDataSampleWithBinningFromBinsize(sample, binsize);
		BOOST_CHECK_CLOSE(meanAndError.error, expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(error1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 1;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedError = 3.44121381077520906E-004;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 100;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedError = 1.1564370727055974e-03;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int binsize = 100;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromNumberOfBins)

	static void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		MeanAndError meanAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, numberOfBins);
		BOOST_CHECK_CLOSE(meanAndError.error, expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(error1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 1005;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 3.44121381077520906E-004;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error2)
	{
		std::string fileThatDoesExist = "datafile.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 10;
		double expectedValue = 1.1564370727055974e-03;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 10;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error4)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 100;
		double expectedValue = 8.02188322114928275E-004;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

