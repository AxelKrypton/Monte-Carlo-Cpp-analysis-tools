#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SimulationDataContainer
#include <boost/test/unit_test.hpp>

#include "SimulationDataContainer.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

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
		std::string fileThatDoesExist = "GeneralTestFiles/wrong_configfile_1";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/wrong_configfile_2";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/wrong_configfile_3";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/wrong_configfile_4";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build7)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/wrong_configfile_5";
		BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont(fileThatDoesExist), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build8)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		BOOST_CHECK_NO_THROW(SimulationDataContainer simDataCont(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build9)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		int numberOfDataFilesGivenInInputFile = 3;
		SimulationDataContainer simDataCont(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfDatafiles(), numberOfDataFilesGivenInInputFile);
	}

	BOOST_AUTO_TEST_CASE(build10)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        const int numberOfSimulationParametersGivenInInputFileOnFirstLine = 4;
        const int numberOfSimulationParametersGivenInInputFileOnOtherLines = 3;
		SimulationDataContainer simDataCont(fileThatDoesExist);
        BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfSimulationParameters(0),
                            numberOfSimulationParametersGivenInInputFileOnFirstLine);
        for(int i=1; i<simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfSimulationParameters(i),
                                numberOfSimulationParametersGivenInInputFileOnOtherLines);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(AccessOperator)

	BOOST_AUTO_TEST_CASE(AccessOperator1)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		BOOST_REQUIRE_THROW(simDataCont[-1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator2)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		int numberOfDataFilesGivenInInputFile = simDataCont.getNumberOfDatafiles();
		BOOST_REQUIRE_THROW(simDataCont[numberOfDataFilesGivenInInputFile+1], std::out_of_range);
	}

	BOOST_AUTO_TEST_CASE(AccessOperator3)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		SimulationDataContainer simDataCont(fileThatDoesExist);
		int numberOfDataFilesGivenInInputFile = simDataCont.getNumberOfDatafiles();
		BOOST_REQUIRE_NO_THROW(SimulationData simData = simDataCont[numberOfDataFilesGivenInInputFile-1]);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(BinningContainer)

    BOOST_AUTO_TEST_CASE(BinningContainer1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        const int numberOfBins = 100;
        std::vector<int> referenceEntriesToBeLeftOut(3);
        referenceEntriesToBeLeftOut[0] = 4;
        referenceEntriesToBeLeftOut[1] = 53;
        referenceEntriesToBeLeftOut[2] = 84;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::pair<SimulationDataContainer, std::vector<int> >
                uncorrObject = simDataCont.getUncorrelatedSimulationDataSetAndNumbersOfEntriesLeftOut(numberOfBins);
        for(int i=0; i<3; i++){
            BOOST_CHECK_EQUAL(uncorrObject.second[i], referenceEntriesToBeLeftOut[i]);
            BOOST_CHECK_EQUAL(uncorrObject.first[i][0].getNumberOfElements(), numberOfBins);
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(InsertCentralMoments)

    BOOST_AUTO_TEST_CASE(InsertCentralMoments1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> columns;
        std::vector<unsigned int> moments;
        std::vector<bool> useMultipleColumns(3, false);
        columns.push_back(0);
        columns.push_back(1);
        columns.push_back(2);
        moments.push_back(0);
        moments.push_back(1);
        simDataCont = simDataCont.insertMomentsPerData(columns, moments, useMultipleColumns);
        const int newNumberOfColumns = 9;
        const double columnsFirstMomentFirstFile[3][3] = {1.1, 1.4, 1.7, 1.2, 1.5, 1.8, 1.3, 1.6, -1.9};
        for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont[i].getNumberOfDataSample(), newNumberOfColumns);
        for(int i=0; i<3; i++){
            BOOST_REQUIRE_CLOSE(simDataCont[0][4][i], columnsFirstMomentFirstFile[0][i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][6][i], columnsFirstMomentFirstFile[1][i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][8][i], columnsFirstMomentFirstFile[2][i], doublePrecisionInPercent);
        }
        for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
            for(int j=0; j<3; j++){
                BOOST_REQUIRE_EQUAL(simDataCont[i][3][j], 1.0);
                BOOST_REQUIRE_EQUAL(simDataCont[i][5][j], 1.0);
                BOOST_REQUIRE_EQUAL(simDataCont[i][7][j], 1.0);
            }
        }
    }

    BOOST_AUTO_TEST_CASE(InsertCentralMoments2)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_4";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> columns;
        std::vector<unsigned int> moments;
        std::vector<bool> useMultipleColumns(1, true);
        columns.push_back(0);
        moments.push_back(2);
        moments.push_back(3);
        moments.push_back(4);
        simDataCont = simDataCont.insertMomentsPerData(columns, moments, useMultipleColumns);
        const int newNumberOfColumns = 4;
        const double FirstMoment[3] = {2, 2.1, 2.2};
        const double SecondMoment[3] = {0.688333333333333, 0.648333333333333, 0.588333333333333,};
        const double ThirdMoment[3] = {-21.7715, -27.2085, -33.4555};
        const double FourthMoment[3] = {-163.68, -218.484, -285.1368};
        BOOST_REQUIRE_EQUAL(simDataCont[0].getNumberOfDataSample(), newNumberOfColumns);
        for(int i=0; i<3; i++){
            BOOST_REQUIRE_CLOSE(simDataCont[0][0][i], FirstMoment[i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][1][i], SecondMoment[i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][2][i], ThirdMoment[i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][3][i], FourthMoment[i], doublePrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()


