// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

/**
 * double should be correct up to 15 digits (at least)
 * this means two doubles should be the same relative
 * to each other up to 14 digits
 * */
double doublePrecisionInPercent = 1e-12;

#include <cfloat>
#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(precision)

	BOOST_AUTO_TEST_CASE(doublePrecision1)
	{
		int numberOfValidDigitsOnSystem = DBL_DIG; //from <cfloat>
		int desiredNumberOfValidDigits = 15;
		BOOST_REQUIRE_EQUAL(numberOfValidDigitsOnSystem,desiredNumberOfValidDigits );
	}

	BOOST_AUTO_TEST_CASE(doublePrecision2)
	{
		double numberCorrectUpTo15thDigit        = 1234567890123450e-15;
		double anotherNumberCorrectUpTo15thDigit = 1234567890123452e-15;
		BOOST_CHECK_CLOSE(numberCorrectUpTo15thDigit, anotherNumberCorrectUpTo15thDigit, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(doublePrecision3)
	{
		double numberCorrectUpTo15thDigit        = 123456789012345.0;
		double anotherNumberCorrectUpTo15thDigit = 123456789012345.2;
		BOOST_CHECK_CLOSE(numberCorrectUpTo15thDigit, anotherNumberCorrectUpTo15thDigit, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(doublePrecision4)
	{
		double twoThird = 2./3.;
		double oneHalf = .5;
		double oneThird = twoThird * oneHalf;
		//                 digits:    1234567890123456
		double oneThirdTo15thDigit = 3333333333333339e-16;
		BOOST_CHECK_CLOSE(oneThird, oneThirdTo15thDigit, doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: if files are found in subdir can depend on the system!
BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_CHECK_NO_THROW(TestDataSample dataSampleInstance(1));
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		DataSample defaultDataSample;
		BOOST_CHECK_EQUAL(defaultDataSample.getNumberOfElements(), 1);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesNotExist = "theFileThatShouldNotBe";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatDoesNotExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_CHECK_NO_THROW( DataSample dataSample(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatDoesExist, 0), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatHasOnlyOneColumn = "datafile.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatHasOnlyOneColumn, 2), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatHasTwoColumns = "datafileWithTwoColumns.example";
		BOOST_CHECK_NO_THROW( DataSample dataSample(fileThatHasTwoColumns, 2));
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int elementsOfTestArray = 17;
		TestDataSample dataSampleInstance = TestDataSample(elementsOfTestArray);
		BOOST_CHECK_EQUAL(elementsOfTestArray, dataSampleInstance.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int elementsOfTestArray = 0;
		BOOST_REQUIRE_THROW(DataSample dataSampleInstance(elementsOfTestArray), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		DataSample dataSample(fileThatDoesExist);
		int numberOfElementsInDataSample = dataSample.getNumberOfElements();
		BOOST_CHECK_EQUAL(linesInFile, numberOfElementsInDataSample);
	}

	BOOST_AUTO_TEST_CASE(fileWithOffset_invalidArg)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int negativeOffset = -1;
		BOOST_REQUIRE_THROW(DataSample dataSample(fileThatDoesExist, 1, negativeOffset), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(fileWithOffset_notYetImplemented)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int validOffset = 1;
		BOOST_REQUIRE_THROW(DataSample dataSample(fileThatDoesExist, 1, validOffset), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

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
		double referenceValue = 0.;
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
		DataSample dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument2)
	{
		DataSample dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument1)
	{
		DataSample dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument2)
	{
		DataSample dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthCentralMoment)

	BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument1)
	{
		DataSample dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentValidArgument2)
	{
		DataSample dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument1)
	{
		DataSample dataSampleInstance;
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthCentralMomentInvalidArgument2)
	{
		DataSample dataSampleInstance;
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthCentralMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: add tests for 3. and 4. central moments!

BOOST_AUTO_TEST_SUITE(binning)

	//todo: add test to check warning in case number of bins is not a multiple of number of elements?

	/**
	 * "Real binning tests":
	 * With the used samples ("makeValarrayWithArrayPosition") the
	 * binning is very simple. The expected first moment is equal to
	 * the one with from a sample with a reduced number of elements,
	 * leaving out the number of elements that "fall" away during binning.
	 */

	void testBinningWithNumberOfBins(int numberOfElements, int numberOfBins, double expectedFirstMoment)
	{
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* originalSample = testSample.getDataSample();
		DataSample binnedSample = originalSample->createBinnedDataSampleWithNumberOfBins(numberOfBins);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	void testBinningWithBinsize(int numberOfElements, int binsize, double expectedFirstMoment)
	{
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* originalSample = testSample.getDataSample();
		DataSample binnedSample = originalSample->createBinnedDataSampleWithBinsize(binsize);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	void testBinningWithNumberOfBins_elements(int numberOfElements, int desiredNumberOfElementsOfBinnedDataSample)
	{
		int numberOfBins = numberOfElements / desiredNumberOfElementsOfBinnedDataSample;
		DataSample originalSample(numberOfElements);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithNumberOfBins(desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(desiredNumberOfElementsOfBinnedDataSample, binnedSample.getNumberOfElements());
	}

	void testBinningWithBinsize_elements(int numberOfElements, int desiredBinsize)
	{
		int expectedNumberOfElementsInBinnedDataSample = numberOfElements / desiredBinsize;
		DataSample originalSample(numberOfElements);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithBinsize(desiredBinsize);
		BOOST_CHECK_EQUAL(expectedNumberOfElementsInBinnedDataSample, binnedSample.getNumberOfElements());
	}

	void testBinningWithNumberOfBins_wrongArgument(int numberOfElements, int numberOfBins)
	{
		DataSample originalSample(numberOfElements);
		BOOST_REQUIRE_THROW(originalSample.createBinnedDataSampleWithNumberOfBins(numberOfBins), std::invalid_argument);
	}

	void testBinningWithBinsize_wrongArgument(int numberOfElements, int binsize)
	{
		DataSample originalSample(numberOfElements);
		BOOST_REQUIRE_THROW(originalSample.createBinnedDataSampleWithBinsize(binsize), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument1)
	{
		int numberOfElements = 27;
		int numberOfBins = 0;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument2)
	{
		int numberOfElements = 33;
		int negativeNumber = - numberOfElements;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, negativeNumber);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument3)
	{
		int numberOfElements = 13;
		int numberThatIsTooBig = numberOfElements + 1;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, numberThatIsTooBig);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument4)
	{
		int numberOfElements = 19;
		int numberOfBins = 0;
		testBinningWithBinsize_wrongArgument(numberOfElements, numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument5)
	{
		int numberOfElements = 41;
		int numberThatIsTooBig = numberOfElements + 1;
		testBinningWithBinsize_wrongArgument(numberOfElements, numberThatIsTooBig);
	}

	BOOST_AUTO_TEST_CASE(binningWrongArgument6)
	{
		int numberOfElements = 47;
		int negativeNumber = - numberOfElements;
		testBinningWithBinsize_wrongArgument(numberOfElements, negativeNumber);
	}

	BOOST_AUTO_TEST_CASE(binningWithNumberOfBins1)
	{
		int numberOfElements = 27;
		int desiredNumberOfElementsOfBinnedDataSample = numberOfElements;
		testBinningWithNumberOfBins_elements(numberOfElements, desiredNumberOfElementsOfBinnedDataSample);
	}

	BOOST_AUTO_TEST_CASE(binningWithNumberOfBins2)
	{
		int numberOfElements = 23;
		int desiredNumberOfElementsOfBinnedDataSample = 1;
		testBinningWithNumberOfBins_elements(numberOfElements, desiredNumberOfElementsOfBinnedDataSample);
	}

	BOOST_AUTO_TEST_CASE(binningWithNumberOfBins3)
	{
		int numberOfElements = 25;
		int desiredNumberOfElementsOfBinnedDataSample = numberOfElements/2.;
		testBinningWithNumberOfBins_elements(numberOfElements, desiredNumberOfElementsOfBinnedDataSample);
	}

	BOOST_AUTO_TEST_CASE(binningWithBinsize1)
	{
		int numberOfElements = 25;
		int desiredBinsize = numberOfElements;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

	BOOST_AUTO_TEST_CASE(binningWithBinsize2)
	{
		int numberOfElements = 25;
		int desiredBinsize = 1;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

	BOOST_AUTO_TEST_CASE(binningWithBinsize3)
	{
		int numberOfElements = 25;
		int desiredBinsize = 3;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

	BOOST_AUTO_TEST_CASE(trivialBinning1)
	{
		int numberOfElements = 27;
		int desiredNumberOfElementsOfBinnedDataSample = numberOfElements;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSample* originalSample = testSample.getDataSample();
		DataSample binnedSample = originalSample->createBinnedDataSampleWithNumberOfBins(desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(1), binnedSample.getNthMoment(1));
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
	}

	BOOST_AUTO_TEST_CASE(trivialBinning2)
	{
		int numberOfElements = 25;
		int desiredBinsize = 1;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSample* originalSample = testSample.getDataSample();
		DataSample binnedSample = originalSample->createBinnedDataSampleWithBinsize(desiredBinsize);
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
	}

	BOOST_AUTO_TEST_CASE(realBinningWithNumberOfBins1)
	{
		int evenNumberOfElements = 24;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(evenNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithNumberOfBins2)
	{
		int oddNumberOfElements = 25;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(oddNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithNumberOfBins3)
	{
		int evenNumberOfElements = 26;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 11;
		double expectedFirstMoment = 10.5;
		testBinningWithNumberOfBins(evenNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithNumberOfBins4)
	{
		int oddNumberOfElements = 29;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 9;
		double expectedFirstMoment = 13.;
		testBinningWithNumberOfBins(oddNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithBinsize1)
	{
		int evenNumberOfElements = 24;
		int evenBinsize = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithBinsize(evenNumberOfElements, evenBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithBinsize2)
	{
		int oddNumberOfElements = 21;
		int evenBinsize = 12;
		double expectedFirstMoment = 5.5;
		testBinningWithBinsize(oddNumberOfElements, evenBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithBinsize3)
	{
		int evenNumberOfElements = 34;
		int oddBinsize = 3;
		double expectedFirstMoment = 16.;
		testBinningWithBinsize(evenNumberOfElements, oddBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinningWithBinsize4)
	{
		int oddNumberOfElements = 47;
		int oddBinsize = 19;
		double expectedFirstMoment = 18.5;
		testBinningWithBinsize(oddNumberOfElements, oddBinsize, expectedFirstMoment);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(applyFunction)

	double square(double in)
	{
		return in * in;
	}

	BOOST_AUTO_TEST_CASE(applyFunction1)
	{
		int numberOfElements = 53;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample sampleFromFunction = sample->applyFunction();
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample->getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(applyFunction2)
	{
		int numberOfElements = 53;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample sampleFromFunction = sample->applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample->getNthMoment(2), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: add tests checking for isJackknifeSample!
BOOST_AUTO_TEST_SUITE(jackknife)

	BOOST_AUTO_TEST_CASE(jackknifeBuild2)
	{
		int numberOfElements = 10;
		DataSample sample(numberOfElements);
		JackknifeDataSample jackSample(sample);
		BOOST_CHECK_EQUAL(numberOfElements, jackSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(jackknifeElements)
	{
		int numberOfElements = 37;
		DataSample sample(numberOfElements);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(jackknifeFirstMoment)
	{
		int numberOfElements = 89;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample jackknifeSample = sample->createJackknifeEstimators();
		BOOST_REQUIRE_EQUAL(sample->getNthMoment(1), jackknifeSample.getNthMoment(1));
	}

	double calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
	{
		double prefactor = pow(double(numberOfElements), 2.) - 2. * numberOfElements;
		double normalization = pow(double(numberOfElements-1), 2.);
		return ( sample.getNthMoment(2) + pow(sample.getNthMoment(1),2.) * prefactor ) / normalization;
	}

	BOOST_AUTO_TEST_CASE(jackknifeSecondMoment)
	{
		int numberOfElements = 45;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample jackknifeSample = sample->createJackknifeEstimators();
		double expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(*sample, numberOfElements);
		BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance2)
	{
		int tooFewElementsForJackknife = 1;
		DataSample sample(tooFewElementsForJackknife);
		BOOST_REQUIRE_THROW(sample.createJackknifeEstimators(), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance3)
	{
		int enoughElementsForJackknife = 43;
		DataSample sample(enoughElementsForJackknife);
		DataSample jackknifeSampleTmp = sample.createJackknifeEstimators();
		JackknifeDataSample jackknifeSample(jackknifeSampleTmp);
		BOOST_CHECK_NO_THROW(jackknifeSample.getJackknifeVariance());
	}

	double expectedValueForJackknifeVarianceBasedOnAnalyticExpression(DataSample sample, int numberOfElements)
	{
		double secondMoment = sample.getNthMoment(2);
		double firstMoment = sample.getNthMoment(1);
		double prefactor = 1. / (numberOfElements - 1.);
		return prefactor * ( secondMoment - pow(firstMoment, 2.) );
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance4)
	{
		int numberOfElements = 43;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample jackknifeSampleTmp = sample->createJackknifeEstimators();
		JackknifeDataSample jackknifeSample(jackknifeSampleTmp);
		double jackknifeVariance = jackknifeSample.getJackknifeVariance();
		double expectedValue = expectedValueForJackknifeVarianceBasedOnAnalyticExpression(*sample, numberOfElements);
		BOOST_CHECK_CLOSE(jackknifeVariance, expectedValue, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 1;
		DataSample sample(fileThatDoesExist);
		DataSample binnedSample = sample.createBinnedDataSampleWithBinsize(binsize);
		DataSample jackknifeSampleTmp = binnedSample.createJackknifeEstimators();
		JackknifeDataSample jackknifeSample(jackknifeSampleTmp);
		double expectedValue = 3.44121381077520906E-004;
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, doublePrecisionInPercent);
	}

//	BOOST_AUTO_TEST_CASE(jackknifeError2)
//	{
//		std::string fileThatDoesExist = "datafile.example";
//		int binsize = 100;
//		DataSample sample(fileThatDoesExist);
//		DataSample binnedSample = sample.createBinnedDataSampleWithBinsize(binsize);
//		std::cout.precision(32);
//		std::cout << binnedSample.getVariance() << std::endl;
//		DataSample jackknifeSample = binnedSample.createJackknifeEstimators();
//		double expectedValue = 1.15015003710525849E-003;
//		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, doublePrecisionInPercent);
//	}

	BOOST_AUTO_TEST_CASE(jackknifeError3)
	{
		std::string fileThatDoesExist = "datafile.example";
		int numberOfBins = 1005;
		DataSample sample(fileThatDoesExist);
		DataSample binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSample jackknifeSampleTmp = binnedSample.createJackknifeEstimators();
		JackknifeDataSample jackknifeSample(jackknifeSampleTmp);
		double expectedValue = 3.44121381077520906E-004;
		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, doublePrecisionInPercent);
	}

//	BOOST_AUTO_TEST_CASE(jackknifeError4)
//	{
//		std::string fileThatDoesExist = "datafile.example";
//		int numberOfBins = 10;
//		DataSample sample(fileThatDoesExist);
//		DataSample binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
//		DataSample jackknifeSample = sample.createJackknifeEstimators();
//		double expectedValue = 0.41152744587655254;//1.15015003710525849E-003;
//		BOOST_CHECK_CLOSE(jackknifeSample.getJackknifeError(), expectedValue, testPrecision);
//	}

	//todo: fix this test!
//	BOOST_AUTO_TEST_CASE(jackknifeVarianceError1)
//	{
//		int numberOfElements = 40;
//		std::valarray<double> tmp = makeValarrayWithEntriesBetweenOneAndEight(numberOfElements);
//		DataSample sample(tmp);
//		DataSample binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfElements);
//		std::cout.precision(20);
//		std::cout << binnedSample.getVariance() << std::endl;
//		DataSample jackknifeSample = binnedSample.createJackknifeEstimators();
//
//		std::cout << "v1:\t"<<jackknifeSample.getJackknifeError() << std::endl;
//		std::cout << "v2:\t"<<sqrt(jackknifeSample.getJackknifeVariance_v2()) << std::endl;
//		std::cout << "anl.\t" << expectedValueForJackknifeVarianceBasedOnAnalyticExpression(sample, numberOfElements) << std::endl;
//		float d1 = 39.*(1./39./39. * (25.5 + 4.5*4.5  * ( 40.*40. - 2.*40.  ) ) - 4.5*4.5) ;
//		double d2 = 39.*(1./39./39. * (25.5 + 4.5*4.5  * ( 40.*40. - 2.*40.  ) ) - 4.5*4.5) ;
//		long double d3 = 39.*(1./39./39. * (25.5 + 4.5*4.5  * ( 40.*40. - 2.*40.  ) ) - 4.5*4.5) ;
//		std::cout << "an2:\t"<< d1 << std::endl;
//		std::cout << "an2:\t"<< d2 << std::endl;
//		std::cout << "an2:\t"<< d3  << std::endl;
//
//		double expectedValue = sqrt(expectedValueForJackknifeVarianceBasedOnAnalyticExpression(sample, numberOfElements));
//		double v1 = jackknifeSample.getJackknifeError();
//		double v2 = sqrt(jackknifeSample.getJackknifeVariance_v2());
//
//		pbit(d2);
//		pbit(expectedValue);
//		pbit(v1);
//		pbit(v2);
//
//		BOOST_CHECK_CLOSE(v1, expectedValue, 1e-14);
//		BOOST_CHECK_CLOSE(v2, expectedValue, 1e-14);
//	}


BOOST_AUTO_TEST_SUITE_END()

