// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

// double should be correct up to 15 digits (at least)
// this means two doubles should be the same up to 14 digits
double doublePrecisionInPercent = 1e-12;

#include <cfloat>

/**
 * Arrays filled with constants have a mean equal to the constant and zero variance.
 * Arrays proportional to sums over the iteration variable can be evaluated using the triangular numbers,
 * the  square pyramidal numbers or Faulhaber's formula in general.
 */
std::valarray<double> makeValarrayWithZeros(int length)
{
	return std::valarray<double>(length);
}

std::valarray<double> makeValarrayWithOnes(int length)
{
	return std::valarray<double>(1., length);
}

std::valarray<double> makeValarrayWithArrayPosition(int length)
{
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = iteration;
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithEntriesBetweenZeroAndOne(int length)
{
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = (double(iteration)) / (returnValarray.size() - 1);
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithEntriesBetweenOneAndEight(int length)
{
	if (length % 8 != 0)
		throw std::invalid_argument("length must be multiple of 8!");
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < 8; iteration ++)
	{
		for (int iteration2 = 0; iteration2 < length/8; iteration2 ++)
		{
			returnValarray[iteration + 8*iteration2] = (double(iteration+1));
		}
	}
	return returnValarray;
}

enum FillType { zeros, ones, arrayPosition, entriesSymmetricBetweenZeroAndOne, entriesBetweenOneAndEight, bigAndSmallEntries };

class TestDataSample
{
public:
  TestDataSample(int length, double referenceValue = 0., FillType fillType = zeros):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
                std::valarray<double> * testValues;
		if ( fillType == zeros )
		{
		  testValues = new std::valarray<double>(length);
		}
		if ( fillType == ones )
		  {
		    std::valarray<double> tmp = makeValarrayWithOnes(length);		    
		    testValues = new std::valarray<double>(tmp);
		  }
		if ( fillType == arrayPosition )
		  {
		    std::valarray<double> tmp = makeValarrayWithArrayPosition(length);		    
		    testValues = new std::valarray<double>(tmp);
		  }
		if ( fillType == entriesSymmetricBetweenZeroAndOne )
		  {
		    std::valarray<double> tmp = makeValarrayWithEntriesBetweenZeroAndOne(length);		    
		    testValues = new std::valarray<double>(tmp);
		  }
		actualValue = 0.;
		dataSampleInstance = new DataSample(*testValues);
		delete testValues;
	}

	TestDataSample(std::valarray<double> valarrayIn, double referenceValue):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSample(valarrayIn);
	}

	TestDataSample(std::string dataFilename, double referenceValue, int column = 1):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSample(dataFilename, column);
	}

	~TestDataSample()
	{
		testActualValueAgainstReferenceValue();
		delete dataSampleInstance;
	}

	int getNumberOfElements()
	{
		return dataSampleInstance->getNumberOfElements();
	}

protected:
	void testActualValueAgainstReferenceValue()
	{
		BOOST_CHECK_CLOSE(actualValue, referenceValue, testPrecision);
	}

	DataSample * dataSampleInstance;
	double referenceValue;
	double actualValue;
	double testPrecision;
};

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
		BOOST_REQUIRE_THROW(TestDataSample dataSampleInstance(elementsOfTestArray), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		DataSample dataSample(fileThatDoesExist);
		int numberOfElementsInDataSample = dataSample.getNumberOfElements();
		BOOST_CHECK_EQUAL(linesInFile, numberOfElementsInDataSample);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(zerothMoment)

	class TestDataSampleZerothMoment : public TestDataSample
	{
	public:
	  TestDataSampleZerothMoment(int length, double referenceValue, FillType fillType) :
	    TestDataSample(length, referenceValue, fillType)
		{
			actualValue = dataSampleInstance->getNthMoment(0);
		};
	};

	BOOST_AUTO_TEST_CASE(ZerothMoment1)
	{
	        int numberOfElements = 1;
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(numberOfElements, referenceValue, zeros);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment2)
	{
	        int numberOfElements = 23;
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(numberOfElements, referenceValue, ones);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment3)
	{
	        int numberOfElements = 24;
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(numberOfElements, referenceValue, arrayPosition);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment4)
	{
	        int numberOfElements = 24;
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(numberOfElements, referenceValue, entriesSymmetricBetweenZeroAndOne);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(firstMoment)

	class TestDataSampleFirstMoment : public TestDataSample
	{
	public:
		TestDataSampleFirstMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getNthMoment(1);
		};

		TestDataSampleFirstMoment(std::string dataFilename, double referenceValue, int column = 1) :
			TestDataSample(dataFilename, referenceValue, column)
		{
			actualValue = dataSampleInstance->getNthMoment(1);
		};
	};

	BOOST_AUTO_TEST_CASE(firstMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		TestDataSampleFirstMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		TestDataSampleFirstMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 11.5;
		TestDataSampleFirstMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.5;
		TestDataSampleFirstMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment5)
	{
		std::string fileThatDoesExist = "datafile.example";
		double referenceValue = 0.56130529942755358;
		TestDataSampleFirstMoment tester(fileThatDoesExist, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(firstMoment6)
	{
		std::string fileThatDoesExist = "datafileWithTwoColumns.example";
		double referenceValue = 0.56130529942755358;
		TestDataSampleFirstMoment tester(fileThatDoesExist, referenceValue, 2);
	}

	BOOST_AUTO_TEST_CASE(firstMoment7)
	{
		std::string fileThatDoesExist = "datafileWithTwoColumns.example";
		double referenceValue = 1.;
		TestDataSampleFirstMoment tester(fileThatDoesExist, referenceValue, 1);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondMoment)

	class TestDataSampleSecondMoment : public TestDataSample
	{
	public:
		TestDataSampleSecondMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getNthMoment(2);
		};
	};

	BOOST_AUTO_TEST_CASE(secondMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		TestDataSampleSecondMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		TestDataSampleSecondMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 180.166666666667;
		TestDataSampleSecondMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.340579710144927;
		TestDataSampleSecondMoment tester(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(thirdMoment)

	class TestDataSampleThirdMoment : public TestDataSample
	{
	public:
		TestDataSampleThirdMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getNthMoment(3);
		};
	};

	BOOST_AUTO_TEST_CASE(thirdMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		TestDataSampleThirdMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		TestDataSampleThirdMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 3174.;
		TestDataSampleThirdMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.260869565217391;
		TestDataSampleThirdMoment tester(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fourthMoment)

	class TestDataSampleFourthMoment : public TestDataSample
	{
	public:
		TestDataSampleFourthMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getNthMoment(4);
		};
	};

	BOOST_AUTO_TEST_CASE(fourthMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		TestDataSampleFourthMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		TestDataSampleFourthMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 59635.1666666667;
		TestDataSampleFourthMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.213103750582176;
		TestDataSampleFourthMoment tester(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(variance)

	class TestDataSampleVariance : public TestDataSample
	{
	public:
		TestDataSampleVariance(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getVariance();
		};
	};

	BOOST_AUTO_TEST_CASE(variance1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		TestDataSampleVariance tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 0.;
		TestDataSampleVariance tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 47.91666666666667;
		TestDataSampleVariance tester(testValues, referenceValue);
	}

	//todo: implement some tests with big numbers!
	BOOST_AUTO_TEST_CASE(variance4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.0905797101449274;
		//		double referenceValue = 0.0833425931070244;//0.0842644320297951;//0.0854700854700854;//0.0905797101449274;
		TestDataSampleVariance tester(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getNthMoment)

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument1)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		DataSample dataSampleInstance(testValues);
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument2)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		DataSample dataSampleInstance(testValues);
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument1)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		DataSample dataSampleInstance(testValues);
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument2)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		DataSample dataSampleInstance(testValues);
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

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
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	void testBinningWithBinsize(int numberOfElements, int binsize, double expectedFirstMoment)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithBinsize(binsize);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	void testBinningWithNumberOfBins_elements(int numberOfElements, int desiredNumberOfElementsOfBinnedDataSample)
	{
		int numberOfBins = numberOfElements / desiredNumberOfElementsOfBinnedDataSample;
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithNumberOfBins(desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(desiredNumberOfElementsOfBinnedDataSample, binnedSample.getNumberOfElements());
	}

	void testBinningWithBinsize_elements(int numberOfElements, int desiredBinsize)
	{
		int expectedNumberOfElementsInBinnedDataSample = numberOfElements / desiredBinsize;
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithBinsize(desiredBinsize);
		BOOST_CHECK_EQUAL(expectedNumberOfElementsInBinnedDataSample, binnedSample.getNumberOfElements());
	}

	void testBinningWithNumberOfBins_wrongArgument(int numberOfElements, int numberOfBins)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
		BOOST_REQUIRE_THROW(originalSample.createBinnedDataSampleWithNumberOfBins(numberOfBins), std::invalid_argument);
	}

	void testBinningWithBinsize_wrongArgument(int numberOfElements, int binsize)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
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
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithNumberOfBins(desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(originalSample.getNthMoment(1), binnedSample.getNthMoment(1));
		BOOST_CHECK_EQUAL(originalSample.getNthMoment(2), binnedSample.getNthMoment(2));
	}

	BOOST_AUTO_TEST_CASE(trivialBinning2)
	{
		int numberOfElements = 25;
		int desiredBinsize = 1;
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(numberOfElements);
		DataSample originalSample(testValues);
		DataSample binnedSample = originalSample.createBinnedDataSampleWithBinsize(desiredBinsize);
		BOOST_CHECK_EQUAL(originalSample.getNthMoment(2), binnedSample.getNthMoment(2));
		BOOST_CHECK_EQUAL(originalSample.getNthMoment(2), binnedSample.getNthMoment(2));
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

BOOST_AUTO_TEST_SUITE(jackknife)

	BOOST_AUTO_TEST_CASE(jackknifeElements)
	{
		int numberOfElements = 37;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), jackknifeSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(jackknifeFirstMoment)
	{
		int numberOfElements = 89;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		BOOST_REQUIRE_EQUAL(sample.getNthMoment(1), jackknifeSample.getNthMoment(1));
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
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		double expectedValue = calcExpectedValueForSecondMomentOfJackknifeEstimatorsBasedOnAnalyticExpression(sample, numberOfElements);
		BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), doublePrecisionInPercent);
	}

	double square(double in)
	{
		return in * in;
	}

	BOOST_AUTO_TEST_CASE(applyFunction1)
	{
		int numberOfElements = 53;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample sampleFromFunction = sample.applyFunction();
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample.getNthMoment(1), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(applyFunction2)
	{
		int numberOfElements = 53;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample sampleFromFunction = sample.applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample.getNthMoment(2), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance1)
	{
		DataSample sample;
		BOOST_REQUIRE_THROW(sample.getJackknifeVariance(), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance2)
	{
		DataSample sample;
		BOOST_REQUIRE_THROW(sample.createJackknifeEstimators(), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance3)
	{
		int numberOfElements = 43;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
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
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		double jackknifeVariance = jackknifeSample.getJackknifeVariance();
		double expectedValue = expectedValueForJackknifeVarianceBasedOnAnalyticExpression(sample, numberOfElements);
		BOOST_CHECK_CLOSE(jackknifeVariance, expectedValue, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(jackknifeError1)
	{
		std::string fileThatDoesExist = "datafile.example";
		int binsize = 1;
		DataSample sample(fileThatDoesExist);
		DataSample binnedSample = sample.createBinnedDataSampleWithBinsize(binsize);
		DataSample jackknifeSample = binnedSample.createJackknifeEstimators();
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
		DataSample jackknifeSample = binnedSample.createJackknifeEstimators();
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

