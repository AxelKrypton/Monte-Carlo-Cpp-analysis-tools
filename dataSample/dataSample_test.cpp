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

BOOST_AUTO_TEST_CASE(mean1)
{
	// valarray initialises to zero
	std::valarray<double> testValues(1);
	double referenceValue = 0.;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(mean2)
{
	std::valarray<double> testValues(1, 23);
	double referenceValue = 1.;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(mean3)
{
	std::valarray<double> testValues(24);
	for (int iteration = 0; iteration < testValues.size(); iteration ++)
	{
		testValues[iteration] = iteration + 1;
	}
	double referenceValue = 12.5;

	dataSample * dataSampleInstance;
	dataSampleInstance = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance);

	double mean = dataSampleInstance->getMean();
	BOOST_CHECK_CLOSE(mean, referenceValue, testPrecision);
}

BOOST_AUTO_TEST_CASE(printValuesToScreen)
{
	std::valarray<double> testValues(1);
	dataSample dataSampleInstance(testValues);
	dataSampleInstance.printValuesToScreen();
}
