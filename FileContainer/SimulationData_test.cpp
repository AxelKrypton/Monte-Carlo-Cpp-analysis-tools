#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FileContainer
#include <boost/test/unit_test.hpp>

#include "SimulationData.hpp"

BOOST_AUTO_TEST_SUITE(simulationData)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_REQUIRE_THROW(SimulationDataContainer simData , std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesNotExist = "fileThatShouldNotBe";
		BOOST_REQUIRE_THROW(SimulationDataContainer simData(fileThatDoesNotExist) , std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesExist = "simulationData.example";
		BOOST_CHECK_NO_THROW(SimulationDataContainer simData(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		SimulationDataContainer simData(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(simData.getNumberOfDatafiles(), numberOfDataFilesGivenInInputFile);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		int numberOfSimulationParametersGivenInInputFile = 5;
		SimulationDataContainer simData(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(simData.getNumberOfSimulationParameters(), numberOfSimulationParametersGivenInInputFile);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		int numberOfSimulationParametersGivenInInputFile = 5;
		SimulationDataContainer simData(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(simData.getNumberOfSimulationParameters(), numberOfSimulationParametersGivenInInputFile);
	}

	BOOST_AUTO_TEST_CASE(access_invArg1)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		SimulationDataContainer simData(fileThatDoesExist);
		BOOST_REQUIRE_THROW(simData[-1], std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(access_invArg2)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		SimulationDataContainer simData(fileThatDoesExist);
		BOOST_REQUIRE_THROW(simData[numberOfDataFilesGivenInInputFile+1], std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(metaData_invArg1)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		SimulationDataContainer simData(fileThatDoesExist);
		SimulationData meta = simData[0];
		BOOST_REQUIRE_THROW(meta[-1], std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(metaData_invArg2)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		int numberOfSimulationParametersGivenInInputFile = 5;
		SimulationDataContainer simData(fileThatDoesExist);
		SimulationData meta = simData[0];
		BOOST_REQUIRE_THROW(meta[numberOfSimulationParametersGivenInInputFile+1], std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(metaData1)
	{
		std::string fileThatDoesExist = "simulationData.example";
		int numberOfDataFilesGivenInInputFile = 10;
		int numberOfSimulationParametersGivenInInputFile = 5;
		SimulationDataContainer simData(fileThatDoesExist);
		SimulationData meta = simData[0];
		BOOST_REQUIRE_EQUAL(meta[0], 0.);
	}

BOOST_AUTO_TEST_SUITE_END()



