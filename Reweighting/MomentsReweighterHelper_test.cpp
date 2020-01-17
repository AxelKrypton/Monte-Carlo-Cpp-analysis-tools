/*
 *
 *  Copyright (c) 2014-2015,2019-2020 Alessandro Sciarra
 *  Copyright (c) 2015 Christopher Pinke
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
#define BOOST_TEST_MODULE MomentsReweightingDataHandler
#include "MomentsReweighterHelper.hpp"

#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"  // For realFloatPrecisionInPercent
#include "MomentsReweighter.hpp"

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

class MomentsReweighterHelperTest {
  public:
    MomentsReweighterHelperTest(std::initializer_list<std::string> options, std::vector<unsigned int> momentsToBeReweighted = {},
                                std::vector<int> binsizesToBeUsed = {}, bool reweightProbabilityDistribution = true)
        : momentsReweighterHelper(ReweighterTester(options).getRawDataForReweightingAndMetainformation(
            momentsToBeReweighted, binsizesToBeUsed, reweightProbabilityDistribution))
    {
    }
    MomentsReweighterHelperTest(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformation)
        : momentsReweighterHelper(rawDataForReweightingAndMetainformation)
    {
    }
    std::vector<int> getNumberOfBinsToBeUsed() { return momentsReweighterHelper.numberOfBinsToBeUsed; }
    int getNumberOfObsGivenAsInput() { return momentsReweighterHelper.numberOfObservablesGivenAsInput; }
    int getNumberOfObsToBeRew() { return momentsReweighterHelper.numberOfObservablesToBeReweighted; }
    SimulationDataContainer getRawData() { return momentsReweighterHelper.simulationRawDataContainer; }

  private:
    MomentsReweighterHelper momentsReweighterHelper;
};

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1}, {100, 100, 100});
        //        const int referenceNumberOfBins = 16;
        //        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_3";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1}, {100, 100, 100});
        std::vector<int> referenceNumberOfBins{23, 16, 25};
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins[0]);
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[1], referenceNumberOfBins[1]);
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfBinsToBeUsed()[2], referenceNumberOfBins[2]);
    }

    BOOST_AUTO_TEST_CASE(build3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_6";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--obsMultipleColumns=1"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1, 2, 3, 4}, {100, 100, 100});
        const int referenceNumberObsGiven = 4;
        const int referenceNumberObsRew = 19;
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsGivenAsInput(), referenceNumberObsGiven);
        BOOST_REQUIRE_EQUAL(reweightingDataHandler.getNumberOfObsToBeRew(), referenceNumberObsRew);
        std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--obsMultipleColumns=1", "3"};
        BOOST_REQUIRE_THROW(
            MomentsReweighterHelperTest reweightingDataHandler2(options2, {1, 2, 3, 4}, {100, 100, 100}), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
        MomentsReweighterHelperTest reweightingDataHandler(options, {1, 2, 3, 4}, {1, 1, 1});
        std::vector<int> fileLines{2304, 1653, 2584};
        SimulationDataContainer rawData = reweightingDataHandler.getRawData();
        for (int i = 0; i < rawData.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(rawData[i][0].getNumberOfElements(), fileLines[i]);
    }

BOOST_AUTO_TEST_SUITE_END()
