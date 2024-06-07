/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015,2020-2021 Alessandro Sciarra
 *  Copyright (c) 2019 David Leemueller
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

#ifndef REWEIGHTERIO_HPP_
#define REWEIGHTERIO_HPP_

#include "../Parameters/LqcdReweightingParameters.hpp"
#include "SimulationDataContainer.hpp"
#include <optional>

class Binsizes {
  public:
    Binsizes() : binsizes(std::map<int, int>()){};
    int& operator[](const unsigned int& whichMoment) { return binsizes[whichMoment]; };
    int at(const unsigned int& whichMoment)
    {
        try {
            return binsizes.at(whichMoment);
        } catch (std::out_of_range& exceptionThrown) {
            return getDefaultValue();
        }
    };
    std::vector<int> at(const std::initializer_list<unsigned int>& whichMoments)
    {
        std::vector<int> result;
        for (auto i : whichMoments)
            result.push_back(this->at(i));
        return result;
    };
    bool empty() { return binsizes.empty(); };
    int getDefaultValue()
    {
        try {
            binsizes.at(-1);
        } catch (std::out_of_range& exceptionThrown) {
            throw std::invalid_argument(
                "Default value UNSET for Binsizes object! Hint: if performing Reweighting, is the configuration file correct?");
        }
        return binsizes[-1];
    };
    void setDefaultValue(const int& valueIn) { binsizes[-1] = valueIn; };
    void print()
    {
        for (auto elem : binsizes)
            std::cout << "Binsize[" << elem.first << "] = " << elem.second << "\n";
    };
    // TODO: Implement operator == between Binsizes object and remove this method that has been created only for testing purposes!
    std::map<int, int> getMap() { return binsizes; };

  private:
    std::map<int, int> binsizes;
};

class ReweighterIO {
    friend class Reweighter;
    friend class ReweighterIOTester;

  public:
    ReweighterIO() = delete;
    /*
     * TODO:
     * ATTENTION: The ctor MUST take an argument of type ReweightingParameters not the particular
     *            case LqcdReweightingParameters. It is temporary in order to make everything work!
     */
    ReweighterIO(LqcdReweightingParameters parameters);
    // Output to file
    // void writeNewConfigurationFileWithMetaparameters(MomentsReweighter reweighter, std::string newConfigFileName = "");
    // void writeNewPointsToFileWithLogZ(MomentsReweighter reweighter, std::string outputFileName = "logZAtNewPoints");
  private:
    SimulationDataContainer readFromFileDataContainer;
    bool isMeanKnownToBeZero;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    unsigned int numberOfMultipleColumns;
    std::vector<std::string> namesOfParametersIgnoringMetaParameters;  // just vector of string because the simulation parameters are the
                                                                       // same for ALL data files!
    std::vector<std::vector<realFloat>> valuesOfSimulationParametersIgnoringMetaParameters;
    // Specific member for each meta parameter
    std::vector<Binsizes> valuesOfSpecifiedBinsizes;
    std::vector<realFloat> valuesOfSpecifiedLogZ;
    ErrorCalculationMethod errorMethod;
    std::shared_ptr<int> bootstrapNumber;
    bool deactivateReweightingForProbabilityDistribution;
    realFloat binsizeProbabilityDistribution;
    std::optional<SimulationDataContainer> auxiliaryData;
};

#endif /* REWEIGHTERIO_HPP_ */
