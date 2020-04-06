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

void printBinningInformation(const BinningParameters& parameters, std::string observable)
{
    std::cout << "### Performing binning in " << observable << " calculation using ";
    if (parameters.useNumberOfBins)
        std::cout << parameters.number << " as number of bins!\n";
    else
        std::cout << parameters.number << " as binsize!\n";
}
