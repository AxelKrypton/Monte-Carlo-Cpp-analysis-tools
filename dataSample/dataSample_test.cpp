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

//todo: make add. tests for first moment? This is the same as mean...
BOOST_AUTO_TEST_SUITE(mean)

	void checkDataSampleMean(std::valarray<double> valarrayIn, double referenceValue)
	{
		dataSample * dataSampleInstance;
		dataSampleInstance = new dataSample(valarrayIn);
		BOOST_REQUIRE(dataSampleInstance);
		BOOST_CHECK_CLOSE(dataSampleInstance->getMean(), referenceValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(mean1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		checkDataSampleMean(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(mean2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		checkDataSampleMean(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(mean3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 11.5;
		checkDataSampleMean(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(mean4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.5;
		checkDataSampleMean(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(secondMoment)

	void checkDataSampleSecondMoment(std::valarray<double> valarrayIn, double referenceValue)
	{
		dataSample * dataSampleInstance;
		dataSampleInstance = new dataSample(valarrayIn);
		BOOST_REQUIRE(dataSampleInstance);
		BOOST_CHECK_CLOSE(dataSampleInstance->getSecondMoment(), referenceValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(secondMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		checkDataSampleSecondMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		checkDataSampleSecondMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 180.166666666667;
		checkDataSampleSecondMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(secondMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.340579710144927;
		checkDataSampleSecondMoment(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(thirdMoment)

	void checkDataSampleThirdMoment(std::valarray<double> valarrayIn, double referenceValue)
	{
		dataSample * dataSampleInstance;
		dataSampleInstance = new dataSample(valarrayIn);
		BOOST_REQUIRE(dataSampleInstance);
		BOOST_CHECK_CLOSE(dataSampleInstance->getThirdMoment(), referenceValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		checkDataSampleThirdMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		checkDataSampleThirdMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 3174.;
		checkDataSampleThirdMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(thirdMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.260869565217391;
		checkDataSampleThirdMoment(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fourthMoment)

	void checkDataSampleFourthMoment(std::valarray<double> valarrayIn, double referenceValue)
	{
		dataSample * dataSampleInstance;
		dataSampleInstance = new dataSample(valarrayIn);
		BOOST_REQUIRE(dataSampleInstance);
		BOOST_CHECK_CLOSE(dataSampleInstance->getFourthMoment(), referenceValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		checkDataSampleFourthMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 1.;
		checkDataSampleFourthMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 59635.1666666667;
		checkDataSampleFourthMoment(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(fourthMoment4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.213103750582176;
		checkDataSampleFourthMoment(testValues, referenceValue);
	}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(variance)

	void checkDataSampleVariance(std::valarray<double> valarrayIn, double referenceValue)
	{
		dataSample * dataSampleInstance;
		dataSampleInstance = new dataSample(valarrayIn);
		BOOST_REQUIRE(dataSampleInstance);
		BOOST_CHECK_CLOSE(dataSampleInstance->getVariance(), referenceValue, testPrecision);
	}

	BOOST_AUTO_TEST_CASE(variance1)
	{
		std::valarray<double> testValues = makeValarrayWithZeros(1);
		double referenceValue = 0.;
		checkDataSampleVariance(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance2)
	{
		std::valarray<double> testValues = makeValarrayWithOnes(23);
		double referenceValue = 0.;
		checkDataSampleVariance(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance3)
	{
		std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
		double referenceValue = 47.9166666667;
		checkDataSampleVariance(testValues, referenceValue);
	}

	BOOST_AUTO_TEST_CASE(variance4)
	{
		std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
		double referenceValue = 0.0905797101449;
		checkDataSampleVariance(testValues, referenceValue);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_CASE(printValuesToScreen)
{
	std::valarray<double> testValues(1);
	dataSample dataSampleInstance(testValues);
	dataSampleInstance.printValuesToScreen();
}

