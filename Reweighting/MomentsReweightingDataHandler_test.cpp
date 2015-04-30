#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MomentsReweightingDataHandler
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "MomentsReweightingDataHandler.hpp"
#include "MomentsReweighter.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

class MomentsReweightingDataHandlerTest : public MomentsReweightingDataHandler{
public:
    MomentsReweightingDataHandlerTest() : MomentsReweightingDataHandler(){}
    MomentsReweightingDataHandlerTest(std::string configurationFileIn,
                               std::vector<unsigned int> obsToBeRewUsingMultipleColumns = std::vector<unsigned int>(),
                               std::string errorMethodIn = "bootstrap")
      : MomentsReweightingDataHandler(configurationFileIn, obsToBeRewUsingMultipleColumns, errorMethodIn){}
    std::vector<int> getNumberOfBinsToBeUsed(){ return numberOfBinsToBeUsed;}
    int getNumberOfObsGivenAsInput(){ return numberOfObservablesGivenAsInput;}
    int getNumberOfObsToBeRew(){ return numberOfObservablesToBeReweighted;}
};

//TODO: Test copy ctor and operator=

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandler reweightingDataHandler, std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        //These tests use configfile without mandatory binsize label since the exception must be thrown before
        std::string fileThatDoesNotExist = "fileThatShouldNotBe";
        std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_3"; //wrong structure
        std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_4"; //correct structure but with two identical set of parameters
        std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_5"; //correct structure but with two identical filenames
        std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_6"; //correct structure but with not existing file inside
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesNotExist), std::exception);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong1), std::exception);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong2), std::exception);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong3), std::exception);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong4), std::exception);
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
        std::string fileThatDoesExistButWrong7 = "GeneralTestFiles/wrong_configfile_13"; //correct structure but with different number of observables in two files
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong1), std::logic_error);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong2), std::invalid_argument);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong3), std::logic_error);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong4), std::logic_error);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong5), std::runtime_error);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong6), std::invalid_argument);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExistButWrong7), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_3";
		BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "unknown"), std::invalid_argument);
		BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), ""), std::invalid_argument);
		BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "Jack_100"), std::invalid_argument);
		BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot_100_200"), std::invalid_argument);
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jackknife"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "JaCk"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jAcKKniFe"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "bootstrap"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "bOOt"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "BooTSTRap"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot 200"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot_300"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot-400"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "boot,500"));
		BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "bootstrap.1000"));
	}

    BOOST_AUTO_TEST_CASE(build5)
    {
        std::string fileThatDoesExist1 = "GeneralTestFiles/simulationDataContainer.configfile_1";
        std::string fileThatDoesExist2 = "GeneralTestFiles/simulationDataContainer.configfile_2";
        std::string fileThatDoesExist3 = "GeneralTestFiles/simulationDataContainer.configfile_3";
        BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist1, std::vector<unsigned int>(), "jack"));
        BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist2, std::vector<unsigned int>(), "jack"));
        BOOST_REQUIRE_NO_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist3, std::vector<unsigned int>(), "jack"));
    }

    BOOST_AUTO_TEST_CASE(build6)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
        const int referenceNumberOfBins = 16;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins);
    }

    BOOST_AUTO_TEST_CASE(build7)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_6";
        std::vector<unsigned int> multipleColumns(1,1);
        MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, multipleColumns);
        const int referenceNumberObsGiven = 4;
        const int referenceNumberObsRew = 16;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsGivenAsInput(), referenceNumberObsGiven);
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsToBeRew(), referenceNumberObsRew);
        multipleColumns.push_back(3);
        BOOST_REQUIRE_THROW(MomentsReweightingDataHandlerTest reweightingDataHandler2(fileThatDoesExist, multipleColumns), std::invalid_argument);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(getters)

    BOOST_AUTO_TEST_CASE(getters1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
        std::vector<std::string> referenceParams;
        referenceParams.push_back("beta");
        referenceParams.push_back("chem_pot_im");
        std::vector<std::string> gottenParams = reweightingDataHandler.getNamesOfParametersIgnoringMetaParameters();
        for(size_t i=0; i<referenceParams.size(); i++)
            BOOST_REQUIRE_EQUAL(referenceParams[i], gottenParams[i]);
    }

    BOOST_AUTO_TEST_CASE(getters2)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
        std::vector<std::vector<double> > referenceValues(3, std::vector<double>(2));
        referenceValues[0][0] = 4.0;
        referenceValues[1][0] = 4.5;
        referenceValues[2][0] = 4.5;
        referenceValues[0][1] = 0.7;
        referenceValues[1][1] = 1.0;
        referenceValues[2][1] = 1.3;
        std::vector<std::vector<double> > gottenValues = reweightingDataHandler.getValuesOfSimulationParametersIgnoringMetaParameters();
        for(size_t i=0; i<referenceValues.size(); i++){
            for(size_t j=0; j<referenceValues[i].size(); j++)
                BOOST_REQUIRE_EQUAL(referenceValues[i][j], gottenValues[i][j]);
        }
    }

    BOOST_AUTO_TEST_CASE(getters3)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        MomentsReweightingDataHandlerTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
        std::vector<double> referenceLogZ(3, 0.0);
        referenceLogZ[1] = 3.14;
        std::vector<double> setLogZ(3, 0.0);
        reweightingDataHandler.extractAndSetProvidedValuesOfLogZAtSimulatedPoints(setLogZ);
        for(size_t i=0; i<referenceLogZ.size(); i++)
            BOOST_REQUIRE_EQUAL(referenceLogZ[i], setLogZ[i]);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(functionalities)

//TODO: Write a ReweighterMockup object to be used inside these tests!

    BOOST_AUTO_TEST_CASE(writeNewConfigFile1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        MomentsReweightingDataHandler reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(MomentsReweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(), "jack"));
        std::string outputFileName = "configFileWithLogZ";
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
    }

    BOOST_AUTO_TEST_CASE(writeNewConfigFile2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        MomentsReweightingDataHandler reweightingDataHandler(fileThatDoesExist);
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        MomentsReweighter* reweighter = new MomentsReweighter(fileThatDoesExist, newRanges, newNumPoints);
        std::vector<double> simulatedLogZ = reweighter->getLogZAtSimulatedPoints();
        std::string outputFileName = "testWritingConfigFile";
        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(*reweighter, outputFileName);
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        delete reweighter;
        reweighter = new MomentsReweighter(outputFileName);
        for(size_t i=0; i<simulatedLogZ.size(); i++)
            BOOST_REQUIRE_CLOSE(reweighter->getLogZAtSimulatedPoints()[i], simulatedLogZ[i], doublePrecisionInPercent);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
        delete reweighter;
    }

    BOOST_AUTO_TEST_CASE(writeNewPointsToFile)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        MomentsReweightingDataHandler reweightingDataHandler(fileThatDoesExist);
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        MomentsReweighter reweighter(fileThatDoesExist, newRanges, newNumPoints);
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


