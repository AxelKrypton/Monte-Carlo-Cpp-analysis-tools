/*
 *
 *  Copyright (c) 2020 Alessandro Sciarra
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
