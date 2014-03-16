// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

double testPrecision = 10e-8;

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

class TestDataSample
{
public:
	TestDataSample(std::valarray<double> valarrayIn, double referenceValue):
		referenceValue(referenceValue)
	{
		//todo: need delete here?
		dataSampleInstance = new DataSample(valarrayIn);
	}

	~TestDataSample()
	{
		testActualValueAgainstReferenceValue();
	}
protected:
	void testActualValueAgainstReferenceValue()
	{
		BOOST_CHECK_CLOSE(actualValue, referenceValue, testPrecision);
	}

	DataSample * dataSampleInstance;
	double referenceValue;
	double actualValue;
	const static double testPrecision = 10e-8;
};

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		std::valarray<double> testValues(1);

		//TODO: check difference between this allocation and x = dataSample(asdf);
		DataSample * dataSampleInstance;
		dataSampleInstance = new DataSample(testValues);
		BOOST_REQUIRE(dataSampleInstance);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		DataSample defaultDataSample;
		BOOST_CHECK_EQUAL(defaultDataSample.getNumberOfElements(), 1);
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int elementsOfTestArray = 17;
		std::valarray<double> testValues(elementsOfTestArray);

		DataSample * dataSampleInstance;
		dataSampleInstance = new DataSample(testValues);
		int elementsOfDataSample = dataSampleInstance->getNumberOfElements();
		BOOST_CHECK_EQUAL(elementsOfTestArray, elementsOfDataSample);
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int elementsOfTestArray = 0;
		std::valarray<double> testValues(elementsOfTestArray);
		BOOST_REQUIRE_THROW(DataSample dataSampleInstance(testValues), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(zerothMoment)

	class TestDataSampleZerothMoment : public TestDataSample
	{
	public:
		TestDataSampleZerothMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
		{
			actualValue = dataSampleInstance->getNthMoment(0);
		};
	};

	BOOST_AUTO_TEST_CASE(ZerothMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(ZerothMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 1.;
		TestDataSampleZerothMoment tester(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: make add. tests for mean? This is the same as first moment...
BOOST_AUTO_TEST_SUITE(firstMoment)

	class TestDataSampleFirstMoment : public TestDataSample
	{
	public:
		TestDataSampleFirstMoment(std::valarray<double> valarrayIn, double referenceValue) :
			TestDataSample(valarrayIn, referenceValue)
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
		double referenceValue = 47.9166666667;
		TestDataSampleVariance tester(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.0905797101449;
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
		BOOST_CHECK_CLOSE(expectedValue, jackknifeSample.getNthMoment(2), testPrecision);
	}

	double square(double in)
	{
		return in * in;
	}

	BOOST_AUTO_TEST_CASE(applyFunction)
	{
		int numberOfElements = 53;
		std::valarray<double> testValues = makeValarrayWithArrayPosition(numberOfElements);
		DataSample sample(testValues);
		DataSample sampleFromFunction = sample.applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample.getNthMoment(2), testPrecision);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance1)
	{
		DataSample sample;
		BOOST_REQUIRE_THROW(sample.getJackknifeVariance(), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(jackknifeVariance2)
	{
		DataSample sample;
		DataSample jackknifeSample = sample.createJackknifeEstimators();
		BOOST_CHECK_NO_THROW(jackknifeSample.getJackknifeVariance());
	}

BOOST_AUTO_TEST_SUITE_END()

