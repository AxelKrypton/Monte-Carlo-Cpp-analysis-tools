/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2015,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "binnedDataSample.hpp"
#include "binning.hpp"

#include "TestDataSample.hpp"
#include "../executables/exceptions.hpp"

BOOST_AUTO_TEST_SUITE(withNumberOfBins_build)

	BOOST_AUTO_TEST_CASE(build)
	{
		int numberOfElements = 543;
		int numberOfBins = 1;
		DataSampleBasic sample(numberOfElements);
		BOOST_CHECK_NO_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, numberOfBins) );
	}

	static void testBinningWithNumberOfBins_wrongArgument(int numberOfElements, int numberOfBins)
	{
		DataSampleBasic sample(numberOfElements);
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
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, numberThatIsTooBig);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument3)
	{
		int numberOfElements = 33;
		int negativeNumber = - numberOfElements;
		testBinningWithNumberOfBins_wrongArgument(numberOfElements, negativeNumber);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument4)
	{
		int numberOfElements = 33;
		int notFittingBinningParameter = 2;
		DataSampleBasic sample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromNumberOfBins binnedSample(sample, notFittingBinningParameter, true), wrongBinningParameter);
	}

	BOOST_AUTO_TEST_CASE(invalidArgument5)
	{
		int numberOfElements = 33;
		int notFittingBinningParameter = 2;
		DataSampleBasic sample(numberOfElements);
		BOOST_REQUIRE_THROW(BinnedDataSampleFromBinsize binnedSample(sample, notFittingBinningParameter, true), wrongBinningParameter);
	}


	static void testBinningWithNumberOfBins_elements(int numberOfElements, int desiredNumberOfElementsOfBinnedDataSample)
	{
		DataSampleBasic sample(numberOfElements);
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
		TestDataSample<realFloat> testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSample* originalSample = testSample.getDataSample();
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

	static void testBinningWithNumberOfBins(int numberOfElements, int numberOfBins, realFloat expectedFirstMoment)
	{
		TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
		DataSample* originalSample = testSample.getDataSample();
		BinnedDataSampleFromNumberOfBins binnedSample(*originalSample, numberOfBins);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	BOOST_AUTO_TEST_CASE(realBinning1)
	{
		int evenNumberOfElements = 24;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		realFloat expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(evenNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning2)
	{
		int oddNumberOfElements = 25;
		int evenDesiredNumberOfElementsOfBinnedDataSample = 12;
		realFloat expectedFirstMoment = 11.5;
		testBinningWithNumberOfBins(oddNumberOfElements, evenDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning3)
	{
		int evenNumberOfElements = 26;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 11;
		realFloat expectedFirstMoment = 10.5;
		testBinningWithNumberOfBins(evenNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning4)
	{
		int oddNumberOfElements = 29;
		int oddDesiredNumberOfElementsOfBinnedDataSample = 9;
		realFloat expectedFirstMoment = 13.;
		testBinningWithNumberOfBins(oddNumberOfElements, oddDesiredNumberOfElementsOfBinnedDataSample, expectedFirstMoment);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(withBinsize_build)

	BOOST_AUTO_TEST_CASE(build)
	{
		int numberOfElements = 927;
		int binsize = 1;
		DataSampleBasic sample(numberOfElements);
		BOOST_CHECK_NO_THROW(BinnedDataSampleFromBinsize binnedSample(sample, binsize) );
	}

	static void testBinningWithBinsize_wrongArgument(int numberOfElements, int binsize)
	{
		DataSampleBasic originalSample(numberOfElements);
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
		DataSampleBasic originalSample(numberOfElements);
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
		TestDataSample<realFloat> testSample(numberOfElements, entriesSymmetricBetweenZeroAndOne);
		DataSample* originalSample = testSample.getDataSample();
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

	void testBinningWithBinsize(int numberOfElements, int binsize, realFloat expectedFirstMoment)
	{
		TestDataSample<realFloat> testSample(numberOfElements, arrayPosition);
		DataSample* originalSample = testSample.getDataSample();
		BinnedDataSampleFromBinsize binnedSample (*originalSample, binsize);
		BOOST_CHECK_EQUAL(expectedFirstMoment, binnedSample.getNthMoment(1));
	}

	BOOST_AUTO_TEST_CASE(realBinning1)
	{
		int evenNumberOfElements = 24;
		int evenBinsize = 12;
		realFloat expectedFirstMoment = 11.5;
		testBinningWithBinsize(evenNumberOfElements, evenBinsize, expectedFirstMoment);
	}


	BOOST_AUTO_TEST_CASE(realBinning2)
	{
		int oddNumberOfElements = 21;
		int evenBinsize = 12;
		realFloat expectedFirstMoment = 5.5;
		testBinningWithBinsize(oddNumberOfElements, evenBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning3)
	{
		int evenNumberOfElements = 34;
		int oddBinsize = 3;
		realFloat expectedFirstMoment = 16.;
		testBinningWithBinsize(evenNumberOfElements, oddBinsize, expectedFirstMoment);
	}

	BOOST_AUTO_TEST_CASE(realBinning4)
	{
		int oddNumberOfElements = 47;
		int oddBinsize = 19;
		realFloat expectedFirstMoment = 18.5;
		testBinningWithBinsize(oddNumberOfElements, oddBinsize, expectedFirstMoment);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(binningWithBinsizeAndNumberOfBins)

	BOOST_AUTO_TEST_CASE(test1)
	{
		//file has 21 entries, first 20 are 1, last one is zero
		int binsize = 10;
		int numberOfBins = 2;
		std::string filename = "datafile3.example";

		DataSampleBasic tmp (filename);
		BinnedDataSampleFromBinsize binnedSample(tmp, binsize);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), 1., realFloatPrecisionInPercent);
		BinnedDataSampleFromNumberOfBins binnedSample2 (tmp, numberOfBins);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), binnedSample2.getNthMoment(1), realFloatPrecisionInPercent);
	}

	static void testBinningWithBinsizeAndNumberOfBins(std::string filename, int desiredBinsize, int desiredNumberOfBins)
	{
		DataSampleBasic tmp (filename);
		BinnedDataSampleFromBinsize binnedSample (tmp, desiredBinsize);
		BinnedDataSampleFromNumberOfBins binnedSample2 (tmp, desiredNumberOfBins);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(1), binnedSample2.getNthMoment(1), realFloatPrecisionInPercent);
		BOOST_REQUIRE_CLOSE(binnedSample.getNthMoment(2), binnedSample2.getNthMoment(2), realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(test2)
	{
		//file has 1000 non-trivial entries
		int desiredBinsize = 100;
		int desiredNumberOfBins = 10;
		std::string filename = "datafile2.example";

		testBinningWithBinsizeAndNumberOfBins(filename, desiredBinsize, desiredNumberOfBins);
	}

	BOOST_AUTO_TEST_CASE(test3)
	{
		//file has 1000 non-trivial entries
		int desiredBinsize = 10;
		int desiredNumberOfBins = 100;
		std::string filename = "datafile2.example";

		testBinningWithBinsizeAndNumberOfBins(filename, desiredBinsize, desiredNumberOfBins);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(resizeRawDataDuringBinning)

	//file has 21 entries, first 20 are 1, last one is zero
	std::string filename = "datafile3.example";
	
	BOOST_AUTO_TEST_CASE(test1)
	{
		int numberOfElements = 543;
		int numberOfBins = 1;
		DataSample sample(numberOfElements);

		DataSample binnedSample = performBinningFromNumberOfBins(sample, numberOfBins, false);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), numberOfElements);
	}
	
	BOOST_AUTO_TEST_CASE(test2)
	{
		int numberOfElements = 543;
		int numberOfBins = 5;
		DataSample sample(numberOfElements);

		DataSample binnedSample = performBinningFromNumberOfBins(sample, numberOfBins, true);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), numberOfElements - numberOfElements % numberOfBins);
	}
	
	BOOST_AUTO_TEST_CASE(test3)
	{
		int numberOfElements = 543;
		int binsize = 5;
		DataSample sample(numberOfElements);

		DataSample binnedSample = performBinningFromBinsize(sample, binsize, false);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), numberOfElements);
	}
	
	BOOST_AUTO_TEST_CASE(test4)
	{
		int numberOfElements = 543;
		int binsize = 5;
		DataSample sample(numberOfElements);

		DataSample binnedSample = performBinningFromBinsize(sample, binsize, true);
		BOOST_REQUIRE_EQUAL(sample.getNumberOfElements(), numberOfElements - numberOfElements % binsize);
	}
	
BOOST_AUTO_TEST_SUITE_END()
