/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2016,2018,2020 Alessandro Sciarra
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

#ifndef LQCDREWEIGHTINGPARAMETERS_HPP_
#define LQCDREWEIGHTINGPARAMETERS_HPP_

#include "../types.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "iostream"
namespace po = boost::program_options;

// TODO: Merge with other Parameters class

class ReweightingParameters {
    /*
     * TODO: implement! Here the methods for a general reweighting in N par should be put.
     *       Then the LqcdReweightingParameters should be a particular case of this class.
     *       One idea is to make some getters here be overwritten by those of the child class.
     *       For example here getNewBetaRange_low should return a std::vector<realFloat> and
     *       in LqcdReweightingParameters just one realFloat. Actually I do not know if this can
     *       be done overwriting methods, maybe a class template would be better. For the moment
     *       leave like that since we can use polymorphism in the Reweighter class constructor!
     */
};

class LqcdReweightingParameters : ReweightingParameters {
  public:
    LqcdReweightingParameters(int argc, const char** argv);

    void printParameters();

    std::string getInputfile();
    std::string getOutputfilePrefix();
    unsigned int getNumberOfNewBetaPoints();
    realFloat getNewBetaRange_low();
    realFloat getNewBetaRange_high();
    bool getDeactivateReweightingForMean();
    bool getDeactivateReweightingForVariance();
    bool getDeactivateReweightingForSkewness();
    bool getDeactivateReweightingForKurtosis();
    std::vector<unsigned int> getColumnsToBeReweightedUsingMultipleColumns();
    bool getUseJackknifeAsErrorMethod();
    bool getUseBootstrapAsErrorMethod();
    bool getUseSimulatedPointsAsNewPoints();
    bool getPrintEstimatorsToFile();
    int getNumberOfBootstrapResample();
    realFloat getWeightPrecision();
    bool getIsMeanKnownToBeZero();
    unsigned int getNumberOfMultipleColumnsForSingleObservable();
    bool getDeactivateReweightingForProbabilityDistribution();
    realFloat getBinsizeProbabilityDistribution();

    struct parse_aborted {
    };

  private:
    void checkParsedArguments(po::variables_map& vm, po::options_description& desc);

    std::string inputfile;
    std::string outputfilePrefix;
    unsigned int numberOfNewBetaPoints;
    realFloat newBetaRange_low;
    realFloat newBetaRange_high;
    bool deactivateReweightingForMean;
    bool deactivateReweightingForVariance;
    bool deactivateReweightingForSkewness;
    bool deactivateReweightingForKurtosis;
    bool useJackknifeAsErrorMethod;
    bool useBootstrapAsErrorMethod;
    bool useSimulatedPointsAsNewPoints;
    bool printEstimatorsToFile;
    realFloat weightPrecision;
    int numberOfBootstrapResample;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    bool isMeanKnownToBeZero;
    unsigned int numberOfMultipleColumnsForSingleObservable;
    bool deactivateReweightingForProbabilityDistribution;
    realFloat binsizeProbabilityDistribution;
};

#endif
