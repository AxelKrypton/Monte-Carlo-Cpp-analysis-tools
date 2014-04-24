// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "binnedDataSample.hpp"

BOOST_AUTO_TEST_SUITE(withNumberOfBins)

	BOOST_AUTO_TEST_CASE(build1)
	{
		int numberOfElements = 543;
		int numberOfBins = 1;
		DataSample sample(numberOfElements);
		BOOST_CHECK_NO_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins) );
	}

	BOOST_AUTO_TEST_CASE(invalidArgument1)
	{
		int numberOfElements = 123;
		int numberOfBins = 0;
		DataSample sample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument2)
	{
		int numberOfElements = 123;
		int numberOfBins = numberOfElements + 1;
		DataSample sample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int numberOfElements = 7624;
		int numberOfBins = numberOfElements;
		DataSample sample(numberOfElements);
		BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins);
		BOOST_REQUIRE_EQUAL(numberOfElements, binnedSample.getNumberOfElements() );
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int numberOfElements = 794;
		int numberOfBins = 1;
		DataSample sample(numberOfElements);
		BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins);
		BOOST_REQUIRE_EQUAL(numberOfBins, binnedSample.getNumberOfElements() );
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		int numberOfElements = 795;
		int numberOfBins = numberOfElements / 2;
		DataSample sample(numberOfElements);
		BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins);
		BOOST_REQUIRE_EQUAL(numberOfBins, binnedSample.getNumberOfElements() );
	}

BOOST_AUTO_TEST_SUITE_END()
