// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include <iostream>
#include "jackknifeEstimators.hpp"

#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(jackknifeEstimators)

	BOOST_AUTO_TEST_CASE(build)
	{
		int enoughElementsForJackknife = 57;
		DataSample sample(enoughElementsForJackknife);
		BOOST_CHECK_NO_THROW(JackknifeEstimators jackSample(sample));
	}

	BOOST_AUTO_TEST_CASE(buildInvalidArgument)
	{
		int tooFewElementsForJackknife = 1;
		DataSample sample(tooFewElementsForJackknife);
		BOOST_REQUIRE_THROW(JackknifeEstimators jackSample(sample), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements)
	{
		int numberOfElements = 37;
		DataSample sample(numberOfElements);
		JackknifeEstimators jackknifeSample(sample);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinnedDataSample)

	BOOST_AUTO_TEST_CASE(build_invalidArgument)
	{
		DataSample sample;
		BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinnedDataSample jackknifeSample(sample), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements)
	{
		int numberOfElements = 37;
		DataSample sample(numberOfElements);
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(sample);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(firstMoment1)
	{
		int numberOfElements = 89;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(*sample);
		BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(firstMoment2)
	{
		int numberOfElements = 1e3;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(*sample);
		BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), doublePrecisionInPercent);
	}

	double calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(DataSampleAnalyzer sample, int numberOfElements)
	{
		double prefactor = pow(double(numberOfElements), 2.) - 2. * numberOfElements;
		double normalization = pow(double(numberOfElements-1), 2.);
		return ( sample.getNthMoment(2) + pow(sample.getNthMoment(1),2.) * prefactor ) / normalization;
	}

	BOOST_AUTO_TEST_CASE(secondMoment1)
	{
		int numberOfElements = 45;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(*sample);
		double expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(*sample, numberOfElements);
		BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(secondMoment2)
	{
		int numberOfElements = 5e3;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(*sample);
		double expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(*sample, numberOfElements);
		BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), doublePrecisionInPercent);
	}

	double expectedValueForJackknifeVarianceBasedOnAnalyticExpression(DataSampleAnalyzer sample, int numberOfElements)
	{
		double secondMoment = sample.getNthMoment(2);
		double firstMoment = sample.getNthMoment(1);
		double prefactor = 1. / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	BOOST_AUTO_TEST_CASE(variance)
	{
		int numberOfElements = 43;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(*sample);
		double jackknifeVariance = jackknifeSample.getJackknifeVariance();
		double expectedValue = expectedValueForJackknifeVarianceBasedOnAnalyticExpression(*sample, numberOfElements);
		BOOST_CHECK_CLOSE(jackknifeVariance, expectedValue, doublePrecisionInPercent);
	}

	void checkMeanErrorWithBinsize(std::string file, int binsize, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer binnedSample = sample.createBinnedDataSampleWithBinsize(binsize);
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(binnedSample);
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, testPrecision);
	}

	void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackknifeSample(binnedSample);
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(withBinsize_meanError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 1;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 3.44121381077520906E-004;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withBinsize_meanError2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 100;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.1564370727055974e-03;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withBinsize_meanError3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int binsize = 100;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_meanError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 1005;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 3.44121381077520906E-004;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_meanError2)
	{
		std::string fileThatDoesExist = "datafile.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 10;
		double expectedValue = 1.1564370727055974e-03;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_meanError3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 10;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_meanError4)
	{
		std::string fileThatDoesExist = "datafile2.example";
		double precisionOfDataInFileInPercent = 1e-10;
		int numberOfBins = 100;
		double expectedValue = 8.02188322114928275E-004;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	void checkVarianceErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer varSample = (sample - sample.getNthMoment(1) )^2;
		DataSampleAnalyzer binnedSample = varSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackSample(binnedSample);
		BOOST_CHECK_CLOSE(jackSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_varianceError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 1005;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 4.98147373492720661E-006;

		checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(withNumberOfBins_varianceError2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 10;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.0424604327986017e-05;

		checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinningWithNumberOfBins)

	BOOST_AUTO_TEST_CASE(build_invalidArg1)
	{
		int enoughElementsForJackknife = 57;
		int numberOfBinsTooLowForJackknife = 1;
		DataSample sample(enoughElementsForJackknife);
		BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithNumberOfBins jackSample(sample, numberOfBinsTooLowForJackknife), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build_invalidArg2)
	{
		int enoughElementsForJackknife = 57;
		int numberOfBinsTooHighForJackknife = enoughElementsForJackknife + 1;
		DataSample sample(enoughElementsForJackknife);
		BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithNumberOfBins jackSample(sample, numberOfBinsTooHighForJackknife), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(firstMoment1)
	{
		int numberOfElements = 1e3;
		int numberOfBins = 1e3;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample(*sample, numberOfBins);
		BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance)
	{
		int numberOfElements = 43;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		JackknifeEstimators jackknifeSample(*sample);
		JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample2(*sample, numberOfElements);
		double jackknifeVariance1 = jackknifeSample.getJackknifeVariance();
		double jackknifeVariance2 = jackknifeSample2.getJackknifeVariance() * (numberOfElements -1 ) * (numberOfElements -1 );
		BOOST_CHECK_CLOSE(jackknifeVariance1, jackknifeVariance2, doublePrecisionInPercent);
	}

	void checkMeanErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSample sample(file);
		JackknifeEstimatorsFromBinningWithNumberOfBins jackknifeSample(sample, numberOfBins);
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(meanError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 1005;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 3.44121381077520906E-004;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(meanError2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 10;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.1564370727055974e-03;
		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(meanError3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int numberOfBins = 10;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, precisionOfDataInFileInPercent);
	}

	void checkVarianceErrorWithNumberOfBins(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer varSample = (sample - sample.getNthMoment(1))^2;
		JackknifeEstimatorsFromBinningWithNumberOfBins jackSample(varSample, numberOfBins);
		BOOST_CHECK_CLOSE(jackSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(varianceError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 10;
		double expectedValue = 1.0424604327986017e-05;

		checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(varianceError2)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int numberOfBins = 10;
		double expectedValue = 1.19735074528675533E-005;

		checkVarianceErrorWithNumberOfBins(fileThatDoesExist, numberOfBins, expectedValue, doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsFromBinningWithBinsize)

	BOOST_AUTO_TEST_CASE(build_invalidArg1)
	{
		int enoughElementsForJackknife = 57;
		int binsizeTooLowForJackknife = 0;
		DataSample sample(enoughElementsForJackknife);
		BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeTooLowForJackknife), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build_invalidArg2)
	{
		int enoughElementsForJackknife = 57;
		int binsizeTooHighForJackknife = enoughElementsForJackknife;
		DataSample sample(enoughElementsForJackknife);
		BOOST_REQUIRE_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeTooHighForJackknife), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build)
	{
		int enoughElementsForJackknife = 57;
		int binsizeThatAlwaysWorks = 1;
		DataSample sample(enoughElementsForJackknife);
		BOOST_CHECK_NO_THROW(JackknifeEstimatorsFromBinningWithBinsize jackSample(sample, binsizeThatAlwaysWorks));
	}

	BOOST_AUTO_TEST_CASE(firstMoment1)
	{
		int numberOfElements = 1e3;
		int binsize = 1;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		JackknifeEstimatorsFromBinningWithBinsize jackknifeSample(*sample, binsize);
		BOOST_REQUIRE_CLOSE(sample->getNthMoment(1), jackknifeSample.getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance)
	{
		int numberOfElements = 43;
		int binsize = 1;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		JackknifeEstimators jackknifeSample(*sample);
		JackknifeEstimatorsFromBinningWithBinsize jackknifeSample2(*sample, binsize);
		double jackknifeVariance1 = jackknifeSample.getJackknifeVariance();
		double jackknifeVariance2 = jackknifeSample2.getJackknifeVariance() * (numberOfElements -1 ) * (numberOfElements -1 );
		BOOST_CHECK_CLOSE(jackknifeVariance1, jackknifeVariance2, doublePrecisionInPercent);
	}

	void checkMeanErrorWithBinsize(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSample sample(file);
		JackknifeEstimatorsFromBinningWithBinsize jackknifeSample(sample, numberOfBins);
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(meanError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 1;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 3.44121381077520906E-004;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(meanError2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 100;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.1564370727055974e-03;
		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	BOOST_AUTO_TEST_CASE(meanError3)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int binsize = 100;
		double precisionOfDataInFileInPercent = 1e-10;
		double expectedValue = 1.14688734781786292E-003;

		checkMeanErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, precisionOfDataInFileInPercent);
	}

	void checkVarianceErrorWithBinsize(std::string file, int numberOfBins, double expectedValue, double testPrecision)
	{
		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer varSample = ( sample - sample.getNthMoment(1))^2;
		JackknifeEstimatorsFromBinningWithBinsize jackSample(varSample, numberOfBins);
		BOOST_CHECK_CLOSE(jackSample.getJackknifeError(), expectedValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(varianceError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 100;
		double expectedValue = 1.0424604327986017e-05;

		checkVarianceErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(varianceError2)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int binsize = 100;
		double expectedValue = 1.19735074528675533E-005;

		checkVarianceErrorWithBinsize(fileThatDoesExist, binsize, expectedValue, doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(jackknifeEstimatorsExplicitAgainstCombinedMethod)

	BOOST_AUTO_TEST_CASE(jackknifeEstimatorsFromBinningWithNumberOfBins_varianceError2)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int numberOfBins = 50;

		DataSampleAnalyzer sample(fileThatDoesExist);
		DataSampleAnalyzer varSample = ( sample - sample.getNthMoment(1))^2;
		JackknifeEstimatorsFromBinningWithNumberOfBins jackSample1(varSample, numberOfBins);
		DataSample binnedSample = varSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample);

		double varianceError1 = jackSample1.getJackknifeError();
		double varianceError2 = jackSample2.getJackknifeError();

		BOOST_CHECK_CLOSE(varianceError1, varianceError2, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeEstimatorsFromBinningWithBinsize_varianceError2)
	{
		std::string fileThatDoesExist = "datafile2.example";
		int binsize = 20;

		DataSampleAnalyzer sample(fileThatDoesExist);
		DataSampleAnalyzer varSample = ( sample - sample.getNthMoment(1))^2;
		JackknifeEstimatorsFromBinningWithBinsize jackSample1(varSample, binsize);
		DataSample binnedSample = varSample.createBinnedDataSampleWithBinsize(binsize);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample);

		double varianceError1 = jackSample1.getJackknifeError();
		double varianceError2 = jackSample2.getJackknifeError();

		BOOST_CHECK_CLOSE(varianceError1, varianceError2, doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

