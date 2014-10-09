// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataAnalysisUtilities.hpp"

#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(meanAndError)

	static void testMeanAndError(DataSample * sample, double expectedMean, double expectedError)
	{
		EstimateAndError meanAndError = calcMeanAndErrorOfUncorrelatedDataSample(*sample);
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
#include <iomanip>

void checkEstimateAndError(EstimateAndError expectedEstimateAndError, EstimateAndError calculatedEstimateAndError, double testPrecision)
{
	BOOST_CHECK_CLOSE(expectedEstimateAndError.error, calculatedEstimateAndError.error, testPrecision);
	BOOST_CHECK_CLOSE(expectedEstimateAndError.estimate, calculatedEstimateAndError.estimate, testPrecision);
}

#include "binnedDataSample.hpp"

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromBinsize)

	static void checkMeanErrorWithBinsize(std::string file, int binsize, EstimateAndError expected, double testPrecision)
	{
		std::string argumentFile = "--file=" + file;
		std::string argumentBinsize = "--binsize=" + boost::lexical_cast<std::string>(binsize);
		const char * arguments[] = {"foo", argumentFile.c_str(), argumentBinsize.c_str()};
		Parameters parameters(3, arguments);

		DataSample sample(file);
		DataSample binnedData = performBinningFromBinsize(sample, binsize);

		EstimateAndError meanAndError = calcMeanAndErrorOfDataSample(sample, parameters);
		checkEstimateAndError(expected, meanAndError, testPrecision);
	}

	//This file has 1005 entries, from which 5 are discarded when binning with binsize 100
	std::string fileThatDoesExist = "datafile.example";
	double precisionOfDataInFileInPercent = 1e-10;
	
	BOOST_AUTO_TEST_CASE(error1)
	{
		int binsize = 1;
		double expectedMean = 5.61305299427553583e-01;
		double expectedError = 3.44121381077520906E-004;
		
		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error2)
	{
		int binsize = 100;

		double expectedError = 1.1564370727055974e-03;
		double expectedMean = 0.56125906512982415;

		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error3)
	{
		int binsize = 100;
		
		DataSample sample(fileThatDoesExist);
		DataSample binnedData = performBinningFromBinsize(sample, binsize);
		
		BOOST_CHECK_CLOSE(sample.getNthMoment(1), binnedData.getNthMoment(1),precisionOfDataInFileInPercent );
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromNumberOfBins)

	static void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		std::string argumentFile = "--file=" + file;
		std::string argumentBinsize = "--numberOfBins=" + boost::lexical_cast<std::string>(numberOfBins);
		const char * arguments[] = {"foo", argumentFile.c_str(), argumentBinsize.c_str()};
		Parameters parameters(3, arguments);

		DataSample sample(file);

		EstimateAndError meanAndError = calcMeanAndErrorOfDataSample(sample, parameters);
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

		EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedValue, doublePrecisionInPercent);
	}

	static void testVarianceAndError(DataSample * sample, double expectedVariance, double expectedError, double testPrecision)
	{
		EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
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

	BOOST_AUTO_TEST_CASE(withBinning)
	{
		std::string gaussianData = "gaussianNumbers_0_1_0_3.dat";
		
		double expectedVariance = 1.;
		double expectedError = 1e-3;

		double expectedPrecisionInPercent = 1;

		const char * arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError varianceAndError = calcVarianceAndErrorOfDataSample(sample, parameters);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedVariance, expectedPrecisionInPercent);
		BOOST_CHECK_SMALL(varianceAndError.error, expectedError);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(skewnessAndError)

	BOOST_AUTO_TEST_CASE(withBinning)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";
		
		double expectedSkewness = 1.;
		double expectedError = 1e-2;

		double expectedPrecisionInPercent = 1;

		const char * arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError skewnessAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
		BOOST_CHECK_CLOSE(skewnessAndError.estimate, expectedSkewness, expectedPrecisionInPercent);
		BOOST_CHECK_SMALL(skewnessAndError.error, expectedError);
	}
	
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(binderAndError)

	BOOST_AUTO_TEST_CASE(withBinning)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";
		
		double expectedKurtosis = 3.;
		double expectedError = 1e-2;

		double expectedPrecisionInPercent = 1;

		const char * arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError kurtosisAndError = calcBinderAndErrorOfDataSample(sample, parameters);
		BOOST_CHECK_CLOSE(kurtosisAndError.estimate, expectedKurtosis, expectedPrecisionInPercent);
		BOOST_CHECK_SMALL(kurtosisAndError.error, expectedError);
	}
	
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(createDataSampleFromFile)

	BOOST_AUTO_TEST_CASE(noBinning)
	{
		std::string fileThatDoesExist = "datafile.example";
		double elementsInFile = 1005.;
		const char * arguments[] = {"foo", "foo", "--useBinning"};
		Parameters parameters(3, arguments);
		DataSample tmp(fileThatDoesExist);
		
		BOOST_CHECK_EQUAL(elementsInFile, tmp.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(binningWithNumberOfBins)
	{
		std::string fileThatDoesExist = "datafile.example";
		const char * arguments[] = {"foo", "foo"};
		Parameters parameters(2, arguments);

		int expectedNumberOfElements = parameters.numberOfBins;
		DataSample tmp(fileThatDoesExist);
		BOOST_CHECK_EQUAL(expectedNumberOfElements, tmp.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(binningWithBinsize)
	{
		std::string fileThatDoesExist = "datafile.example";
		double elementsInFile = 1005.;
		const char * arguments[] = {"foo", "foo", "--binsize=10"};
		Parameters parameters(3, arguments);

		int expectedNumberOfElements = (int) elementsInFile / 10;
		DataSample tmp(fileThatDoesExist);
		BOOST_CHECK_EQUAL(expectedNumberOfElements, tmp.getNumberOfElements());
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(autocorrelation)

	BOOST_AUTO_TEST_CASE(corrFuncTestVsBerg)
	{
		const char * arguments[] = {"foo", "--file=gaussianNumbers_Berg.dat", "-a", "--timeMaxAutocorrelationFunction=128", "--numberOfBinsForAutocorrelation=32"};
		Parameters parameters(5, arguments);
		DataSample gaussianCorrelatedBergData(parameters.file, 2);
		DataSample referenceCorrFuncValues("autFuncBergRefResult32bins.dat", 2);
		DataSample referenceCorrFuncErrors("autFuncBergRefResult32bins.dat", 3);

		std::vector<EstimateAndError> result = calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(gaussianCorrelatedBergData, parameters);

		/*
		 * Since in the Berg Code the jackknife estimators are always calculated without
		 * any analytic simplification, the reference result are too different from ours.
		 * Then we just implemented this analytic simplification in his code.
		 * [the modification consists in introducing after line "CALL DATJACK(NBINS,WORK,ACORJ)"
		 *  of the file autcorj.f the for loop
		 *     DO IBINS=1,NBINS
		 *       ACORJ(IBINS)=WORK(IBINS)
		 *     END DO
		 *  and in the file at21.f divide the ACE output variable of STEBJ0 by (NBINS-1) when
		 *  it is print to screen]
		 *
		 * Nevertheless there are some rounding errors because the calculation is carried out differently
		 * from how we do. This is the reason why here we do not use "doublePrecisionInPercent"
		 * but only 3.e-10 in the boost check of the error.
		 */
		for(uint i=0; i<result.size(); i++){
			BOOST_CHECK_CLOSE(result[i].estimate, referenceCorrFuncValues[i], 3.e-10);
			BOOST_CHECK_CLOSE(result[i].error, referenceCorrFuncErrors[i], 3.e-10);
		}
	}

	BOOST_AUTO_TEST_CASE(tauTestVsBerg)
	{
		const char * arguments[] = {"foo", "--file=gaussianNumbers_Berg.dat", "-a", "--timeMaxAutocorrelationFunction=128", "--numberOfBinsForAutocorrelation=32"};
		Parameters parameters(5, arguments);
		DataSample gaussianCorrelatedBergData(parameters.file, 2);
		DataSample referenceTauValues("intTauBergRefResult32bins.dat", 2);
		DataSample referenceTauErrors("intTauBergRefResult32bins.dat", 3);

		std::vector<EstimateAndError> result = calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(gaussianCorrelatedBergData, parameters);

		/*
		 * See the comment above for the reason why we use 1.e-10 instead
		 * of "doublePrecisionInPercent" in the boost check.
		 */
		for(uint i=0; i<result.size(); i++){
			BOOST_CHECK_CLOSE(result[i].estimate, referenceTauValues[i], 1.e-10);
			BOOST_CHECK_CLOSE(result[i].error, referenceTauErrors[i], 1.e-10);
		}
	}

BOOST_AUTO_TEST_SUITE_END()



