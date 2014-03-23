// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dataSample
#include <boost/test/unit_test.hpp>

#include "dataSample.hpp"
#include "dataSampleAnalyzer.hpp"

#include <cfloat>
#include "TestDataSample.hpp"

BOOST_AUTO_TEST_SUITE(precision)

	BOOST_AUTO_TEST_CASE(doublePrecision1)
	{
		int numberOfValidDigitsOnSystem = DBL_DIG; //from <cfloat>
		int desiredNumberOfValidDigits = 15;
		BOOST_REQUIRE_EQUAL(numberOfValidDigitsOnSystem,desiredNumberOfValidDigits );
	}

	BOOST_AUTO_TEST_CASE(doublePrecision2)
	{
		double numberCorrectUpTo15thDigit        = 1234567890123450e-15;
		double anotherNumberCorrectUpTo15thDigit = 1234567890123452e-15;
		BOOST_CHECK_CLOSE(numberCorrectUpTo15thDigit, anotherNumberCorrectUpTo15thDigit, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(doublePrecision3)
	{
		double numberCorrectUpTo15thDigit        = 123456789012345.0;
		double anotherNumberCorrectUpTo15thDigit = 123456789012345.2;
		BOOST_CHECK_CLOSE(numberCorrectUpTo15thDigit, anotherNumberCorrectUpTo15thDigit, doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(doublePrecision4)
	{
		double twoThird = 2./3.;
		double oneHalf = .5;
		double oneThird = twoThird * oneHalf;
		//                 digits:    1234567890123456
		double oneThirdTo15thDigit = 3333333333333339e-16;
		BOOST_CHECK_CLOSE(oneThird, oneThirdTo15thDigit, doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

//todo: if files are found in subdir can depend on the system!
BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_CHECK_NO_THROW(DataSample dataSampleInstance(1));
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		DataSample defaultDataSample;
		BOOST_CHECK_EQUAL(defaultDataSample.getNumberOfElements(), 1);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesNotExist = "theFileThatShouldNotBe";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatDoesNotExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_CHECK_NO_THROW( DataSample dataSample(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "datafile.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatDoesExist, 0), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatHasOnlyOneColumn = "datafile.example";
		BOOST_REQUIRE_THROW( DataSample dataSample(fileThatHasOnlyOneColumn, 2), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatHasTwoColumns = "datafileWithTwoColumns.example";
		BOOST_CHECK_NO_THROW( DataSample dataSample(fileThatHasTwoColumns, 2));
	}

	BOOST_AUTO_TEST_CASE(elements1)
	{
		int elementsOfTestArray = 17;
		DataSample dataSampleInstance(elementsOfTestArray);
		BOOST_CHECK_EQUAL(elementsOfTestArray, dataSampleInstance.getNumberOfElements());
	}

	BOOST_AUTO_TEST_CASE(elements2)
	{
		int elementsOfTestArray = 0;
		BOOST_REQUIRE_THROW(DataSample dataSampleInstance(elementsOfTestArray), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(elements3)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		DataSample dataSample(fileThatDoesExist);
		int numberOfElementsInDataSample = dataSample.getNumberOfElements();
		BOOST_CHECK_EQUAL(linesInFile, numberOfElementsInDataSample);
	}

	BOOST_AUTO_TEST_CASE(fileWithOffset_invalidArg)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int negativeOffset = -1;
		BOOST_REQUIRE_THROW(DataSample dataSample(fileThatDoesExist, 1, negativeOffset), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(fileWithOffset_notYetImplemented)
	{
		std::string fileThatDoesExist = "datafile.example";
		int linesInFile = 1005;
		int validOffset = 1;
		BOOST_REQUIRE_THROW(DataSample dataSample(fileThatDoesExist, 1, validOffset), std::invalid_argument);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(operators)

	BOOST_AUTO_TEST_CASE(access)
	{
		int numberOfElements = 10;
		DataSample sample(numberOfElements);
		BOOST_REQUIRE_EQUAL(sample[numberOfElements-1], 0);
	}

	BOOST_AUTO_TEST_CASE(set)
	{
		int numberOfElements = 10;
		double someValue = 1.23456789;
		int index = numberOfElements/2;
		DataSample sample(numberOfElements);
		sample[index] = someValue;
		BOOST_REQUIRE_EQUAL(sample[index], someValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(applyFunction)

	double square(double in)
	{
		return in * in;
	}

	BOOST_AUTO_TEST_CASE(applyFunction1)
	{
		int numberOfElements = 53;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSample* sample = testSample.getDataSample();
		DataSample sampleFromFunction = sample->applyFunction();
		BOOST_CHECK_CLOSE(sampleFromFunction.sum(), sample->sum(), doublePrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(applyFunction2)
	{
		int numberOfElements = 53;
		TestDataSample testSample(numberOfElements, arrayPosition);
		DataSampleAnalyzer* sample = testSample.getDataSample();
		DataSampleAnalyzer sampleFromFunction = sample->applyFunction(square);
		BOOST_CHECK_CLOSE(sampleFromFunction.getNthMoment(1), sample->getNthMoment(2), doublePrecisionInPercent);
	}

BOOST_AUTO_TEST_SUITE_END()

