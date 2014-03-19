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

std::valarray<double> makeValarrayWithBigAndSmallEntries(int length)
{
	std::valarray<double> returnValarray(length*2);
	for (int iteration = 0; iteration < length; iteration ++)
	{
		returnValarray[iteration*2] = (double(iteration)) / (length - 1) * 5e6;
		returnValarray[iteration*2+1] = (double(iteration)) / (length - 1) * 5e-6;
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
  TestDataSample(int length, FillType fillType = zeros, double referenceValue = 0.):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent), fillType(fillType)
	{
	  std::valarray<double> * testValues = initDataSampleBasedOnFillType(length);
	  dataSampleInstance = new DataSample(*testValues);
	  delete testValues;
	  actualValue = 0.;
	}

	TestDataSample(std::string dataFilename, int column = 1, int offset = 0, double referenceValue = 0.):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
		actualValue = 0.;
		dataSampleInstance = new DataSample(dataFilename, column, offset);
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

	DataSample* getDataSample()
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

	DataSample * dataSampleInstance;
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
