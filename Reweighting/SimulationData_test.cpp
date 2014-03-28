#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SimulationData
#include <boost/test/unit_test.hpp>

#include "SimulationData.hpp"

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_REQUIRE_THROW(SimulationData simData, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesNotExist = "fileThatShouldNotBe";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesNotExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesExistButWrong = "wrong_inputfile_1"; //not all lines with same number of columns
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExistButWrong = "wrong_inputfile_2"; //not all lines with same number of columns
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExistButWrong = "wrong_inputfile_3"; //only commented or empty lines
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExistButWrong = "wrong_inputfile_4"; //not only numeric data
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatDoesExistButWrong = "wrong_inputfile_5"; //not only numeric data
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build8)
	{
		std::string fileThatDoesExistButWrong = "simulationDataContainer.configfile"; //not only numeric data
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_THROW(SimulationData simData(filledMap, fileThatDoesExistButWrong), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build9)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> emptyMap;
		BOOST_REQUIRE_THROW(SimulationData simData(emptyMap, fileThatDoesExist), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build10)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		BOOST_REQUIRE_NO_THROW(SimulationData simData(filledMap, fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build11)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		const int offset = 1;
		SimulationData simData(filledMap, fileThatDoesExist, offset);
		const int numberOfDataRead = 2;
		BOOST_REQUIRE_EQUAL(simData[0].getNumberOfElements(), numberOfDataRead);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(getters)

	BOOST_AUTO_TEST_CASE(getters1)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		SimulationData simData(filledMap, fileThatDoesExist);
		const int numberOfSimulationParameters = 1;
		BOOST_REQUIRE_EQUAL(simData.getNumberOfSimulationParameters(), numberOfSimulationParameters);
	}

	BOOST_AUTO_TEST_CASE(getters2)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		SimulationData simData(filledMap, fileThatDoesExist);
		const int numberOfSimulationDataSample = 3;
		BOOST_REQUIRE_EQUAL(simData.getNumberOfDataSample(), numberOfSimulationDataSample);
	}

	BOOST_AUTO_TEST_CASE(getters3)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		filledMap["chem_pot_im"] = 1.047;
		SimulationData simData(filledMap, fileThatDoesExist);
		const double betaValue = 4.0;
		const double chempotValue = 1.047;
		BOOST_REQUIRE_EQUAL(simData.getParameterValue("beta"), betaValue);
		BOOST_REQUIRE_EQUAL(simData.getParameterValue("chem_pot_im"), chempotValue);
		BOOST_REQUIRE_THROW(simData.getParameterValue("log_Z"), std::out_of_range);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(AccessOperator)

	BOOST_AUTO_TEST_CASE(AccessOperator1)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		SimulationData simData(filledMap, fileThatDoesExist);
		BOOST_REQUIRE_THROW(simData[-1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator2)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		SimulationData simData(filledMap, fileThatDoesExist);
		int numberOfDataSampleGivenInInputFile = simData.getNumberOfDataSample();
		BOOST_REQUIRE_THROW(simData[numberOfDataSampleGivenInInputFile+1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator3)
	{
		std::string fileThatDoesExist = "input_test_file_1";
		std::map<std::string, double> filledMap;
		filledMap["beta"] = 4.0;
		SimulationData simData(filledMap, fileThatDoesExist);
		int numberOfDataSampleGivenInInputFile = simData.getNumberOfDataSample();
		BOOST_REQUIRE_NO_THROW(DataSample dataSample = simData[numberOfDataSampleGivenInInputFile-1]);
	}

BOOST_AUTO_TEST_SUITE_END()
