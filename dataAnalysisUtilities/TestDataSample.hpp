/*
 * TestDataSample.hpp
 * Wrapper class to test DataSample.
 */

#ifndef TESTDATASAMPLE_HPP_
#define TESTDATASAMPLE_HPP_

#include "dataSampleTestUtilities.hpp"
#include <type_traits>


template <class T = double> class TestDataSample
{
public:
  	TestDataSample(int length, FillType fillType = zeros, T* referenceValue = NULL):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent), fillType(fillType)
	{
	  if(!(std::is_same<T, double>::value) && !(std::is_same<T, DataSampleBasic>::value))
		  throw std::invalid_argument("TestDataSample created with an invalid template type!");
	  std::valarray<double> * testValues = initDataSampleBasedOnFillType(length);
	  dataSampleInstance = new DataSample(*testValues);
	  delete testValues;
	  actualValue = NULL;
	}

	TestDataSample(std::string dataFilename, int column = 1, int offset = 0, T* referenceValue = NULL):
		referenceValue(referenceValue), testPrecision(doublePrecisionInPercent)
	{
        if(!(std::is_same<T, double>::value) && !(std::is_same<T, DataSampleBasic>::value))
        	throw std::invalid_argument("TestDataSample created with an invalid template type!");
		actualValue = NULL;
		dataSampleInstance = new DataSample(dataFilename, column, offset);
		fillType = zeros;
	}

	virtual ~TestDataSample()
	{
		testActualValueAgainstReferenceValue();
		delete dataSampleInstance;
		if(actualValue != NULL) delete actualValue;
		//Do not delete referenceValue since the memory it points to is "coming from outside the class"
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
		/*
		 * Here I cast T objects to double or to DataSampleBasic since the compiler doesn't know it is the case (we are inside the if)
		 * TODO: it is quite ugly code. Think how to improve it, maybe template specialization!?
		 */
		if(actualValue != NULL && referenceValue != NULL){
			if(std::is_same<T, double>::value)
				//Here I cast T objects to DataSampleBasic since the compiler doesn't know it is the case (we are inside the if)
				BOOST_CHECK_CLOSE(*(reinterpret_cast<double*>(actualValue)), *(reinterpret_cast<double*>(referenceValue)), testPrecision);
			else if (std::is_same<T, DataSampleBasic>::value){
				//Here I cast T objects to DataSampleBasic since the compiler doesn't know it is the case (we are inside the if)
				for(int i=0; i<(*reinterpret_cast<DataSampleBasic*>(actualValue)).getNumberOfElements(); i++)
					BOOST_CHECK_CLOSE((*reinterpret_cast<DataSampleBasic*>(actualValue))[i], (*reinterpret_cast<DataSampleBasic*>(referenceValue))[i], testPrecision);
			} else
				throw std::logic_error("Thrown an exception that should never be thrown! Investigate!");
		}
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
		  else if ( fillType == onesMinusOnes )
		  {
			  std::valarray<double> tmp = makeValarrayWithOnesAndMinusOnes(length);
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
	T* referenceValue;
	T* actualValue;
	double testPrecision;
	FillType fillType;
};

class TestDataSampleNthMoment : public TestDataSample<>
{
public:
	TestDataSampleNthMoment(int n, int length, FillType fillType, double &referenceValue) :
		TestDataSample(length, fillType, &referenceValue)
	{
		actualValue = new double;
		*actualValue = dataSampleInstance->getNthMoment(n);
	};
	TestDataSampleNthMoment(int n, std::string dataFilename, double referenceValue = 0, int column = 1, int offset = 0.) :
		TestDataSample(dataFilename, column, offset, &referenceValue)
	{
		actualValue = new double;
		*actualValue = dataSampleInstance->getNthMoment(n);
	};
};


class TestDataSampleNthCentralMoment : public TestDataSample<>
{
public:
	TestDataSampleNthCentralMoment(int n, int length, FillType fillType, double &referenceValue) :
		TestDataSample(length, fillType, &referenceValue)
	{
		actualValue = new double;
		*actualValue = dataSampleInstance->getNthCentralMoment(n);
	};
};


class TestDataSampleNthMomentPerDataPoint : public TestDataSample<DataSampleBasic>
{
public:
	TestDataSampleNthMomentPerDataPoint(int n, int length, FillType fillType, DataSampleBasic &referenceValue) :
		TestDataSample<DataSampleBasic>(length, fillType, &referenceValue)
	{
		actualValue = new DataSampleBasic(length);
		*actualValue = dataSampleInstance->getNthMomentPerDataPoint(n);
	};
};


class TestDataSampleNthCentralMomentPerDataPoint : public TestDataSample<DataSampleBasic>
{
public:
	TestDataSampleNthCentralMomentPerDataPoint(int n, int length, FillType fillType, DataSampleBasic &referenceValue) :
		TestDataSample<DataSampleBasic>(length, fillType, &referenceValue)
	{
		actualValue = new DataSampleBasic;
		*actualValue = dataSampleInstance->getNthCentralMomentPerDataPoint(n);
	};
};


#endif /* TESTDATASAMPLE_HPP_ */
