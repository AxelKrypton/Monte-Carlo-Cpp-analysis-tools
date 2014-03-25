// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

#include <cfloat>
#include "dataSampleTestUtilities.hpp"

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

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_CHECK_NO_THROW(DataSample dataSampleInstance(1));
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
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatHasOnlyOneColumn, 2), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatHasTwoColumns = "datafileWithTwoColumns.example";
		BOOST_CHECK_NO_THROW( DataSample dataSample(fileThatHasTwoColumns, 2));
	}

	BOOST_AUTO_TEST_CASE(build_emptyFile1)
	{
		std::string emptyFile = "emptyFile.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(emptyFile), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_emptyFile2)
	{
		std::string fileThatHasEmptyLine = "fileWithEmptyLine.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatHasEmptyLine), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_commentFile)
	{
		std::string fileThatHasOnlyComment = "fileWithComment.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatHasOnlyComment), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_invalidFile)
	{
		std::string fileThatHasInvalidLine = "fileWithBrokenLine.example";
		int validElementsInFile = 3;
		DataSample dataSample(fileThatHasInvalidLine);
		BOOST_REQUIRE_EQUAL( dataSample.getNumberOfElements(), validElementsInFile);
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int elementsOfTestArray = 17;
		DataSample dataSampleInstance(elementsOfTestArray);
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

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		int numberOfElements = 253;
		DataSample sample1(numberOfElements);
		DataSample sample2(sample1);
		BOOST_CHECK_EQUAL(sample1.getNumberOfElements(), sample2.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(offset_emptyFile1)
	{
		std::string emptyFile = "emptyFile.example";
		int tooLargeOffset = 1;
		BOOST_REQUIRE_THROW( DataSample dataSample(emptyFile, 1, tooLargeOffset), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(offset_tooLarge)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int tooLargeOffset = linesInFile;
		BOOST_REQUIRE_THROW(DataSample dataSample(fileThatDoesExist, 1, tooLargeOffset), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(offset1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int offset = 456;
		DataSample dataSample(fileThatDoesExist, 1, offset);
		BOOST_REQUIRE_EQUAL(dataSample.getNumberOfElements(), linesInFile - offset);
	}

	BOOST_AUTO_TEST_CASE(offset2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int offset = 0;
		DataSample dataSample(fileThatDoesExist, 1, offset);
		BOOST_REQUIRE_EQUAL(dataSample.getNumberOfElements(), linesInFile);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(operatorsAndFunctions)

	BOOST_AUTO_TEST_CASE(access)
	{
		int numberOfElements = 10;
		DataSample sample(numberOfElements);
		BOOST_REQUIRE_EQUAL(sample[numberOfElements-1], 0);
	}

	BOOST_AUTO_TEST_CASE(setSpecificValue)
	{
		int numberOfElements = 10;
		double someValue = 1.23456789;
		int index = numberOfElements/2;
		DataSample sample(numberOfElements);
		sample[index] = someValue;
		BOOST_REQUIRE_EQUAL(sample[index], someValue);
	}

	BOOST_AUTO_TEST_CASE(powerFunction)
	{
		int numberOfElements = 213;
		DataSample sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSample sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		DataSample sampleSquared2 = sample.pow(2);
		BOOST_CHECK_CLOSE(sampleSquared2.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower1)
	{
		int numberOfElements = 311;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample shifted = sample.shiftAndPow(1, 0.);
		BOOST_CHECK_CLOSE(sample.sum(), shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower2)
	{
		int numberOfElements = 311;
		double expectedValue = 311.;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample shifted = sample.shiftAndPow(0, 0.);
		BOOST_CHECK_CLOSE(expectedValue, shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower3)
	{
		int numberOfElements = 311;
		double expectedValue = 0.;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample shifted = sample.shiftAndPow(1, 1.);
		BOOST_CHECK_CLOSE(expectedValue, shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower4)
	{
		int numberOfElements = 415;
		DataSample sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSample shifted = sample.shiftAndPow(2, 0.);
		DataSample sample2(makeValarrayWithSquaredArrayPosition(numberOfElements));
		BOOST_CHECK_CLOSE(sample2.sum(), shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shift1)
	{
		int numberOfElements = 333;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample shifted = sample.shift(0.);
		BOOST_REQUIRE_EQUAL(shifted.sum(), numberOfElements);
	}

	BOOST_AUTO_TEST_CASE(shift2)
	{
		int numberOfElements = 333;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample shifted = sample.shift(1.);
		BOOST_REQUIRE_EQUAL(shifted.sum(), 0.);
	}

	BOOST_AUTO_TEST_CASE(mulitply1)
	{
		int numberOfElements = 432;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample multiplied = sample*0.;
		BOOST_REQUIRE_EQUAL(multiplied.sum(), 0.);
	}

	BOOST_AUTO_TEST_CASE(mulitply2)
	{
		int numberOfElements = 432;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample multiplied = sample*(1./numberOfElements);
		BOOST_REQUIRE_CLOSE(multiplied.sum(), 1., doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(slice)

	void testSampleSliceInvalidArgument(int numberOfElements, int start, int size, int stride)
	{
		DataSample sample(makeValarrayWithArrayPosition(numberOfElements));
		BOOST_REQUIRE_THROW(sample.sampleSlice(start, size, stride), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_1)
	{
		int numberOfElements = 111;
		testSampleSliceInvalidArgument(numberOfElements, -1, numberOfElements, 1);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_2)
	{
		int numberOfElements = 64;
		testSampleSliceInvalidArgument(numberOfElements, numberOfElements, numberOfElements, 1);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_3)
	{
		int numberOfElements = 53;
		testSampleSliceInvalidArgument(numberOfElements, 0, 0, 1);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_4)
	{
		int numberOfElements = 66;
		testSampleSliceInvalidArgument(numberOfElements, 0, numberOfElements+1, 1);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_5)
	{
		int numberOfElements = 93;
		testSampleSliceInvalidArgument(numberOfElements, 0, numberOfElements, 0);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_6)
	{
		int numberOfElements = 77;
		testSampleSliceInvalidArgument(numberOfElements, 0, numberOfElements, numberOfElements);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_7)
	{
		int numberOfElements = 88;
		testSampleSliceInvalidArgument(numberOfElements, 0, numberOfElements, 2);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_8)
	{
		int numberOfElements = 55;
		testSampleSliceInvalidArgument(numberOfElements, 0, 1, numberOfElements);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument_9)
	{
		int numberOfElements = 66;
		testSampleSliceInvalidArgument(numberOfElements, 1, numberOfElements, 1);
	}

	BOOST_AUTO_TEST_CASE(slice)
	{
		int numberOfElements = 66;
		DataSample sample(makeValarrayWithOnes(numberOfElements));
		DataSample sample2 = sample.sampleSlice(0, numberOfElements/2, 2);
		BOOST_REQUIRE_CLOSE(sample.sum()/2., sample2.sum(), doublePrecisionInPercent);
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
		DataSample sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSample sampleFromFunction = sample.applyFunction();
		BOOST_CHECK_CLOSE(sampleFromFunction.sum(), sample.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(applyFunction2)
	{
		int numberOfElements = 53;
		DataSample sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSample sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		DataSample sampleFromFunction = sample.applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

