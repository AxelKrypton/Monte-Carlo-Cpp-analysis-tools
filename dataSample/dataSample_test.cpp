// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"

BOOST_AUTO_TEST_CASE(build)
{
	std::valarray<double> testValues(1);

	dataSample * dataSampleInstance1;
	dataSampleInstance1 = new dataSample(testValues);
	BOOST_REQUIRE(dataSampleInstance1);
}

BOOST_AUTO_TEST_CASE(printValuesToScreen)
{
	std::valarray<double> testValues(1);
	dataSample dataSampleInstance(testValues);
	dataSampleInstance.printValuesToScreen();
}
