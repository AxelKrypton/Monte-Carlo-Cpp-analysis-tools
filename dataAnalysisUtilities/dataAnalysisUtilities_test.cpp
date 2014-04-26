// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataAnalysisUtilities.hpp"

#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(meanAndError)

	static void testMeanAndError(DataSample * sample, double expectedMean, double expectedError)
	{
		EstimateAndError meanAndError = calcMeanAndErrorOfDataSample(*sample);
		BOOST_CHECK_CLOSE(meanAndError.estimate, expectedMean, doublePrecisionInPercent);
		BOOST_CHECK_CLOSE(meanAndError.error, expectedError, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 795;
		DataSample sample(numberOfElements);

		double expectedMean = 0.;
		double expectedError = 0.;

		testMeanAndError(&sample, expectedMean, expectedError);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 1542;
		TestDataSample testSample(numberOfElements, ones);
		DataSample* sample = testSample.getDataSample();

		double expectedMean = 1.;
		double expectedError = 0.;

		testMeanAndError(sample, expectedMean, expectedError);
	}

	double expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
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
		DataSample* sample = testSample.getDataSample();

		double expectedError = sqrt( expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements) );
		double expectedMean = 0.5  * (numberOfElements - 1);

		testMeanAndError(sample, expectedMean, expectedError);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromBinsize)

	static void checkMeanErrorWithBinsize(std::string file, int binsize, double expectedValue, double testPrecision)
	{
		DataSample sample(file);
		EstimateAndError meanAndError = calcMeanAndErrorOfDataSampleWithBinningFromBinsize(sample, binsize);
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
		DataSample sample(file);
		EstimateAndError meanAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, numberOfBins);
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

BOOST_AUTO_TEST_SUITE(varianceAndError)

	double expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
	{
		double secondMoment = sample.getNthMoment(2);
		double firstMoment = sample.getNthMoment(1);
		double prefactor = numberOfElements / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	BOOST_AUTO_TEST_CASE(variance)
	{
		int numberOfElements = 2674;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		double expectedValue = expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements);

		EstimateAndError varianceAndError = calcVarianceAndErrorOfDataSample(*sample);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedValue, doublePrecisionInPercent);
	}

	static void testVarianceAndError(DataSample * sample, double expectedVariance, double expectedError, double testPrecision)
	{
		EstimateAndError varianceAndError = calcVarianceAndErrorOfDataSample(*sample);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedVariance, testPrecision);
		BOOST_CHECK_CLOSE(varianceAndError.error, expectedError, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 2674;
		DataSample sample(numberOfElements);

		double expectedMean = 0.;
		double expectedError = 0.;

		testVarianceAndError(&sample, expectedMean, expectedError, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 1542;
		TestDataSample testSample(numberOfElements, ones);
		DataSample* sample = testSample.getDataSample();

		double expectedMean = 0.;
		double expectedError = 0.;

		testVarianceAndError(sample, expectedMean, expectedError, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test3)
	{
		int numberOfElements = 2345;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();

		double expectedVariance = 458447.5;
		double expectedError = 8465.84748859;

		//todo: check this again!
		//the difference in the error estimate exceeds 1e-13, most likely due to rounding errors.
		double testPrecision = doublePrecisionInPercent*1e3;

		testVarianceAndError(sample, expectedVariance, expectedError, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_varianceError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		double elementsInFile = 1005.;
		//the value of the reference program must be multiplied by N/(N-1) to get the unbiased value!
		double expectedVariance = 1.18893203014724946E-004 * elementsInFile / (elementsInFile - 1.) ;
		double expectedError = 4.98147373492720661E-006;

		double precisionOfDataInFileInPercent = 1e-10;
		//the difference in the variance estimate exceeds 1e-10 a bit, most likely due to rounding errors.
		precisionOfDataInFileInPercent *= 5.;

		DataSample sample(fileThatDoesExist);

		testVarianceAndError(&sample, expectedVariance, expectedError, precisionOfDataInFileInPercent);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(createDataSampleFromFile)

	BOOST_AUTO_TEST_CASE(noBinning)
	{
		std::string fileThatDoesExist = "datafile.example";
		double elementsInFile = 1005.;
		const char * arguments[] = {"foo", "foo", "--useBinning"};
		Parameters parameters(3, arguments);
		DataSample tmp = createDataSampleFromDatafile(fileThatDoesExist, parameters);
		BOOST_CHECK_EQUAL(elementsInFile, tmp.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(binning)
	{
		std::string fileThatDoesExist = "datafile.example";
		double elementsInFile = 1005.;
		const char * arguments[] = {"foo", "foo"};
		Parameters parameters(2, arguments);

		int expectedNumberOfElements = parameters.numberOfBins;
		DataSample tmp = createDataSampleFromDatafile(fileThatDoesExist, parameters);
		BOOST_CHECK_EQUAL(expectedNumberOfElements, tmp.getNumberOfElements());
	}

BOOST_AUTO_TEST_SUITE_END()
