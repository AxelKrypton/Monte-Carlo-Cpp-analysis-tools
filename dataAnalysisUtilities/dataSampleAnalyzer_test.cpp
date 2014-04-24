// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSampleAnalyzer.hpp"

#include "TestDataSample.hpp"

//todo: add build tests

BOOST_AUTO_TEST_SUITE(zerothMoment)

	BOOST_AUTO_TEST_CASE(ZerothMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(0, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment2)
	{
		int numberOfElements = 23e2;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(0, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment3)
	{
		int numberOfElements = 24e3;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(0, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment4)
	{
		int numberOfElements = 24e4;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(firstMoment)

	BOOST_AUTO_TEST_CASE(firstMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 0.;
		TestDataSampleNthMoment tester(1, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment2)
	{
		int numberOfElements = 23;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(1, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment3)
	{
		int numberOfElements = 24;
		double referenceValue = 11.5;
		TestDataSampleNthMoment tester(1, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment4)
	{
		int numberOfElements = 24;
		double referenceValue = 0.5;
		TestDataSampleNthMoment tester(1, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment5)
	{
		std::string fileThatDoesExist = "datafile.example";
		double referenceValue = 0.56130529942755358;
		TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment6)
	{
		std::string fileThatDoesExist = "datafileWithTwoColumns.example";
		double referenceValue = 0.56130529942755358;
		TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue, 2);
	}

	BOOST_AUTO_TEST_CASE(firstMoment7)
	{
		std::string fileThatDoesExist = "datafileWithTwoColumns.example";
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(1, fileThatDoesExist, referenceValue, 1);
	}

	BOOST_AUTO_TEST_CASE(firstMoment8)
	{
		int numberOfElements = 1e6;
		double referenceValue = 499999.5;
		TestDataSampleNthMoment tester(1, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(firstMoment9, 1)
	BOOST_AUTO_TEST_CASE(firstMoment9)
	{
		int numberOfElements = roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid;
		double referenceValue = 1250000.00000125;
		TestDataSampleNthMoment tester(1, numberOfElements, bigAndSmallEntries, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondMoment)

	BOOST_AUTO_TEST_CASE(secondMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 0.;
		TestDataSampleNthMoment tester(2, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment2)
	{
		int numberOfElements = 23;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(2, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment3)
	{
		int numberOfElements = 24;
		double referenceValue = 180.166666666667;
		TestDataSampleNthMoment tester(2, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment4)
	{
		int numberOfElements = 24;
		double referenceValue = 0.340579710144927;
		TestDataSampleNthMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(thirdMoment)

	BOOST_AUTO_TEST_CASE(thirdMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 0.;
		TestDataSampleNthMoment tester(3, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment2)
	{
		int numberOfElements = 23;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(3, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment3)
	{
		int numberOfElements = 24;
		double referenceValue = 3174.;
		TestDataSampleNthMoment tester(3, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment4)
	{
		int numberOfElements = 24;
		double referenceValue = 0.260869565217391;
		TestDataSampleNthMoment tester(3, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fourthMoment)

	BOOST_AUTO_TEST_CASE(fourthMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 0.;
		TestDataSampleNthMoment tester(4, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment2)
	{
		int numberOfElements = 23;
		double referenceValue = 1.;
		TestDataSampleNthMoment tester(4, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment3)
	{
		int numberOfElements = 24;
		double referenceValue = 59635.1666666667;
		TestDataSampleNthMoment tester(4, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment4)
	{
		int numberOfElements = 24;
		double referenceValue = 0.213103750582176;
		TestDataSampleNthMoment tester(4, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(zerothCentralMoment)

	BOOST_AUTO_TEST_CASE(zerothCentralMoment1)
	{
		int numberOfElements = 1e3;
		double referenceValue = 1.;
		TestDataSampleNthCentralMoment tester(0, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(firstCentralMoment)

	BOOST_AUTO_TEST_CASE(firstCentralMoment1)
	{
		int numberOfElements = 1e3;
		double referenceValue = 0.;
		TestDataSampleNthCentralMoment tester(1, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondCentralMoment)

	BOOST_AUTO_TEST_CASE(secondCentralMoment1)
	{
		int numberOfElements = 1;
		double referenceValue = 0.;
		TestDataSampleNthCentralMoment tester(2, numberOfElements, zeros, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondCentralMoment2)
	{
		int numberOfElements = 23;
		double referenceValue = 0.;
		TestDataSampleNthCentralMoment tester(2, numberOfElements, ones, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondCentralMoment3)
	{
		int numberOfElements = 24;
		double referenceValue = 47.91666666666667;
		TestDataSampleNthCentralMoment tester(2, numberOfElements, arrayPosition, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondCentralMoment4)
	{
		int numberOfElements = 24;
		double referenceValue = 0.0905797101449274;
		TestDataSampleNthCentralMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

	//todo: perhaps add tests where the values between 0 and 1 are repeated...
	BOOST_AUTO_TEST_CASE(secondCentralMoment5)
	{
		int numberOfElements = 1e5;
		double referenceValue = 0.0833350000166668;
		TestDataSampleNthCentralMoment tester(2, numberOfElements, entriesSymmetricBetweenZeroAndOne, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthMoment)

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument1)
	{
		DataSampleAnalyzer dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument2)
	{
		DataSampleAnalyzer dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument1)
	{
		DataSampleAnalyzer dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument2)
	{
		DataSampleAnalyzer dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthCentralMoment)

	BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument1)
	{
		DataSampleAnalyzer dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument2)
	{
		DataSampleAnalyzer dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument1)
	{
		DataSampleAnalyzer dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument2)
	{
		DataSampleAnalyzer dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: add tests for 3. and 4. central moments!

BOOST_AUTO_TEST_SUITE(binning)

	BOOST_AUTO_TEST_CASE(BinningWithBinsizeAndNumberOfBins1)
	{
		//file has 21 entries, first 20 are 1, last one is zero
		int binsize = 10;
		int numberOfBins = 2;
		std::string filename = "datafile3.example";
		DataSampleAnalyzer tmp (filename);

		DataSampleAnalyzer binnedSample(tmp.createBinnedDataSampleWithBinsize(binsize));
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), 1., doublePrecisionInPercent);

		DataSampleAnalyzer binnedSample2 = tmp.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), binnedSample2.getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(BinningWithBinsizeAndNumberOfBins2)
	{
		//file has 1000 non-trivial entries
		int desiredBinsize = 100;
		int desiredNumberOfBins = 10;
		std::string filename = "datafile2.example";
		DataSampleAnalyzer tmp (filename);
		DataSampleAnalyzer binnedSample = tmp.createBinnedDataSampleWithBinsize(desiredBinsize);
		DataSampleAnalyzer binnedSample2 = tmp.createBinnedDataSampleWithNumberOfBins(desiredNumberOfBins);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), binnedSample2.getNthMoment(1), doublePrecisionInPercent);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(2), binnedSample2.getNthMoment(2), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(BinningWithBinsizeAndNumberOfBins3)
	{
		//file has 1000 non-trivial entries
		int desiredBinsize = 10;
		int desiredNumberOfBins = 100;
		std::string filename = "datafile2.example";
		DataSampleAnalyzer tmp (filename);
		DataSampleAnalyzer binnedSample = tmp.createBinnedDataSampleWithBinsize(desiredBinsize);
		DataSampleAnalyzer binnedSample2 = tmp.createBinnedDataSampleWithNumberOfBins(desiredNumberOfBins);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), binnedSample2.getNthMoment(1), doublePrecisionInPercent);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(2), binnedSample2.getNthMoment(2), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()







