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
