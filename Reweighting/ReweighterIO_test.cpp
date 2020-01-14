/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "ReweighterIO.hpp"

static ReweighterIO createLqcdParameters(std::string filename, bool useJackknife = false)
{
	int numberOfArguments = 3;
	filename = std::string("-f" + filename);
	std::string whichError;
	if(useJackknife)
		whichError = "--useJackknifeAsErrorMethod";
	else
		whichError = "--useBootstrapAsErrorMethod";
	const char * arguments[] = {"foo", filename.c_str(), whichError.c_str()};
	return ReweighterIO(LqcdReweightingParameters(numberOfArguments, arguments));
}

class ReweighterIOTester{
public:
	ReweighterIOTester(std::string filename, bool useJackknife = false) : reweighterIO(createLqcdParameters(filename, useJackknife)){};
	std::vector<std::string> getNamesOfParametersIgnoringMetaParameters(){ return reweighterIO.namesOfParametersIgnoringMetaParameters; };
	std::vector<std::vector<realFloat> > getValuesOfSimulationParametersIgnoringMetaParameters(){ return reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters; };
	std::vector<Binsizes> getValuesOfSpecifiedBinsizes(){ return reweighterIO.valuesOfSpecifiedBinsizes; };
	std::vector<realFloat> getValuesOfSpecifiedLogZ(){ return reweighterIO.valuesOfSpecifiedLogZ; };
	ErrorCalculationMethod getErrorMethod(){ return reweighterIO.errorMethod; };
	int getBootstrapNumber(){ return *(reweighterIO.bootstrapNumber); };
private:
	ReweighterIO reweighterIO;
};


BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        //These tests use configfile without mandatory binsize label since the exception must be thrown before
        std::string fileThatDoesNotExist = "fileThatShouldNotBe";
        std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_3"; //wrong structure
        std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_4"; //correct structure but with two identical set of parameters
        std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_5"; //correct structure but with two identical filenames
        std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_6"; //correct structure but with not existing file inside
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesNotExist), std::exception);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong1), std::exception);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong2), std::exception);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong3), std::exception);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong4), std::exception);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        //Here we use configfile with mandatory binsize label even if sometimes the exception is thrown before
        std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_7"; //correct structure but with datafile with wrong number of columns
        std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_8"; //correct structure but with different parameters name in two lines
        std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_9"; //correct structure but with different number of observables in one file
        std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_10"; //correct structure but with two identical set of reweigthing parameters
        std::string fileThatDoesExistButWrong5 = "GeneralTestFiles/wrong_configfile_11"; //correct structure but without binsize at least on one line
        std::string fileThatDoesExistButWrong6 = "GeneralTestFiles/wrong_configfile_12"; //correct structure but with negative binsize
        std::string fileThatDoesExistButWrong7 = "GeneralTestFiles/wrong_configfile_13"; //correct structure but with different number of observables in two files
        std::string fileThatDoesExistButWrong8 = "GeneralTestFiles/wrong_configfile_14"; //correct structure but with chars in moment binsize
        std::string fileThatDoesExistButWrong9 = "GeneralTestFiles/wrong_configfile_15"; //correct structure but with negative moment in binsize
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong1), std::logic_error);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong2), std::invalid_argument);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong3), std::logic_error);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong4), std::logic_error);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong5), std::invalid_argument);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong6), std::invalid_argument);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong7), std::logic_error);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong8), std::invalid_argument);
        BOOST_REQUIRE_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExistButWrong9), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build3)
    {
        std::string fileThatDoesExist1 = "GeneralTestFiles/simulationDataContainer.configfile_1"; //This test should fail since on some line there is no binsize given!
        std::string fileThatDoesExist2 = "GeneralTestFiles/simulationDataContainer.configfile_2";
        std::string fileThatDoesExist3 = "GeneralTestFiles/simulationDataContainer.configfile_3";
        std::string fileThatDoesExist4 = "RealTestData/configfile_3";
        BOOST_REQUIRE_NO_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExist1, true));
        BOOST_REQUIRE_NO_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExist2, false));
        BOOST_REQUIRE_NO_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExist3, true));
        BOOST_REQUIRE_NO_THROW(ReweighterIOTester reweighterIOTester(fileThatDoesExist4, false));
    }

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_6";
        ReweighterIOTester reweighterIOTester(fileThatDoesExist);
        std::string referenceParName = "beta";
        std::vector<std::vector<realFloat> > referenceParValue(3, std::vector<realFloat>(1));
        referenceParValue[0][0] = 5.355;
        referenceParValue[1][0] = 5.357;
        referenceParValue[2][0] = 5.360;
        BOOST_REQUIRE_EQUAL(reweighterIOTester.getNamesOfParametersIgnoringMetaParameters()[0], referenceParName);
        for(size_t i=0; i<referenceParValue.size(); i++){
        	for(size_t j=0; j<referenceParValue[i].size(); j++){
        		BOOST_REQUIRE_EQUAL(boost::lexical_cast<std::string>(reweighterIOTester.getValuesOfSimulationParametersIgnoringMetaParameters()[i][j]), boost::lexical_cast<std::string>(referenceParValue[i][j]));
        	}
        }
    }

    BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_6";
		ReweighterIOTester reweighterIOTester(fileThatDoesExist, true);
		const ErrorCalculationMethod referenceErrorMethod = jackknife;
		BOOST_REQUIRE_EQUAL(reweighterIOTester.getErrorMethod(), referenceErrorMethod);
	}

    BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_6";
		ReweighterIOTester reweighterIOTester(fileThatDoesExist);
		const ErrorCalculationMethod referenceErrorMethod = bootstrap;
		const int referenceBootstrapResample = 100;
		BOOST_REQUIRE_EQUAL(reweighterIOTester.getErrorMethod(), referenceErrorMethod);
		BOOST_REQUIRE_EQUAL(reweighterIOTester.getBootstrapNumber(), referenceBootstrapResample);
	}

    BOOST_AUTO_TEST_CASE(build7)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_6";
        ReweighterIOTester reweighterIOTester(fileThatDoesExist);
        Binsizes binsizesFile1, binsizesFile2, binsizesFile3;
        binsizesFile1.setDefaultValue(1);
        binsizesFile2[1] = 1;
        binsizesFile2[4] = 5;
        binsizesFile3[1] = 2;
        binsizesFile3[2] = 8;
        std::vector<Binsizes> gottenBinsizes = reweighterIOTester.getValuesOfSpecifiedBinsizes();
        BOOST_REQUIRE_EQUAL(gottenBinsizes.size(), 3);
        BOOST_REQUIRE(gottenBinsizes[0].getMap() == binsizesFile1.getMap());
        BOOST_REQUIRE(gottenBinsizes[1].getMap() == binsizesFile2.getMap());
        BOOST_REQUIRE(gottenBinsizes[2].getMap() == binsizesFile3.getMap());
	}

    BOOST_AUTO_TEST_CASE(build8)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
		ReweighterIOTester reweighterIOTester(fileThatDoesExist);
		std::vector<realFloat> referenceLogZ({NAN, 3.14, NAN});
		std::vector<realFloat> gottenLogZ = reweighterIOTester.getValuesOfSpecifiedLogZ();
		BOOST_REQUIRE_EQUAL(gottenLogZ.size(), 3);
		BOOST_REQUIRE((boost::math::isnan)(gottenLogZ[0]));  //parenthesis around boost::math::isnan crucial otherwise the std lib macro is called!
		BOOST_REQUIRE_EQUAL(boost::lexical_cast<std::string>(gottenLogZ[1]), boost::lexical_cast<std::string>(referenceLogZ[1]));
		BOOST_REQUIRE((boost::math::isnan)(gottenLogZ[2])); //parenthesis around boost::math::isnan crucial otherwise the std lib macro is called!
	}

BOOST_AUTO_TEST_SUITE_END()


/*
 * See comment in ReweighterIO.cpp file regarding these functions to know why they are commented out.
 */

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
//        std::vector<std::pair<realFloat, realFloat> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.8, 1.2)};
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
//        std::vector<realFloat> simulatedLogZ = reweighter->getLogZAtSimulatedPoints();
//        std::string outputFileName = "testWritingConfigFile";
//        reweightingDataHandler.writeNewConfigurationFileWithMetaparameters(*reweighter, outputFileName);
//        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
//        delete reweighter;
//		std::initializer_list<std::string> options2 = {"-f" + outputFileName, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
//													   "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
//        reweighter = new MomentsReweighter(ReweighterTester(options2).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed));
//        for(size_t i=0; i<simulatedLogZ.size(); i++)
//            BOOST_REQUIRE_CLOSE(reweighter->getLogZAtSimulatedPoints()[i], simulatedLogZ[i], realFloatPrecisionInPercent);
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
//        std::vector<realFloat> newLogZ = reweighter.getLogZAtNewPoints();
//        reweightingDataHandler.writeNewPointsToFileWithLogZ(reweighter, outputFileName);
//        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
//        if(boost::filesystem::exists(outputFileName))
//            boost::filesystem::remove(outputFileName);
//    }
//
//BOOST_AUTO_TEST_SUITE_END()



