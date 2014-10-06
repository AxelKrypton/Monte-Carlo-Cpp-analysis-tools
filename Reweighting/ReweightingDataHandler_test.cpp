#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ReweightingDataHandler
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "ReweightingDataHandler.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

class ReweightingDataHandlerTest : public ReweightingDataHandler{
public:
    ReweightingDataHandlerTest() : ReweightingDataHandler(){}
    ReweightingDataHandlerTest(std::string configurationFileIn) : ReweightingDataHandler(configurationFileIn){}
    int getNumberOfBinsToBeUsed(){ return numberOfBinsToBeUsed;}
};


BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        BOOST_REQUIRE_THROW(ReweightingDataHandler reweightingDataHandler, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        //These tests use configfile without mandatory binsize label since the exception must be thrown before
        std::string fileThatDoesNotExist = "fileThatShouldNotBe";
        std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_3"; //wrong structure
        std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_4"; //correct structure but with two identical set of parameters
        std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_5"; //correct structure but with two identical filenames
        std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_6"; //correct structure but with not existing file inside
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesNotExist), std::exception);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong1), std::exception);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong2), std::exception);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong3), std::exception);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong4), std::exception);
    }

    BOOST_AUTO_TEST_CASE(build3)
    {
        //Here we use configfile with mandatory binsize label even if sometimes the exception is thrown before
        std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_7"; //correct structure but with datafile with wrong number of columns
        std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_8"; //correct structure but with different parameters name in two lines
        std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_9"; //correct structure but with different number of observables in one file
        std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_10"; //correct structure but with two identical set of reweigthing parameters
        std::string fileThatDoesExistButWrong5 = "GeneralTestFiles/wrong_configfile_11"; //correct structure but without binsize at least on one line
        std::string fileThatDoesExistButWrong6 = "GeneralTestFiles/wrong_configfile_12"; //correct structure but with negative binsize
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong1), std::logic_error);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong2), std::invalid_argument);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong3), std::logic_error);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong4), std::logic_error);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong5), std::runtime_error);
        BOOST_REQUIRE_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong6), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::string fileThatDoesExist1 = "GeneralTestFiles/simulationDataContainer.configfile_1";
        std::string fileThatDoesExist2 = "GeneralTestFiles/simulationDataContainer.configfile_2";
        std::string fileThatDoesExist3 = "GeneralTestFiles/simulationDataContainer.configfile_3";
        BOOST_REQUIRE_NO_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist1));
        BOOST_REQUIRE_NO_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist2));
        BOOST_REQUIRE_NO_THROW(ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist3));
    }

    BOOST_AUTO_TEST_CASE(build5)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist);
        const int referenceNumberOfBins = 16;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed(), referenceNumberOfBins);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(getters)

    BOOST_AUTO_TEST_CASE(getters1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        ReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist);
        const int referenceNumberOfBins = 16;
        SimulationDataContainer simDataCont = reweightingDataHandler.getDataForReweighter();
        for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
            for(int j=0; j<simDataCont[i].getNumberOfDataSample(); j++){
                BOOST_REQUIRE_EQUAL(simDataCont[i][j].getNumberOfElements(), referenceNumberOfBins);
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(functionalities)

    BOOST_AUTO_TEST_CASE(writeNewConfigFile1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        ReweightingDataHandler reweightingDataHandler(fileThatDoesExist);
        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(Reweighter(reweightingDataHandler.getDataForReweighter()));
        std::string outputFileName = "configFileWithLogZ";
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
    }

    BOOST_AUTO_TEST_CASE(writeNewConfigFile2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        ReweightingDataHandler reweightingDataHandler(fileThatDoesExist);
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        Reweighter* reweighter = new Reweighter(reweightingDataHandler.getDataForReweighter(), newRanges, newNumPoints);
        std::vector<double> simulatedLogZ = reweighter->getLogZAtSimulatedPoints();
        std::string outputFileName = "testWritingConfigFile";
        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(*reweighter, outputFileName);
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        delete reweighter;
        reweighter = new Reweighter(ReweightingDataHandler(outputFileName).getDataForReweighter());
        for(size_t i=0; i<simulatedLogZ.size(); i++)
            BOOST_REQUIRE_CLOSE(reweighter->getLogZAtSimulatedPoints()[i], simulatedLogZ[i], doublePrecisionInPercent);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
        delete reweighter;
    }

    BOOST_AUTO_TEST_CASE(writeNewPointsToFile)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        ReweightingDataHandler reweightingDataHandler(fileThatDoesExist);
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        Reweighter reweighter(reweightingDataHandler.getDataForReweighter(), newRanges, newNumPoints);
        std::string outputFileName = "testWritingNewPoints";
        std::ofstream outputFile;
        outputFile.open(outputFileName.c_str());
        outputFile << "Hello!";
        outputFile.close();
        BOOST_REQUIRE_MESSAGE(boost::filesystem::exists( outputFileName ) == true, "Something bad happened creating a file!");
        BOOST_REQUIRE_THROW(reweightingDataHandler.writeNewPointsToFileWithLogZ(reweighter, outputFileName), std::invalid_argument);
        boost::filesystem::remove(outputFileName);
        std::vector<double> newLogZ = reweighter.getLogZAtNewPoints();
        reweightingDataHandler.writeNewPointsToFileWithLogZ(reweighter, outputFileName);
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
    }

BOOST_AUTO_TEST_SUITE_END()


