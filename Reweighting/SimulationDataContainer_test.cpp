#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SimulationDataContainer
#include <boost/test/unit_test.hpp>

#include "SimulationDataContainer.hpp"

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesNotExist = "fileThatShouldNotBe";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesNotExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesExist = "wrong_configfile_1";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "wrong_configfile_2";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "wrong_configfile_3";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		BOOST_CHECK_NO_THROW(SimulationDataContainer simDataCont(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		int numberOfDataFilesGivenInInputFile = 3;
		SimulationDataContainer simDataCont(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfDatafiles(), numberOfDataFilesGivenInInputFile);
	}

	BOOST_AUTO_TEST_CASE(build8)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		int numberOfSimulationParametersGivenInInputFile = 2;
		SimulationDataContainer simDataCont(fileThatDoesExist);
		for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++)
			BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfSimulationParameters(i), numberOfSimulationParametersGivenInInputFile);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(AccessOperator)

	BOOST_AUTO_TEST_CASE(AccessOperator1)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		BOOST_REQUIRE_THROW(simDataCont[-1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator2)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		int numberOfDataFilesGivenInInputFile = simDataCont.getNumberOfDatafiles();
		BOOST_REQUIRE_THROW(simDataCont[numberOfDataFilesGivenInInputFile+1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator3)
	{
		std::string fileThatDoesExist = "simulationDataContainer.configfile";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		int numberOfDataFilesGivenInInputFile = simDataCont.getNumberOfDatafiles();
		BOOST_REQUIRE_NO_THROW(SimulationData simData = simDataCont[numberOfDataFilesGivenInInputFile-1]);
	}

BOOST_AUTO_TEST_SUITE_END()
