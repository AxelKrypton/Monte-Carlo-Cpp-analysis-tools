// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataAnalysisUtilities.hpp"
#include "TestDataSample.hpp"
#include "binning.hpp"
#include "binnedDataSample.hpp"
#include <iomanip>

static void checkEstimateAndError(EstimateAndError expectedEstimateAndError, EstimateAndError calculatedEstimateAndError, realFloat testPrecision)
{
	BOOST_CHECK_CLOSE(expectedEstimateAndError.error, calculatedEstimateAndError.error, testPrecision);
	BOOST_CHECK_CLOSE(expectedEstimateAndError.estimate, calculatedEstimateAndError.estimate, testPrecision);
}

BOOST_AUTO_TEST_SUITE(meanAndError)

	static void testMeanAndError(DataSample * sample, EstimateAndError expectedMeanAndError)
	{
		if(expectedMeanAndError.estimate == 0.0){
			EstimateAndError meanAndError_zeroMean = calcMeanAndErrorOfUncorrelatedDataSample(*sample, true);
			//Trivial test, but better than nothing
			checkEstimateAndError(expectedMeanAndError, meanAndError_zeroMean, realFloatPrecisionInPercent);
		}
		//Leave in any case this test, more significant
		EstimateAndError meanAndError = calcMeanAndErrorOfUncorrelatedDataSample(*sample);
		checkEstimateAndError(expectedMeanAndError, meanAndError, realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 795;
		DataSample sample(numberOfElements);

		EstimateAndError expectedMeanAndError(0,0);

		testMeanAndError(&sample, expectedMeanAndError);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 1542;
		TestDataSample<realFloat> testSample(numberOfElements, ones);
		DataSample* sample = testSample.getDataSample();

		EstimateAndError expectedMeanAndError(1,0);

		testMeanAndError(sample, expectedMeanAndError);
	}

	static realFloat expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
	{
		realFloat secondMoment = sample.getNthMoment(2);
		realFloat firstMoment = sample.getNthMoment(1);
		realFloat prefactor = 1. / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	BOOST_AUTO_TEST_CASE(test3)
	{
		int numberOfElements = 795;
		TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();

		EstimateAndError expectedMeanAndError;
		expectedMeanAndError.estimate = 0.5  * (numberOfElements - 1);
		expectedMeanAndError.error = sqrt( expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements) );

		testMeanAndError(sample, expectedMeanAndError);
	}

	BOOST_AUTO_TEST_CASE(test4)
	{
		int numberOfElements = 4;
		TestDataSample<realFloat> testSample(numberOfElements, onesMinusOnes);
		DataSample* sample = testSample.getDataSample();

		EstimateAndError expectedMeanAndError;
		expectedMeanAndError.estimate = 0.;
		expectedMeanAndError.error = sqrt( 1. /(numberOfElements-1) );

		testMeanAndError(sample, expectedMeanAndError);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromBinsize)

	static Parameters createParameters(int binsize, std::string file = "noFileGiven")
	{
		std::string argumentFile = "--file=" + file;
		std::string argumentBinsize = "--binsize=" + boost::lexical_cast<std::string>(binsize);
		const char * arguments[] = {"foo", argumentFile.c_str(), argumentBinsize.c_str()};
		Parameters parameters(3, arguments);
		
		return parameters;
	}

	static void checkMeanAndErrorWithBinsize(std::string file, int binsize, EstimateAndError expected, realFloat testPrecision)
	{
		Parameters parameters = createParameters(binsize, file);

		DataSample sample(file);
		DataSample binnedData = performBinningFromBinsize(sample, binsize);

		if(expected.estimate == 0.0){
			EstimateAndError meanAndError_zeroMean = calcMeanAndErrorOfUncorrelatedDataSample(binnedData, true);
			//Trivial test, but better than nothing
			checkEstimateAndError(expected, meanAndError_zeroMean, testPrecision);
		}
		//Leave in any case this test, more significant
		EstimateAndError meanAndError = calcMeanAndErrorOfUncorrelatedDataSample(binnedData);
		checkEstimateAndError(expected, meanAndError, testPrecision);
	}

	//This file has 1005 entries, from which 5 are discarded when binning with binsize 100
	std::string fileThatDoesExist = "datafile.example";
	realFloat precisionOfDataInFileInPercent = 1e-10;
	
	BOOST_AUTO_TEST_CASE(error1)
	{
		int binsize = 1;
		realFloat expectedMean = 5.61305299427553583e-01;
		realFloat expectedError = 3.44121381077520906E-004;
		
		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanAndErrorWithBinsize(fileThatDoesExist, binsize, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error2)
	{
		int binsize = 100;

		realFloat expectedMean = 0.56125906512982415;
		realFloat expectedError = 1.1564370727055974e-03;

		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanAndErrorWithBinsize(fileThatDoesExist, binsize, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error3)
	{
		int binsize = 100;
		
		DataSample sample(fileThatDoesExist);
		DataSample binnedData = performBinningFromBinsize(sample, binsize);
		
		BOOST_CHECK_CLOSE(sample.getNthMoment(1), binnedData.getNthMoment(1),precisionOfDataInFileInPercent );
	}
	
	BOOST_AUTO_TEST_CASE(error4)
	{
		int binsize = 100;
		Parameters parameters = createParameters(binsize);
		
		DataSample sample(fileThatDoesExist);
		DataSample binnedData = performBinningFromBinsize(sample, binsize);
		
		EstimateAndError meanAndErrorFromBinnedDataSample = calcMeanAndErrorOfUncorrelatedDataSample(binnedData);
		EstimateAndError meanAndErrorFromDataSample = calcMeanAndErrorOfDataSample(sample, parameters);
		
		checkEstimateAndError(meanAndErrorFromBinnedDataSample, meanAndErrorFromDataSample, precisionOfDataInFileInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(meanAndErrorWithBinningFromNumberOfBins)

	//This file has 1005 entries, from which 5 are discarded when binning with number of bins 10
	std::string fileThatDoesExist = "datafile.example";
	realFloat precisionOfDataInFileInPercent = 1e-10;
	int numberOfBins = 10;

	static Parameters createParameters(int numberOfBins, std::string file = "noFileGiven")
	{
		std::string argumentFile = "--file=" + file;
		std::string argumentBinsize = "--numberOfBins=" + boost::lexical_cast<std::string>(numberOfBins);
		const char * arguments[] = {"foo", argumentFile.c_str(), argumentBinsize.c_str()};
		Parameters parameters(3, arguments);
		
		return parameters;
	}
	
	static void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, EstimateAndError expected, realFloat testPrecision)
	{
		Parameters parameters = createParameters(numberOfBins, file);

		DataSample sample(file);
		DataSample binnedData = performBinningFromNumberOfBins(sample, numberOfBins);

		if(expected.estimate == 0.0){
			EstimateAndError meanAndError_zeroMean = calcMeanAndErrorOfUncorrelatedDataSample(binnedData, true);
			//Trivial test, but better than nothing
			checkEstimateAndError(expected, meanAndError_zeroMean, testPrecision);
		}
		//Leave in any case this test, more significant
		EstimateAndError meanAndError = calcMeanAndErrorOfDataSample(sample, parameters);
		checkEstimateAndError(expected, meanAndError, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(error1)
	{
		int numberOfBins = 1005;
		
		realFloat expectedMean = 5.61305299427553583e-01;
		realFloat expectedError = 3.44121381077520906E-004;

		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error2)
	{
		realFloat expectedMean = 0.56125906512982415;
		realFloat expectedError = 1.1564370727055974e-03;

		EstimateAndError expectedEstimateAndError(expectedMean, expectedError);
		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedEstimateAndError, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(error3)
	{
		DataSample sample(fileThatDoesExist);
		DataSample binnedData = performBinningFromNumberOfBins(sample, numberOfBins);
		
		BOOST_CHECK_CLOSE(sample.getNthMoment(1), binnedData.getNthMoment(1), precisionOfDataInFileInPercent );
	}
	
	BOOST_AUTO_TEST_CASE(error4)
	{
		Parameters parameters = createParameters(numberOfBins);
		
		DataSample sample(fileThatDoesExist);
		DataSample binnedData = performBinningFromNumberOfBins(sample, numberOfBins);
		
		EstimateAndError meanAndErrorFromBinnedDataSample = calcMeanAndErrorOfUncorrelatedDataSample(binnedData);
		EstimateAndError meanAndErrorFromDataSample = calcMeanAndErrorOfDataSample(sample, parameters);
		
		checkEstimateAndError(meanAndErrorFromBinnedDataSample, meanAndErrorFromDataSample, precisionOfDataInFileInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(varianceAndError)

	realFloat expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
	{
		realFloat secondMoment = sample.getNthMoment(2);
		realFloat firstMoment = sample.getNthMoment(1);
		realFloat prefactor = numberOfElements / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	static void testVarianceAndError(DataSample * sample, EstimateAndError expected, realFloat testPrecision, bool isMeanKnownToBeZero = false)
	{
		if(isMeanKnownToBeZero){
			EstimateAndError varianceAndError_zeroMean = calcVarianceAndErrorOfUncorrelatedDataSample(*sample, true);
			//Trivial test, but better than nothing
			checkEstimateAndError(expected, varianceAndError_zeroMean, testPrecision);
		}
		//Leave in any case this test, more significant
		EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
		checkEstimateAndError(expected, varianceAndError, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(variance)
	{
		int numberOfElements = 2674;
		TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		realFloat expectedValue = expectedValueForUnbiasedVarianceBasedOnAnalyticExpression(*sample, numberOfElements);

		EstimateAndError varianceAndError = calcVarianceAndErrorOfUncorrelatedDataSample(*sample);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedValue, realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 2674;
		DataSample sample(numberOfElements);

		realFloat expectedVariance = 0.;
		realFloat expectedError = 0.;
		
		EstimateAndError expected(expectedVariance, expectedError);
		testVarianceAndError(&sample, expected, realFloatPrecisionInPercent, true);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 1542;
		TestDataSample<realFloat> testSample(numberOfElements, ones);
		DataSample* sample = testSample.getDataSample();

		realFloat expectedVariance = 0.;
		realFloat expectedError = 0.;

		EstimateAndError expected(expectedVariance, expectedError);
		testVarianceAndError(sample, expected, realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test3)
	{
		int numberOfElements = 2345;
		TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();

		realFloat expectedVariance = 458447.5;
		realFloat expectedError = 8465.84748859;

		//todo: check this again!
		//the difference in the error estimate exceeds 1e-13, most likely due to rounding errors.
		realFloat testPrecision = realFloatPrecisionInPercent*1e3;

		EstimateAndError expected(expectedVariance, expectedError);
		testVarianceAndError(sample, expected, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(test4)
	{
		int numberOfElements = 2742;
		TestDataSample<realFloat> testSample(numberOfElements, onesMinusOnes);
		DataSample* sample = testSample.getDataSample();

		realFloat expectedVariance = numberOfElements/(numberOfElements-1.);
		realFloat expectedError = 0;

		EstimateAndError expected(expectedVariance, expectedError);
		testVarianceAndError(sample, expected, realFloatPrecisionInPercent, true);
	}

	BOOST_AUTO_TEST_CASE(withBinning)
	{
		std::string gaussianData = "gaussianNumbers_0_1_0_3.dat";
		
		realFloat expectedVariance = 1.;
		realFloat expectedError = 5e-3;

		realFloat expectedPrecisionInPercent = 1;

		const char * arguments[] = {"foo", "--binsize=1000", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError varianceAndError = calcVarianceAndErrorOfDataSample(sample, parameters);
		BOOST_CHECK_CLOSE(varianceAndError.estimate, expectedVariance, expectedPrecisionInPercent);
		BOOST_CHECK_SMALL(varianceAndError.error, expectedError);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(skewnessAndError)

	BOOST_AUTO_TEST_CASE(withBinning1)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";
		
		realFloat expectedSkewness = 1.;

		const char * arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError skewnessAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue =
                fabs(skewnessAndError.estimate - expectedSkewness) / skewnessAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN( fabs(skewnessAndError.error/skewnessAndError.estimate) < 0.001 );
	}
	
	BOOST_AUTO_TEST_CASE(withBinning2)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";

		realFloat expectedSkewness = 1.;

		const char * arguments[] = {"foo", "--isMeanKnownToBeZero", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(4, arguments);

		DataSample sample(gaussianData);

		EstimateAndError skewnessAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue =
                fabs(skewnessAndError.estimate - expectedSkewness) / skewnessAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN( fabs(skewnessAndError.error/skewnessAndError.estimate) < 0.001 );
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(binderAndError)

BOOST_AUTO_TEST_CASE(withBinning1)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";
		
		realFloat expectedKurtosis = 3.;

		const char * arguments[] = {"foo", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(3, arguments);
	
		DataSample sample(gaussianData);

		EstimateAndError kurtosisAndError = calcBinderAndErrorOfDataSample(sample, parameters);
        realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue =
                fabs(kurtosisAndError.estimate - expectedKurtosis) / kurtosisAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN( fabs(kurtosisAndError.error/kurtosisAndError.estimate) < 0.001 );
	}

	BOOST_AUTO_TEST_CASE(withBinning2)
	{
		std::string gaussianData = "gaussianNumbers_0_1_1_3.dat";

		realFloat expectedKurtosis = 3.;

		const char * arguments[] = {"foo", "--isMeanKnownToBeZero", "--binsize=100", gaussianData.c_str()};
		Parameters parameters(4, arguments);

		DataSample sample(gaussianData);

		EstimateAndError kurtosisAndError = calcBinderAndErrorOfDataSample(sample, parameters);
		realFloat NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue =
                fabs(kurtosisAndError.estimate - expectedKurtosis) / kurtosisAndError.error;

        BOOST_REQUIRE(NumberOfSigmaAtWhichTheResultIsCompatibleWithExpectedValue < 3.0);
        BOOST_WARN( fabs(kurtosisAndError.error/kurtosisAndError.estimate) < 0.001 );
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
		 * from how we do. This is the reason why here we do not use "realFloatPrecisionInPercent"
		 * but only 3.e-10 in the boost check of the error.
		 */
		for(unsigned int i=0; i<result.size(); i++){
            BOOST_REQUIRE_CLOSE(result[i].estimate, referenceCorrFuncValues[i], 3.e-10);
            BOOST_REQUIRE_CLOSE(result[i].error, referenceCorrFuncErrors[i], 3.e-10);
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
		 * of "realFloatPrecisionInPercent" in the boost check.
		 */
		for(unsigned int i=0; i<result.size(); i++){
			BOOST_CHECK_CLOSE(result[i].estimate, referenceTauValues[i], 1.e-10);
			BOOST_CHECK_CLOSE(result[i].error, referenceTauErrors[i], 1.e-10);
		}
	}

BOOST_AUTO_TEST_SUITE_END()



