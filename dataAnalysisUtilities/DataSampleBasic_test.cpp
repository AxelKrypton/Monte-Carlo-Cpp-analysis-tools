// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "DataSampleBasic.hpp"

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
		BOOST_CHECK_NO_THROW(DataSampleBasic dataSampleInstance(1));
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		DataSampleBasic defaultDataSample;
		BOOST_CHECK_EQUAL(defaultDataSample.getNumberOfElements(), 1);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesNotExist = "theFileThatShouldNotBe";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(fileThatDoesNotExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_CHECK_NO_THROW( DataSampleBasic dataSample(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(fileThatDoesExist, 0), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatHasOnlyOneColumn = "datafile.example";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(fileThatHasOnlyOneColumn, 2), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatHasTwoColumns = "datafileWithTwoColumns.example";
		BOOST_CHECK_NO_THROW( DataSampleBasic dataSample(fileThatHasTwoColumns, 2));
	}

	BOOST_AUTO_TEST_CASE(build_emptyFile1)
	{
		std::string emptyFile = "emptyFile.example";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(emptyFile), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_emptyFile2)
	{
		std::string fileThatHasEmptyLine = "fileWithEmptyLine.example";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(fileThatHasEmptyLine), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_commentFile)
	{
		std::string fileThatHasOnlyComment = "fileWithComment.example";
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(fileThatHasOnlyComment), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build_invalidFile)
	{
		std::string fileThatHasInvalidLine = "fileWithBrokenLine.example";
		int validElementsInFile = 3;
		DataSampleBasic dataSample(fileThatHasInvalidLine);
		BOOST_REQUIRE_EQUAL( dataSample.getNumberOfElements(), validElementsInFile);
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int elementsOfTestArray = 17;
		DataSampleBasic dataSampleInstance(elementsOfTestArray);
		BOOST_CHECK_EQUAL(elementsOfTestArray, dataSampleInstance.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int elementsOfTestArray = 0;
		BOOST_REQUIRE_THROW(DataSampleBasic dataSampleInstance(elementsOfTestArray), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		DataSampleBasic dataSample(fileThatDoesExist);
		int numberOfElementsInDataSample = dataSample.getNumberOfElements();
		BOOST_CHECK_EQUAL(linesInFile, numberOfElementsInDataSample);
	}

	BOOST_AUTO_TEST_CASE(fileWithOffset_invalidArg)
	{
		std::string fileThatDoesExist = "datafile.example";
		int negativeOffset = -1;
		BOOST_REQUIRE_THROW(DataSampleBasic dataSample(fileThatDoesExist, 1, negativeOffset), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		int numberOfElements = 253;
		DataSampleBasic sample1(numberOfElements);
		DataSampleBasic sample2(sample1);
		BOOST_CHECK_EQUAL(sample1.getNumberOfElements(), sample2.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(offset_emptyFile1)
	{
		std::string emptyFile = "emptyFile.example";
		int tooLargeOffset = 1;
		BOOST_REQUIRE_THROW( DataSampleBasic dataSample(emptyFile, 1, tooLargeOffset), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(offset_tooLarge)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int tooLargeOffset = linesInFile;
		BOOST_REQUIRE_THROW(DataSampleBasic dataSample(fileThatDoesExist, 1, tooLargeOffset), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(offset1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int offset = 456;
		DataSampleBasic dataSample(fileThatDoesExist, 1, offset);
		BOOST_REQUIRE_EQUAL(dataSample.getNumberOfElements(), linesInFile - offset);
	}

	BOOST_AUTO_TEST_CASE(offset2)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int offset = 0;
		DataSampleBasic dataSample(fileThatDoesExist, 1, offset);
		BOOST_REQUIRE_EQUAL(dataSample.getNumberOfElements(), linesInFile);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(operatorsAndFunctions)

	BOOST_AUTO_TEST_CASE(access)
	{
		int numberOfElements = 10;
		DataSampleBasic sample(numberOfElements);
		BOOST_REQUIRE_EQUAL(sample[numberOfElements-1], 0);
	}

	BOOST_AUTO_TEST_CASE(setSpecificValue)
	{
		int numberOfElements = 10;
		double someValue = 1.23456789;
		int index = numberOfElements/2;
		DataSampleBasic sample(numberOfElements);
		sample[index] = someValue;
		BOOST_REQUIRE_EQUAL(sample[index], someValue);
	}

	BOOST_AUTO_TEST_CASE(addition_compound)
	{
		int numberOfElements = 999;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		sample += 2.;
		BOOST_CHECK_CLOSE(sample.sum(), 3.*numberOfElements, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(addition_wholeSample_compound_invalidArgument)
	{
		int numberOfElements = 532;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(sample1 += sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(addition_wholeSample_compound)
	{
		int numberOfElements = 888;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample1 += sample2;
		BOOST_CHECK_CLOSE(sample1.sum(), 2.*numberOfElements, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(addition1)
	{
		int numberOfElements = 666;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample1 + 1.;
		BOOST_CHECK_CLOSE(sample2.sum(), 2.*numberOfElements, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(addition2)
	{
		int numberOfElements = 666;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = 1. + sample1;
		BOOST_CHECK_CLOSE(sample2.sum(), 2.*numberOfElements, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(addition_wholeSample_invalidArgument)
	{
		int numberOfElements = 532;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(DataSampleBasic sample3 = sample1 + sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(addition_wholeSample)
	{
		int numberOfElements = 777;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample3 = sample1 + sample2;
		BOOST_CHECK_CLOSE(sample3.sum(), 2.*numberOfElements, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(subtraction_compound)
	{
		int numberOfElements = 999;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		sample -= 1.;
		BOOST_CHECK_CLOSE(sample.sum(), 0., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(subtraction_wholeSample_compound)
	{
		int numberOfElements = 222;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample1 -= sample2;
		BOOST_CHECK_CLOSE(sample1.sum(), 0., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(subtraction_wholeSample_compound_invalidArgument)
	{
		int numberOfElements = 123;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(sample1 -= sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(subtraction1)
	{
		int numberOfElements = 345;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample1 - 1.;
		BOOST_CHECK_CLOSE(sample2.sum(), 0., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(subtraction2)
	{
		int numberOfElements = 345;
		DataSampleBasic sample1(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sample2 = 1. - sample1;
		BOOST_CHECK_CLOSE(sample2.sum(), 1. - 0.5*(numberOfElements-2)*(numberOfElements-1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(subtraction_wholeSample_invalidArgument)
	{
		int numberOfElements = 184;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(DataSampleBasic sample3 = sample1 - sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(subtraction_wholeSample)
	{
		int numberOfElements = 777;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample3 = sample1 - sample2;
		BOOST_CHECK_CLOSE(sample3.sum(), 0., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(powerFunction_compound)
	{
		int numberOfElements = 213;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		sample ^= 2;
		BOOST_CHECK_CLOSE(sample.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(powerFunction)
	{
		int numberOfElements = 213;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared2 = sample^2;
		BOOST_CHECK_CLOSE(sampleSquared2.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(powerFunctionDouble_compound)
	{
		int numberOfElements = 213;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		sampleSquared ^= 0.5;
		BOOST_CHECK_CLOSE(sample.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(powerFunctionDouble)
	{
		int numberOfElements = 213;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		DataSampleBasic rooted = sampleSquared ^ 0.5;
		BOOST_CHECK_CLOSE(sample.sum(), rooted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower1)
	{
		int numberOfElements = 311;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic shifted = (sample - 0.)^1;
		BOOST_CHECK_CLOSE(sample.sum(), shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower2)
	{
		int numberOfElements = 311;
		double expectedValue = 311.;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic shifted = (sample - 0.)^0;
		BOOST_CHECK_CLOSE(expectedValue, shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower3)
	{
		int numberOfElements = 311;
		double expectedValue = 0.;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic shifted = ( sample -1.)^1;
		BOOST_CHECK_CLOSE(expectedValue, shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(shiftAndPower4)
	{
		int numberOfElements = 415;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic shifted = (sample - 0.)^2;
		DataSampleBasic sample2(makeValarrayWithSquaredArrayPosition(numberOfElements));
		BOOST_CHECK_CLOSE(sample2.sum(), shifted.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(mulitplication_compound)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		sample *= 0.;
		BOOST_REQUIRE_EQUAL(sample.sum(), 0.);
	}

	BOOST_AUTO_TEST_CASE(mulitplication1)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic multiplied = sample*0.;
		BOOST_REQUIRE_EQUAL(multiplied.sum(), 0.);
	}

	BOOST_AUTO_TEST_CASE(mulitplication2)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic multiplied = sample*(1./numberOfElements);
		BOOST_REQUIRE_CLOSE(multiplied.sum(), 1., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(mulitplication3)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic multiplied = (1./numberOfElements)*sample;
		BOOST_REQUIRE_CLOSE(multiplied.sum(), 1., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(mulitplication_wholeSample_compound_invalidArgument)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(DataSampleBasic multiplied = sample1 * sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(multiplication_wholeSample_compound)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample2 *= (1./numberOfElements);
		sample1 *= sample2;
		BOOST_REQUIRE_CLOSE(sample1.sum(), 1, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(multiplication_wholeSample)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample2 /= numberOfElements;
		DataSampleBasic multiplied = sample1 * sample2;
		BOOST_REQUIRE_CLOSE(multiplied.sum(), 1, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(multiplication_wholeSample_invalidArgument)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(DataSampleBasic multiplied = sample1 * sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_invalidArgument1)
	{
		int numberOfElements = 66;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(DataSampleBasic divided = sample/0., std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_invalidArgument2)
	{
		int numberOfElements = 66;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		BOOST_REQUIRE_THROW(DataSampleBasic divided = numberOfElements/sample, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_compound_invalidArgument)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(sample /= 0., std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_compound)
	{
		int numberOfElements = 432;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		sample /= numberOfElements;
		BOOST_CHECK_CLOSE(sample.sum(), 1., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(division_wholeSample_compound_invalidArgument)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(sample1 /= sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_wholeSample_compound)
	{
		int numberOfElements = 55;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample2 *= numberOfElements;
		sample1 /= sample2;
		BOOST_REQUIRE_CLOSE(sample1.sum(), 1, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(division1)
	{
		int numberOfElements = 66;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic divided = sample/ numberOfElements;
		BOOST_REQUIRE_CLOSE(divided.sum(), 1., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(division2)
	{
		int numberOfElements = 500;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		sample += 1.0;
		sample ^= 6.0;
		DataSampleBasic divided = 1.0 / sample;
		BOOST_REQUIRE_CLOSE(divided.sum(), pow(M_PI,6.)/945., doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(division_wholeSample_invalidArgument)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements + 1));
		BOOST_REQUIRE_THROW(DataSampleBasic divided = sample1 / sample2, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(division_wholeSample)
	{
		int numberOfElements = 432;
		DataSampleBasic sample1(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2(makeValarrayWithOnes(numberOfElements));
		sample2 /= (1./numberOfElements);
		DataSampleBasic divided = sample1 / sample2;
		BOOST_REQUIRE_CLOSE(divided.sum(), 1., doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(slice)

	void testSampleSliceInvalidArgument(int numberOfElements, int start, int size, int stride)
	{
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
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
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample.sampleSlice(0, numberOfElements/2, 2);
		BOOST_REQUIRE_CLOSE(sample.sum()/2., sample2.sum(), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(deleteElement)

	BOOST_AUTO_TEST_CASE(invalidArgument1)
	{
		int numberOfElements = 35;
		int elementToRemove = numberOfElements;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(sample.removeIthElement(elementToRemove), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument2)
	{
		int numberOfElements = 35;
		int elementToRemove = -1;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(sample.removeIthElement(elementToRemove), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(numberOfElements)
	{
		int numberOfElements = 41;
		int elementToRemove = 0;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample.removeIthElement(elementToRemove);
		BOOST_REQUIRE_EQUAL(sample2.getNumberOfElements(), numberOfElements - 1);
	}

	BOOST_AUTO_TEST_CASE(removeElement1)
	{
		int numberOfElements = 52;
		int elementToRemove = 0;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample.removeIthElement(elementToRemove);
		BOOST_REQUIRE_EQUAL(sample2.sum(), sample.sum() - 1);
	}

	BOOST_AUTO_TEST_CASE(removeElement2)
	{
		int numberOfElements = 65;
		int elementToRemove = 45;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample.removeIthElement(elementToRemove);
		BOOST_REQUIRE_EQUAL(sample2.sum(), sample.sum() - 1);
	}

	BOOST_AUTO_TEST_CASE(removeElement3)
	{
		int numberOfElements = 165;
		int elementToRemove = 33;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		sample[elementToRemove] *= 1000;
		DataSampleBasic sample2 = sample.removeIthElement(elementToRemove);
		BOOST_REQUIRE_EQUAL(sample2.sum(), sample.sum() - 1000);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(deleteElements)

	BOOST_AUTO_TEST_CASE(invalidArgument1)
	{
		int numberOfElements = 35;
		int numberOfElementsToRemove = numberOfElements;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(sample.removeLastNElements(numberOfElementsToRemove), std::invalid_argument);
	}
	
	BOOST_AUTO_TEST_CASE(invalidArgument2)
	{
		int numberOfElements = 35;
		int numberOfElementsToRemove = -numberOfElements;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		BOOST_REQUIRE_THROW(sample.removeLastNElements(numberOfElementsToRemove), std::invalid_argument);
	}
	
	BOOST_AUTO_TEST_CASE(removeNothing)
	{
		int numberOfElements = 37;
		int numberOfElementsToRemove = 0;
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		DataSampleBasic sample2 = sample.removeLastNElements(numberOfElementsToRemove);
		BOOST_REQUIRE_EQUAL(sample2.getNumberOfElements(), numberOfElements);
	}
	
	BOOST_AUTO_TEST_CASE(removeFive)
	{
		int numberOfElements = 37;
		int numberOfElementsToRemove = 5;
		
		DataSampleBasic sample(makeValarrayWithOnes(numberOfElements));
		for (int i = numberOfElements-1; i >= numberOfElements - numberOfElementsToRemove; i--)
		{
			sample[i] *= 1000;
		}
		
		DataSampleBasic sample2 = sample.removeLastNElements(numberOfElementsToRemove);
		BOOST_REQUIRE_EQUAL(sample2.sum(), sample.sum() - 1000 * numberOfElementsToRemove);
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
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleFromFunction = sample.applyFunction();
		BOOST_CHECK_CLOSE(sampleFromFunction.sum(), sample.sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(applyFunction2)
	{
		int numberOfElements = 53;
		DataSampleBasic sample(makeValarrayWithArrayPosition(numberOfElements));
		DataSampleBasic sampleSquared(makeValarrayWithSquaredArrayPosition(numberOfElements));
		DataSampleBasic sampleFromFunction = sample.applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.sum(), sampleSquared.sum(), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

