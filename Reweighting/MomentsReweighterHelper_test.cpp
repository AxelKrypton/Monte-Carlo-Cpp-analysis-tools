#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MomentsReweightingDataHandler
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "MomentsReweighterHelper.hpp"
#include "MomentsReweighter.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

class MomentsReweighterHelperTest{
public:
    MomentsReweighterHelperTest(std::initializer_list<std::string> options, std::vector<unsigned int> momentsToBeReweighted = {}, std::vector<int> binsizesToBeUsed = {})
		: momentsReweighterHelper(ReweighterTester(options).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed)){}
	MomentsReweighterHelperTest(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformation)
		: momentsReweighterHelper(rawDataForReweightingAndMetainformation){}
//    MomentsReweighterHelperTest(std::string configurationFileIn,
//                               std::vector<unsigned int> obsToBeRewUsingMultipleColumns = std::vector<unsigned int>(),
//                               std::string errorMethodIn = "bootstrap")
//      : MomentsReweighterHelper(configurationFileIn, obsToBeRewUsingMultipleColumns, errorMethodIn){}
    std::vector<int> getNumberOfBinsToBeUsed(){ return momentsReweighterHelper.numberOfBinsToBeUsed; }
    int getNumberOfObsGivenAsInput(){ return momentsReweighterHelper.numberOfObservablesGivenAsInput; }
    int getNumberOfObsToBeRew(){ return momentsReweighterHelper.numberOfObservablesToBeReweighted; }
    SimulationDataContainer getRawData(){ return momentsReweighterHelper.simulationRawDataContainer; }
private:
    MomentsReweighterHelper momentsReweighterHelper;
};

//TODO: Implement tests for ctor (i.e. left out points in binning)

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1}, {100,100,100});
        const int referenceNumberOfBins = 16;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins);
    }

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_3";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
		MomentsReweighterHelperTest reweightingDataHandler(options, {1}, {100,100,100});
		std::vector<int> referenceNumberOfBins{23, 16, 25};
		BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins[0]);
		BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[1], referenceNumberOfBins[1]);
		BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[2], referenceNumberOfBins[2]);
	}

    BOOST_AUTO_TEST_CASE(build3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_6";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--obsMultipleColumns=1"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1,2,3,4}, {100,100,100});
        const int referenceNumberObsGiven = 4;
        const int referenceNumberObsRew = 19;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsGivenAsInput(), referenceNumberObsGiven);
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsToBeRew(), referenceNumberObsRew);
        std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--obsMultipleColumns=1", "3"};
        BOOST_REQUIRE_THROW(MomentsReweighterHelperTest reweightingDataHandler2(options2, {1,2,3,4}, {100,100,100}), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1,2,3,4}, {1,1,1});
        std::vector<int> fileLines{2304, 1653, 2584};
        SimulationDataContainer rawData = reweightingDataHandler.getRawData();
        for(int i=0; i<rawData.getNumberOfDatafiles(); i++)
        	BOOST_REQUIRE_EQUAL(rawData[i][0].getNumberOfElements(), fileLines[i]);
    }


BOOST_AUTO_TEST_SUITE_END()


//BOOST_AUTO_TEST_SUITE(getters)
//
//    BOOST_AUTO_TEST_CASE(getters1)
//    {
//        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
//        MomentsReweighterHelperTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
//        std::vector<std::string> referenceParams;
//        referenceParams.push_back("beta");
//        referenceParams.push_back("chem_pot_im");
//        std::vector<std::string> gottenParams = reweightingDataHandler.getNamesOfParametersIgnoringMetaParameters();
//        for(size_t i=0; i<referenceParams.size(); i++)
//            BOOST_REQUIRE_EQUAL(referenceParams[i], gottenParams[i]);
//    }
//
//    BOOST_AUTO_TEST_CASE(getters2)
//    {
//        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
//        MomentsReweighterHelperTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
//        std::vector<std::vector<double> > referenceValues(3, std::vector<double>(2));
//        referenceValues[0][0] = 4.0;
//        referenceValues[1][0] = 4.5;
//        referenceValues[2][0] = 4.5;
//        referenceValues[0][1] = 0.7;
//        referenceValues[1][1] = 1.0;
//        referenceValues[2][1] = 1.3;
//        std::vector<std::vector<double> > gottenValues = reweightingDataHandler.getValuesOfSimulationParametersIgnoringMetaParameters();
//        for(size_t i=0; i<referenceValues.size(); i++){
//            for(size_t j=0; j<referenceValues[i].size(); j++)
//                BOOST_REQUIRE_EQUAL(referenceValues[i][j], gottenValues[i][j]);
//        }
//    }
//
//    BOOST_AUTO_TEST_CASE(getters3)
//    {
//        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
//        MomentsReweighterHelperTest reweightingDataHandler(fileThatDoesExist, std::vector<unsigned int>(), "jack");
//        std::vector<double> referenceLogZ(3, 0.0);
//        referenceLogZ[1] = 3.14;
//        std::vector<double> setLogZ(3, 0.0);
//        reweightingDataHandler.extractAndSetProvidedValuesOfLogZAtSimulatedPoints(setLogZ);
//        for(size_t i=0; i<referenceLogZ.size(); i++)
//            BOOST_REQUIRE_EQUAL(referenceLogZ[i], setLogZ[i]);
//    }
//
//BOOST_AUTO_TEST_SUITE_END()


//BOOST_AUTO_TEST_SUITE(functionalities)
//
////TODO: Write a ReweighterMockup object to be used inside these tests!
//
//    BOOST_AUTO_TEST_CASE(writeNewConfigFile1)
//    {
//        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
//		std::initializer_list<std::string> options = {("-f" + fileThatDoesExist), "--useJackknifeAsErrorMethod"};
//        std::vector<unsigned int> momentsToBeReweighted{1, 2, 3, 4};
//        std::vector<int> binsizesToBeUsed{2, 2, 2};
//        RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed);
//        std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.8, 1.2)};
//		std::vector< unsigned int> newNumPoints(2, 3);
//		rawDataAndMetaInfo.newRangesOfParameters = newRanges;
//		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
//        MomentsReweighterHelper reweightingDataHandler(rawDataAndMetaInfo);
//        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(MomentsReweighter(rawDataAndMetaInfo));
//        std::string outputFileName = "configFileWithLogZ";
//        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
//        if(boost::filesystem::exists(outputFileName))
//            boost::filesystem::remove(outputFileName);
//    }
//
//    BOOST_AUTO_TEST_CASE(writeNewConfigFile2)
//    {
//        std::string fileThatDoesExist = "RealTestData/configfile_1";
//		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
//													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
//        std::vector<unsigned int> momentsToBeReweighted{1, 2, 3, 4};
//		std::vector<int> binsizesToBeUsed{100, 100, 100};
//		RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed);
//        MomentsReweighterHelper reweightingDataHandler(rawDataAndMetaInfo);
//        MomentsReweighter* reweighter = new MomentsReweighter(rawDataAndMetaInfo);
//        std::vector<double> simulatedLogZ = reweighter->getLogZAtSimulatedPoints();
//        std::string outputFileName = "testWritingConfigFile";
//        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(*reweighter, outputFileName);
//        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
//        delete reweighter;
//		std::initializer_list<std::string> options2 = {"-f" + outputFileName, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
//													   "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
//        reweighter = new MomentsReweighter(ReweighterTester(options2).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed));
//        for(size_t i=0; i<simulatedLogZ.size(); i++)
//            BOOST_REQUIRE_CLOSE(reweighter->getLogZAtSimulatedPoints()[i], simulatedLogZ[i], doublePrecisionInPercent);
//        if(boost::filesystem::exists(outputFileName))
//            boost::filesystem::remove(outputFileName);
//        delete reweighter;
//    }
//
//    BOOST_AUTO_TEST_CASE(writeNewPointsToFile)
//    {
//        std::string fileThatDoesExist = "RealTestData/configfile_1";
//		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
//													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
//		std::vector<unsigned int> momentsToBeReweighted{1, 2, 3, 4};
//		std::vector<int> binsizesToBeUsed{100, 100, 100};
//		RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed);
//		MomentsReweighterHelper reweightingDataHandler(rawDataAndMetaInfo);
//        MomentsReweighter reweighter(rawDataAndMetaInfo);
//        std::string outputFileName = "testWritingNewPoints";
//        std::ofstream outputFile;
//        outputFile.open(outputFileName.c_str());
//        outputFile << "Hello!";
//        outputFile.close();
//        BOOST_REQUIRE_MESSAGE(boost::filesystem::exists( outputFileName ) == true, "Something bad happened creating a file!");
//        BOOST_REQUIRE_THROW(reweightingDataHandler.writeNewPointsToFileWithLogZ(reweighter, outputFileName), std::invalid_argument);
//        boost::filesystem::remove(outputFileName);
//        std::vector<double> newLogZ = reweighter.getLogZAtNewPoints();
//        reweightingDataHandler.writeNewPointsToFileWithLogZ(reweighter, outputFileName);
//        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
//        if(boost::filesystem::exists(outputFileName))
//            boost::filesystem::remove(outputFileName);
//    }
//
//BOOST_AUTO_TEST_SUITE_END()


