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

BOOST_AUTO_TEST_SUITE(mean)

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
		returnValarray[iteration] = (double(iteration)) / (returnValarray.size() -1);
	}
	return returnValarray;
}

BOOST_AUTO_TEST_CASE(mean1)
{
	std::valarray<double> testValues = makeValarrayWithZeros(1);
	double referenceValue = 0.;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(mean2)
{
	std::valarray<double> testValues = makeValarrayWithOnes(23);
	double referenceValue = 1.;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(mean3)
{
	std::valarray<double> testValues = makeValarrayWithArrayPosition(24);
	double referenceValue = 11.5;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(mean4)
{
	std::valarray<double> testValues = makeValarrayWithEntriesBetweenZeroAndOne(24);
	double referenceValue = 0.5;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(printValuesToScreen)
{
	std::valarray<double> testValues(1);
	dataSample dataSampleInstance(testValues);
	dataSampleInstance.printValuesToScreen();
}

