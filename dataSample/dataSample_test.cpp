// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

double testPrecision = 10e-8;

BOOST_AUTO_TEST_CASE(build)
{
	std::valarray<double> testValues(1);

	//TODO: check difference between this allocation and x = dataSample(asdf);
	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);
}

BOOST_AUTO_TEST_CASE(elements)
{
	int elementsOfTestArray = 17;
	std::valarray<double> testValues(elementsOfTestArray);

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	int elementsOfDataSample = dataSampleInstance->getNumberOfElements();
	//todo: replace with int comparision
	BOOST_CHECK_CLOSE(float(elementsOfDataSample), float(elementsOfTestArray), testPrecision);
}

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
		dataSampleInstance = new dataSample(valarrayIn);
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

	dataSample * dataSampleInstance;
	double referenceValue;
	double actualValue;
	const static double testPrecision = 10e-8;
};

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
		dataSample dataSampleInstance(testValues);
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(highestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentValidArgument2)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		dataSample dataSampleInstance(testValues);
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_CHECK_NO_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed));
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument1)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		dataSample dataSampleInstance(testValues);
		int highestValueAllowed = dataSampleInstance.getUpperLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(highestValueAllowed + 1), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(getNthMomentInvalidArgument2)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		dataSample dataSampleInstance(testValues);
		int lowestValueAllowed = dataSampleInstance.getLowerLimitForNthMoment();
		BOOST_REQUIRE_THROW(dataSampleInstance.getNthMoment(lowestValueAllowed - 1), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(printValuesToScreen)
{
	std::valarray<double> testValues(1);
	dataSample dataSampleInstance(testValues);
	dataSampleInstance.printValuesToScreen();
}

