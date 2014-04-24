// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "binnedDataSample.hpp"

#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(withNumberOfBins_build)

	BOOST_AUTO_TEST_CASE(build)
	{
		int numberOfElements = 543;
		int numberOfBins = 1;
		DataSample sample(numberOfElements);
		BOOST_CHECK_NO_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins) );
	}

	static void testBinningWithNumberOfBins_wrongArgument(int numberOfElements, int numberOfBins)
	{
		DataSample sample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument1)
	{
		int numberOfElements = 123;
		int numberOfBins = 0;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument2)
	{
		int numberOfElements = 123;
		int numberThatIsTooBig = numberOfElements + 1;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, numberThatIsTooBig);	}

	BOOST_AUTO_TEST_CASE(invalidArgument3)
	{
		int numberOfElements = 33;
		int negativeNumber = - numberOfElements;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, negativeNumber);	}

	static void testBinningWithNumberOfBins_elements(int numberOfElements, int desiredNumberOfElementsOfBinnedDataSample)
	{
		DataSample sample(numberOfElements);
		BinnedDataSampleFromNumberOfBins binnedSample(sample, desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(desiredNumberOfElementsOfBinnedDataSample, binnedSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int numberOfElements = 7624;
		int numberOfBins = numberOfElements;
		testBinningWithNumberOfBins_elements(numberOfElements, numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int numberOfElements = 794;
		int numberOfBins = 1;
		testBinningWithNumberOfBins_elements(numberOfElements, numberOfBins);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		int numberOfElements = 795;
		int numberOfBins = numberOfElements / 2;
		testBinningWithNumberOfBins_elements(numberOfElements, numberOfBins);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(withNumberOfBins_binning)

	BOOST_AUTO_TEST_CASE(trivialBinning)
	{
		int numberOfElements = 27;
		int desiredNumberOfElementsOfBinnedDataSample = numberOfElements;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSampleAnalyzer* originalSample = testSample.getDataSample();
		BinnedDataSampleFromNumberOfBins binnedSample(*originalSample, desiredNumberOfElementsOfBinnedDataSample);
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(1), binnedSample.getNthMoment(1));
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
	}

	/**
	 * "Real binning tests":
	 * With the used samples ("makeValarrayWithArrayPosition") the
	 * binning is very simple. The expected first moment is equal to
	 * the one with from a sample with a reduced number of elements,
	 * leaving out the number of elements that "fall" away during binning.
	 */

	static void testBinningWithNumberOfBins(int numberOfElements, int numberOfBins, double expectedFirstMoment)
	{
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* originalSample = testSample.getDataSample();
		BinnedDataSampleFromNumberOfBins binnedSample(*originalSample, numberOfBins);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	BOOST_AUTO_TEST_CASE(realBinning1)
	{
		int evenNumberOfElements = 24;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(evenNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning2)
	{
		int oddNumberOfElements = 25;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(oddNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning3)
	{
		int evenNumberOfElements = 26;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 11;
		double expectedFirstMoment = 10.5;
		testBinningWithNumberOfBins(evenNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning4)
	{
		int oddNumberOfElements = 29;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 9;
		double expectedFirstMoment = 13.;
		testBinningWithNumberOfBins(oddNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(withBinsize_build)

	BOOST_AUTO_TEST_CASE(build)
	{
		int numberOfElements = 927;
		int binsize = 1;
		DataSample sample(numberOfElements);
		BOOST_CHECK_NO_THROW(BinnedDataSampleFromBinsize binnedSample(sample, binsize) );
	}

	static void testBinningWithBinsize_wrongArgument(int numberOfElements, int binsize)
	{
		DataSample originalSample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromBinsize(originalSample, binsize), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument1)
	{
		int numberOfElements = 19;
		int binsize = 0;
		testBinningWithBinsize_wrongArgument(numberOfElements, binsize);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument2)
	{
		int numberOfElements = 41;
		int numberThatIsTooBig = numberOfElements + 1;
		testBinningWithBinsize_wrongArgument(numberOfElements, numberThatIsTooBig);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument3)
	{
		int numberOfElements = 47;
		int negativeNumber = - numberOfElements;
		testBinningWithBinsize_wrongArgument(numberOfElements, negativeNumber);
	}

	static void testBinningWithBinsize_elements(int numberOfElements, int desiredBinsize)
	{
		int expectedNumberOfElementsInBinnedDataSample = numberOfElements / desiredBinsize;
		DataSample originalSample(numberOfElements);
		BinnedDataSampleFromBinsize binnedSample (originalSample, desiredBinsize);
		BOOST_CHECK_EQUAL(expectedNumberOfElementsInBinnedDataSample, binnedSample.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int numberOfElements = 25;
		int desiredBinsize = numberOfElements;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int numberOfElements = 25;
		int desiredBinsize = 1;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		int numberOfElements = 25;
		int desiredBinsize = 3;
		testBinningWithBinsize_elements(numberOfElements, desiredBinsize);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(withBinsize_binning)

	BOOST_AUTO_TEST_CASE(trivialBinning)
	{
		int numberOfElements = 25;
		int desiredBinsize = 1;
		TestDataSample testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSampleAnalyzer* originalSample = testSample.getDataSample();
		BinnedDataSampleFromBinsize binnedSample(*originalSample, desiredBinsize);
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
		BOOST_CHECK_EQUAL(originalSample->getNthMoment(2), binnedSample.getNthMoment(2));
	}

	/**
	 * "Real binning tests":
	 * With the used samples ("makeValarrayWithArrayPosition") the
	 * binning is very simple. The expected first moment is equal to
	 * the one with from a sample with a reduced number of elements,
	 * leaving out the number of elements that "fall" away during binning.
	 */

	void testBinningWithBinsize(int numberOfElements, int binsize, double expectedFirstMoment)
	{
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* originalSample = testSample.getDataSample();
		BinnedDataSampleFromBinsize binnedSample (*originalSample, binsize);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	BOOST_AUTO_TEST_CASE(realBinning1)
	{
		int evenNumberOfElements = 24;
		int evenBinsize = 12;
		double expectedFirstMoment = 11.5;
		testBinningWithBinsize(evenNumberOfElements, evenBinsize, expectedFirstMoment);
	}


	BOOST_AUTO_TEST_CASE(realBinning2)
	{
		int oddNumberOfElements = 21;
		int evenBinsize = 12;
		double expectedFirstMoment = 5.5;
		testBinningWithBinsize(oddNumberOfElements, evenBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning3)
	{
		int evenNumberOfElements = 34;
		int oddBinsize = 3;
		double expectedFirstMoment = 16.;
		testBinningWithBinsize(evenNumberOfElements, oddBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning4)
	{
		int oddNumberOfElements = 47;
		int oddBinsize = 19;
		double expectedFirstMoment = 18.5;
		testBinningWithBinsize(oddNumberOfElements, oddBinsize, expectedFirstMoment);
	}

BOOST_AUTO_TEST_SUITE_END()
