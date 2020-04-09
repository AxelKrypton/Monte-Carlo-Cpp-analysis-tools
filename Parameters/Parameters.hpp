/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2015,2018,2020 Alessandro Sciarra
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

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "../types.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "iostream"
namespace po = boost::program_options;

class BinningParameters {
  public:
    bool performBinning = false;
    bool binningMustFitDataSample = false;
    bool adjustDataSample = false;
    bool useNumberOfBins = false;
    int number = 0;  // Meaningless since by default binning is not done
};

class Parameters {
  public:
    Parameters(int argc, const char** argv);
    Parameters(std::vector<std::string> argv);

    struct parse_aborted {
    };
    std::string file;
    std::string analysisOutputFilePrefix;
    std::string analysisOutputFilePostfix;
    int binsize;
    std::vector<int> binsizeMoments;
    std::vector<int> binsizeCentralMoments;
    int numberOfBins;
    std::vector<int> numberOfBinsMoments;
    std::vector<int> numberOfBinsCentralMoments;
    int numberOfBinsForAutocorrelation;
    int timeMaxAutocorrelationFunction;
    int offset;
    int column;
    int numberOfColumnsToBeConsidered;
    bool isMeanKnownToBeZero;
    bool doNotUseBinning;
    bool useNumberOfBinsForBinning;
    bool calcAutocorrelation;
    bool doNotAnalyzeMean;
    bool doNotAnalyzeVariance;
    bool doNotAnalyzeSkewness;
    bool doNotAnalyzeKurtosis;
    bool binningMustFitDataSampleSize;
    bool adjustDataSampleSizeToBinning;
    BinningParameters getBinningParametersForObservablesAnalysis(std::string observable) const;

  private:
    void printParameters();
    void setBinningTypeParameter(bool valueIn);
    void checkParsedArguments(po::variables_map& vm, po::options_description& desc);
    void parseBinningInformationForMoments(std::vector<int>& vectorWithBinningInformations, const int defaultValue);
};

#endif /* PARAMETERS_HPP_ */
