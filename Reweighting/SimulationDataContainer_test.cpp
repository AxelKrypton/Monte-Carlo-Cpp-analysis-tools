/*
 *
 *  Copyright (c) 2014-2015,2019-2021 Alessandro Sciarra
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
#define BOOST_TEST_MODULE SimulationDataContainer
#include "SimulationDataContainer.hpp"

#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"  // For realFloatPrecisionInPercent

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1) { BOOST_REQUIRE_THROW(SimulationDataContainer simDataCont, std::invalid_argument); }

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
        const int numberOfSimulationParametersGivenInInputFileOnOtherLines = 4;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfSimulationParameters(0), numberOfSimulationParametersGivenInInputFileOnFirstLine);
        for (int i = 1; i < simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont.getNumberOfSimulationParameters(i), numberOfSimulationParametersGivenInInputFileOnOtherLines);
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
        BOOST_REQUIRE_THROW(simDataCont[numberOfDataFilesGivenInInputFile + 1], std::out_of_range);
    }

    BOOST_AUTO_TEST_CASE(AccessOperator3)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        int numberOfDataFilesGivenInInputFile = simDataCont.getNumberOfDatafiles();
        BOOST_REQUIRE_NO_THROW(SimulationData simData = simDataCont[numberOfDataFilesGivenInInputFile - 1]);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UncorrelatedContainer)

    BOOST_AUTO_TEST_CASE(UncorrelatedContainer1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        const int numberOfBins = 100;
        std::vector<int> referenceEntriesToBeLeftOut(3);
        referenceEntriesToBeLeftOut[0] = 4;
        referenceEntriesToBeLeftOut[1] = 53;
        referenceEntriesToBeLeftOut[2] = 84;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<int> numberOfBinsVec(simDataCont.getNumberOfDatafiles(), numberOfBins);
        SimulationDataContainer uncorrObject = simDataCont.getUncorrelatedSimulationDataSet(numberOfBinsVec, jackknife);
        std::vector<int> entriesLeftOut = simDataCont.getNumberOfEntriesLeftOut(numberOfBinsVec);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_EQUAL(entriesLeftOut[i], referenceEntriesToBeLeftOut[i]);
            BOOST_REQUIRE_EQUAL(uncorrObject[i][0].getNumberOfElements(), numberOfBins);
        }
    }

    BOOST_AUTO_TEST_CASE(UncorrelatedContainer2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        const int numberOfBins = 100;
        std::vector<int> referenceEntriesToBeLeftOut(3);
        referenceEntriesToBeLeftOut[0] = 4;
        referenceEntriesToBeLeftOut[1] = 53;
        referenceEntriesToBeLeftOut[2] = 84;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<int> numberOfBinsVec(simDataCont.getNumberOfDatafiles(), numberOfBins);
        std::default_random_engine generator;
        BOOST_REQUIRE_THROW(simDataCont.getUncorrelatedSimulationDataSet(numberOfBinsVec, bootstrap), std::invalid_argument);
        SimulationDataContainer uncorrObject = simDataCont.getUncorrelatedSimulationDataSet(numberOfBinsVec, bootstrap, &generator);
        std::vector<int> entriesLeftOut = simDataCont.getNumberOfEntriesLeftOut(numberOfBinsVec);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_EQUAL(entriesLeftOut[i], referenceEntriesToBeLeftOut[i]);
            BOOST_REQUIRE_EQUAL(uncorrObject[i][0].getNumberOfElements(), numberOfBins);
        }
    }

    BOOST_AUTO_TEST_CASE(UncorrelatedContainer3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<int> referenceEntriesToBeLeftOut(3);
        referenceEntriesToBeLeftOut[0] = 4;
        referenceEntriesToBeLeftOut[1] = 13;
        referenceEntriesToBeLeftOut[2] = 4;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<int> numberOfBins(simDataCont.getNumberOfDatafiles());
        numberOfBins[0] = 10;
        numberOfBins[1] = 20;
        numberOfBins[2] = 5;
        std::default_random_engine generator;
        SimulationDataContainer uncorrObject = simDataCont.getUncorrelatedSimulationDataSet(numberOfBins, bootstrap, &generator);
        std::vector<int> entriesLeftOut = simDataCont.getNumberOfEntriesLeftOut(numberOfBins);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_EQUAL(entriesLeftOut[i], referenceEntriesToBeLeftOut[i]);
            BOOST_REQUIRE_EQUAL(uncorrObject[i][0].getNumberOfElements(), numberOfBins[i]);
        }
    }

    BOOST_AUTO_TEST_CASE(UncorrelatedContainer4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<int> referenceEntriesToBeLeftOut(3);
        referenceEntriesToBeLeftOut[0] = 0;
        referenceEntriesToBeLeftOut[1] = 0;
        referenceEntriesToBeLeftOut[2] = 0;
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<int> numberOfBins(simDataCont.getNumberOfDatafiles());
        // Force binsize=1 in order to check the drawn data
        numberOfBins[0] = 2304;
        numberOfBins[1] = 1653;
        numberOfBins[2] = 2584;
        std::default_random_engine generator;
        SimulationDataContainer uncorrObject = simDataCont.getUncorrelatedSimulationDataSet(numberOfBins, bootstrap, &generator);
        std::vector<int> entriesLeftOut = simDataCont.getNumberOfEntriesLeftOut(numberOfBins);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_EQUAL(entriesLeftOut[i], referenceEntriesToBeLeftOut[i]);
            BOOST_REQUIRE_EQUAL(uncorrObject[i][0].getNumberOfElements(), numberOfBins[i]);
            for (int j = 0; j < numberOfBins[i]; j++)
                BOOST_REQUIRE_EQUAL(uncorrObject[i][0][j], simDataCont[i][0][j]);
        }
    }

    BOOST_AUTO_TEST_CASE(UncorrelatedContainer5)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_5";
        std::vector<int> referenceEntriesToBeLeftOut(1, 0);
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<int> numberOfBins(simDataCont.getNumberOfDatafiles(), 20);
        std::default_random_engine generator;
        SimulationDataContainer uncorrObject = simDataCont.getUncorrelatedSimulationDataSet(numberOfBins, bootstrap, &generator);
        std::vector<int> entriesLeftOut = simDataCont.getNumberOfEntriesLeftOut(numberOfBins);
        BOOST_REQUIRE_EQUAL(entriesLeftOut[0], referenceEntriesToBeLeftOut[0]);
        for (int j = 0; j < 8; j++) {
            BOOST_REQUIRE_EQUAL(uncorrObject[0][j].getNumberOfElements(), numberOfBins[0]);
        }
        for (int j = 0; j < numberOfBins[0]; j++) {
            // Since the moment is the first, than the picked data must be equal to the original
            BOOST_REQUIRE_EQUAL(uncorrObject[0][2][j], std::pow(uncorrObject[0][0][j], 0));
            BOOST_REQUIRE_EQUAL(uncorrObject[0][3][j], std::pow(uncorrObject[0][0][j], 1));
            BOOST_REQUIRE_EQUAL(uncorrObject[0][4][j], std::pow(uncorrObject[0][0][j], 3));
            BOOST_REQUIRE_EQUAL(uncorrObject[0][5][j], std::pow(uncorrObject[0][1][j], 0));
            BOOST_REQUIRE_EQUAL(uncorrObject[0][6][j], std::pow(uncorrObject[0][1][j], 1));
            BOOST_REQUIRE_EQUAL(uncorrObject[0][7][j], std::pow(uncorrObject[0][1][j], 3));
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(InsertMoments)

    BOOST_AUTO_TEST_CASE(InsertMoments1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{0, 1};
        simDataCont = simDataCont.buildAndGetMomentsPerData(moments, 0, {}, 0);
        const int newNumberOfColumns = 6;
        const realFloat FilesAfter[3][6][3]
            = {{{1.0, 1.0, 1.0}, {1.1, 1.4, 1.7}, {1.0, 1.0, 1.0}, {1.2, 1.5, 1.8}, {1.0, 1.0, 1.0}, {1.3, 1.6, -1.9}},
               {{1.0, 1.0, 1.0}, {2.1, 2.4, 2.7}, {1.0, 1.0, 1.0}, {2.2, 2.5, 2.8}, {1.0, 1.0, 1.0}, {2.3, -2.6, 2.9}},
               {{1.0, 1.0, 1.0}, {3.1, 3.4, 3.7}, {1.0, 1.0, 1.0}, {3.2, 3.5, 3.8}, {1.0, 1.0, 1.0}, {-3.3, 3.6, -3.9}}};

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont[i].getNumberOfDataSample(), newNumberOfColumns);

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++) {
            for (int j = 0; j < newNumberOfColumns; j++) {
                for (int k = 0; k < simDataCont[i][j].getNumberOfElements(); k++) {
                    BOOST_REQUIRE_EQUAL(simDataCont[i][j][k], FilesAfter[i][j][k]);
                }
            }
        }
    }

    BOOST_AUTO_TEST_CASE(InsertMoments2)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_4";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{2, 3, 4};
        std::vector<unsigned int> useMultipleColumns{0};
        simDataCont = simDataCont.buildAndGetMomentsPerData(moments, 0, useMultipleColumns, 4);
        const int newNumberOfColumns = 3;
        const realFloat SecondMoment[3] = {
            0.688333333333333,
            0.648333333333333,
            0.588333333333333,
        };
        const realFloat ThirdMoment[3] = {-21.7715, -27.2085, -33.4555};
        const realFloat FourthMoment[3] = {-163.68, -218.484, -285.1368};
        BOOST_REQUIRE_EQUAL(simDataCont[0].getNumberOfDataSample(), newNumberOfColumns);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_CLOSE(simDataCont[0][0][i], SecondMoment[i], realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][1][i], ThirdMoment[i], realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][2][i], FourthMoment[i], realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(InsertMoments3)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_4";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{2, 1, 4};
        std::vector<unsigned int> useMultipleColumns{0};
        simDataCont = simDataCont.buildAndGetMomentsPerData(moments, 0, useMultipleColumns, 4);
        const int newNumberOfColumns = 3;
        const realFloat FirstMoment[3] = {2.0, 2.1, 2.2};
        const realFloat SecondMoment[3] = {
            0.688333333333333,
            0.648333333333333,
            0.588333333333333,
        };
        const realFloat FourthMoment[3] = {-163.68, -218.484, -285.1368};
        BOOST_REQUIRE_EQUAL(simDataCont[0].getNumberOfDataSample(), newNumberOfColumns);
        for (int i = 0; i < 3; i++) {
            BOOST_REQUIRE_CLOSE(simDataCont[0][0][i], SecondMoment[i], realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][1][i], FirstMoment[i], realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(simDataCont[0][2][i], FourthMoment[i], realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(InsertMoments4)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{0, 1};
        simDataCont = simDataCont.buildAndGetMomentsPerData(moments, 1, {}, 0);
        const int newNumberOfColumns = 5;
        const realFloat FilesAfter[3][5][3] = {{{1.1, 1.4, 1.7}, {1.0, 1.0, 1.0}, {1.2, 1.5, 1.8}, {1.0, 1.0, 1.0}, {1.3, 1.6, -1.9}},
                                               {{2.1, 2.4, 2.7}, {1.0, 1.0, 1.0}, {2.2, 2.5, 2.8}, {1.0, 1.0, 1.0}, {2.3, -2.6, 2.9}},
                                               {{3.1, 3.4, 3.7}, {1.0, 1.0, 1.0}, {3.2, 3.5, 3.8}, {1.0, 1.0, 1.0}, {-3.3, 3.6, -3.9}}};

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont[i].getNumberOfDataSample(), newNumberOfColumns);

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++) {
            for (int j = 0; j < newNumberOfColumns; j++) {
                for (int k = 0; k < simDataCont[i][j].getNumberOfElements(); k++) {
                    BOOST_REQUIRE_EQUAL(simDataCont[i][j][k], FilesAfter[i][j][k]);
                }
            }
        }
    }

    BOOST_AUTO_TEST_CASE(InsertMoments5)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{0, 1};
        simDataCont = simDataCont.buildAndGetMomentsPerData(moments, 2, {}, 0);
        const int newNumberOfColumns = 4;
        const realFloat FilesAfter[3][4][3] = {{{1.1, 1.4, 1.7}, {1.2, 1.5, 1.8}, {1.0, 1.0, 1.0}, {1.3, 1.6, -1.9}},
                                               {{2.1, 2.4, 2.7}, {2.2, 2.5, 2.8}, {1.0, 1.0, 1.0}, {2.3, -2.6, 2.9}},
                                               {{3.1, 3.4, 3.7}, {3.2, 3.5, 3.8}, {1.0, 1.0, 1.0}, {-3.3, 3.6, -3.9}}};

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++)
            BOOST_REQUIRE_EQUAL(simDataCont[i].getNumberOfDataSample(), newNumberOfColumns);

        for (int i = 0; i < simDataCont.getNumberOfDatafiles(); i++) {
            for (int j = 0; j < newNumberOfColumns; j++) {
                for (int k = 0; k < simDataCont[i][j].getNumberOfElements(); k++) {
                    BOOST_REQUIRE_EQUAL(simDataCont[i][j][k], FilesAfter[i][j][k]);
                }
            }
        }
    }

    BOOST_AUTO_TEST_CASE(InsertMoments6)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        SimulationDataContainer simDataCont(fileThatDoesExist);
        std::vector<unsigned int> moments{0, 1};
        BOOST_REQUIRE_THROW(simDataCont.buildAndGetMomentsPerData(moments, 3, {}, 0), std::logic_error);
        BOOST_REQUIRE_THROW(simDataCont.buildAndGetMomentsPerData(moments, 4, {}, 0), std::logic_error);
    }

BOOST_AUTO_TEST_SUITE_END()
