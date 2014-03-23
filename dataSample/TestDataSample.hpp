/*
 * TestDataSample.hpp
 * Wrapper class to test DataSample.
 */

#ifndef TESTDATASAMPLE_HPP_
#define TESTDATASAMPLE_HPP_

#include "dataSampleTestUtilities.hpp"

class TestDataSample
{
public:
  TestDataSample(int length, FillType fillType = zeros, double referenceValue = 0.):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent), fillType(fillType)
	{
	  std::valarray<double> * testValues = initDataSampleBasedOnFillType(length);
	  dataSampleInstance = new DataSampleAnalyzer(*testValues);
	  delete testValues;
	  actualValue = 0.;
	}

	TestDataSample(std::string dataFilename, int column = 1, int offset = 0, double referenceValue = 0.):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSampleAnalyzer(dataFilename, column, offset);
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

	DataSampleAnalyzer* getDataSample()
	{
		return dataSampleInstance;
	}

protected:
	void testActualValueAgainstReferenceValue()
	{
		BOOST_CHECK_CLOSE(actualValue, referenceValue, testPrecision);
	}

	std::valarray<double>* initDataSampleBasedOnFillType(int length)
	{
		  if ( fillType == zeros )
		  {
			  return new std::valarray<double>(length);
		  }
		  else if ( fillType == ones )
		  {
			  std::valarray<double> tmp = makeValarrayWithOnes(length);
			  return new std::valarray<double>(tmp);
		  }
		  else if ( fillType == arrayPosition )
		  {
			  std::valarray<double> tmp = makeValarrayWithArrayPosition(length);
			  return new std::valarray<double>(tmp);
		  }
		  else if ( fillType == entriesSymmetricBetweenZeroAndOne )
		  {
			  std::valarray<double> tmp = makeValarrayWithEntriesBetweenZeroAndOne(length);
			  return new std::valarray<double>(tmp);
		  }
		  else if ( fillType == bigAndSmallEntries )
		  {
			  std::valarray<double> tmp = makeValarrayWithBigAndSmallEntries(length/2);
			  return new std::valarray<double>(tmp);
		  }
		  else
		  {
			  throw std::invalid_argument("Unknown fillType selected!");
		  }
	}

	DataSampleAnalyzer * dataSampleInstance;
	double referenceValue;
	double actualValue;
	double testPrecision;
	FillType fillType;
};

class TestDataSampleNthMoment : public TestDataSample
{
public:
	TestDataSampleNthMoment(int n, int length, FillType fillType, double referenceValue) :
		TestDataSample(length, fillType, referenceValue)
	{
		actualValue = dataSampleInstance->getNthMoment(n);
	};
	TestDataSampleNthMoment(int n, std::string dataFilename, double referenceValue = 0, int column = 1, int offset = 0.) :
		TestDataSample(dataFilename, column, offset, referenceValue)
	{
		actualValue = dataSampleInstance->getNthMoment(n);
	};
};


class TestDataSampleNthCentralMoment : public TestDataSample
{
public:
	TestDataSampleNthCentralMoment(int n, int length, FillType fillType, double referenceValue) :
		TestDataSample(length, fillType, referenceValue)
	{
		actualValue = dataSampleInstance->getNthCentralMoment(n);
	};
};


#endif /* TESTDATASAMPLE_HPP_ */
