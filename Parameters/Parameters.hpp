/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2015,2018,2020-2021 Alessandro Sciarra
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

#include "../Quantities/Constants.hpp"
#include "../types.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <type_traits>

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

class QuantityAttributes {
  public:
    bool isMeanZero = false;
    bool useMultipleEstimates = false;
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
    QuantityAttributes getAnalysisOptions() const;
    template<typename T> BinningParameters getBinningParametersForAnalysis() const
    {
        BinningParameters returnValue;
        returnValue.performBinning = ! doNotUseBinning;
        returnValue.binningMustFitDataSample = binningMustFitDataSampleSize;
        returnValue.adjustDataSample = adjustDataSampleSizeToBinning;
        returnValue.useNumberOfBins = useNumberOfBinsForBinning;
        returnValue.number = getNumberOfBinsToBeUsed<T>();
        return returnValue;
    }

  private:
    void printParameters();
    void setBinningTypeParameter(bool valueIn);
    void checkParsedArguments(po::variables_map& vm, po::options_description& desc);
    void parseBinningInformationForMoments(std::vector<int>& vectorWithBinningInformations, int defaultValue);
    template<typename T> int getNumberOfBinsToBeUsed() const
    {
        std::initializer_list<unsigned int> moments;
        if (numberOfColumnsToBeConsidered == 1)
            moments = constants::neededMomentsUnexpanded<T>;
        else if (isMeanKnownToBeZero)
            moments = constants::neededMomentsExpandedWithZeroMean<T>;
        else
            moments = constants::neededMomentsExpanded<T>;
        std::vector<int> selectedValues(moments.size());
        const std::vector<int>& numbersToUse = (numberOfColumnsToBeConsidered == 1 && std::is_same<T, Mean>::value == false)
                                                   ? ((useNumberOfBinsForBinning) ? numberOfBinsCentralMoments : binsizeCentralMoments)
                                                   : ((useNumberOfBinsForBinning) ? numberOfBinsMoments : binsizeMoments);

        std::transform(moments.begin(), moments.end(), selectedValues.begin(), [numbersToUse](size_t pos) { return numbersToUse[pos]; });

        return (useNumberOfBinsForBinning) ? *std::min_element(selectedValues.begin(), selectedValues.end())
                                           : *std::max_element(selectedValues.begin(), selectedValues.end());
    }
};

#endif /* PARAMETERS_HPP_ */
