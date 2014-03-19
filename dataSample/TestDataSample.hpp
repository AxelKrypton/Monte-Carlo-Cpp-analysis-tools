/*
 * TestDataSample.hpp
 */

#ifndef TESTDATASAMPLE_HPP_
#define TESTDATASAMPLE_HPP_


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

std::valarray<double> makeValarrayWithEntriesBetweenOneAndEight(int length)
{
	if (length % 8 != 0)
		throw std::invalid_argument("length must be multiple of 8!");
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < 8; iteration ++)
	{
		for (int iteration2 = 0; iteration2 < length/8; iteration2 ++)
		{
			returnValarray[iteration + 8*iteration2] = (double(iteration+1));
		}
	}
	return returnValarray;
}

//todo: write test for big and small entries
enum FillType { zeros, ones, arrayPosition, entriesSymmetricBetweenZeroAndOne, entriesBetweenOneAndEight, bigAndSmallEntries };

class TestDataSample
{
public:
  TestDataSample(int length, double referenceValue = 0., FillType fillType = zeros):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
	  std::valarray<double> * testValues;
	  if ( fillType == zeros )
	  {
		  testValues = new std::valarray<double>(length);
	  }
	  if ( fillType == ones )
	  {
		  std::valarray<double> tmp = makeValarrayWithOnes(length);
		  testValues = new std::valarray<double>(tmp);
	  }
	  if ( fillType == arrayPosition )
	  {
		  std::valarray<double> tmp = makeValarrayWithArrayPosition(length);
		  testValues = new std::valarray<double>(tmp);
	  }
	  if ( fillType == entriesSymmetricBetweenZeroAndOne )
	  {
		  std::valarray<double> tmp = makeValarrayWithEntriesBetweenZeroAndOne(length);
		  testValues = new std::valarray<double>(tmp);
	  }
	  actualValue = 0.;
	  dataSampleInstance = new DataSample(*testValues);
	  delete testValues;
	}

	TestDataSample(std::valarray<double> valarrayIn, double referenceValue):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSample(valarrayIn);
	}

	TestDataSample(std::string dataFilename, double referenceValue, int column = 1):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSample(dataFilename, column);
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

	DataSample* getSample()
	{
		return dataSampleInstance;
	}

protected:
	void testActualValueAgainstReferenceValue()
	{
		BOOST_CHECK_CLOSE(actualValue, referenceValue, testPrecision);
	}

	DataSample * dataSampleInstance;
	double referenceValue;
	double actualValue;
	double testPrecision;
};

class TestDataSampleNthMoment : public TestDataSample
{
public:
	TestDataSampleNthMoment(int n, int length, double referenceValue, FillType fillType) :
		TestDataSample(length, referenceValue, fillType)
	{
		actualValue = dataSampleInstance->getNthMoment(n);
	};
	TestDataSampleNthMoment(int n, std::string dataFilename, double referenceValue, int column = 1) :
		TestDataSample(dataFilename, referenceValue, column)
	{
		actualValue = dataSampleInstance->getNthMoment(n);
	};
};


class TestDataSampleNthCentralMoment : public TestDataSample
{
public:
	TestDataSampleNthCentralMoment(int length, double referenceValue, FillType fillType) :
		TestDataSample(length, referenceValue, fillType)
	{
		actualValue = dataSampleInstance->getNthCentralMoment();
	};
};


#endif /* TESTDATASAMPLE_HPP_ */
