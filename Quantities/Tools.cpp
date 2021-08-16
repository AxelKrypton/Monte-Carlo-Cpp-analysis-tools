/*
 *
 *  Copyright (c) 2020-2021 Alessandro Sciarra
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

#include "Tools.hpp"

#include "../IO/io_utilities.hpp"

#include <iomanip>
#include <numeric>

Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable)
{
    Parameters tmp = parameters;
    if (observable == constants::observableName<Mean>) {
        tmp.binsize = tmp.binsizeMoments[1];
        tmp.numberOfBins = tmp.numberOfBinsMoments[1];
    } else if (observable == constants::observableName<Variance>) {
        tmp.binsize = tmp.binsizeCentralMoments[2];
        tmp.numberOfBins = tmp.numberOfBinsCentralMoments[2];
    } else if (observable == constants::observableName<Skewness>) {
        tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[3]);
        tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[3]);
    } else if (observable == constants::observableName<Kurtosis>) {
        tmp.binsize = std::max(tmp.binsizeCentralMoments[2], tmp.binsizeCentralMoments[4]);
        tmp.numberOfBins = std::min(tmp.numberOfBinsCentralMoments[2], tmp.numberOfBinsCentralMoments[4]);
    } else {
        throw std::invalid_argument("Unknown observable in buildLocalParametersWithCorrectBinningInformation function!");
    }
    return tmp;
}

static void warnIfDiscardedElements(const BinningParameters& parameters, int elementsOfSample)
{
    std::string binningMessage;
    if (parameters.useNumberOfBins)
        binningMessage = "Number of bins (";
    else
        binningMessage = "Binsize (";
    binningMessage += std::to_string(parameters.number) + ")";
    int discardedElements = elementsOfSample % parameters.number;
    if (discardedElements != 0) {
        std::ios oldState(nullptr);
        oldState.copyfmt(std::cout);
        std::cout << "# " << Color::FG_LIGHT_YELLOW << Font::BOLD << "WARNING: " << Font::NO_BOLD << binningMessage
                  << " does not divide the sample size (" << elementsOfSample << ").\n"
                  << Color::DEFAULT << "#          " << Color::FG_LIGHT_YELLOW << "Last " << Color::FG_LIGHT_RED << Font::BOLD
                  << discardedElements << Font::NO_BOLD << Color::FG_LIGHT_YELLOW << " values (" << std::fixed << std::setprecision(2)
                  << 100.0 * parameters.number / elementsOfSample << "%) will be discarded!" << Color::DEFAULT << "\n";
        std::cout.copyfmt(oldState);
    }
}

void printBinningInformation(const BinningParameters& parameters, std::string observable, int elementsOfSample)
{
    std::cout << "# Analzying " << Color::observables.at(observable) << observable << Color::DEFAULT << "...\n";
    if (parameters.useNumberOfBins) {
        // std::cout << parameters.number << " as number of bins!\n";
        std::cout << "# Number of bins = " << parameters.number << "\n# Binsize = " << elementsOfSample / parameters.number << "\n";
    } else {
        // std::cout << parameters.number << " as binsize!\n";
        std::cout << "# Binsize = " << parameters.number << "\n# Number of bins = " << elementsOfSample / parameters.number << "\n";
    }
    warnIfDiscardedElements(parameters, elementsOfSample);
}

static realFloat getPowerSum(const std::vector<realFloat>& multipleEstimates, const unsigned int N)
{
    return std::accumulate(multipleEstimates.begin(), multipleEstimates.end(), realFloat{},
                           [N](realFloat partialSum, realFloat x) { return partialSum + std::pow(x, N); });
}

realFloat getUnbiasEstimateOfNthMomentPerTrajectory(const std::vector<realFloat>& multipleEstimates, const unsigned int N)
{
    if (multipleEstimates.size() < N)
        throw std::logic_error("At least as many estimates as the moment are needed for an unbiased estimate!");

    std::vector<realFloat> powerSums(N);
    realFloat normalization = 1.0;
    for (auto i = 0U; i < N; i++) {
        powerSums[i] = getPowerSum(multipleEstimates, i + 1);
        normalization *= multipleEstimates.size() - i;
    }
    realFloat result;
    switch (N) {
        case 1:
            result = powerSums[0];
            break;
        case 2:
            result = (std::pow(powerSums[0], 2) - powerSums[1]);
            break;
        case 3:
            result = (std::pow(powerSums[0], 3) - 3. * powerSums[0] * powerSums[1] + 2. * powerSums[2]);
            break;
        case 4:
            result = (std::pow(powerSums[0], 4) + 8. * powerSums[0] * powerSums[2] + 3. * std::pow(powerSums[1], 2)
                      - 6. * powerSums[1] * std::pow(powerSums[0], 2) - 6. * powerSums[3]);
            break;
        default:
            throw std::invalid_argument("Asked for an unbias estimate of a not implemented moment.");
    }
    return result / normalization;
}
