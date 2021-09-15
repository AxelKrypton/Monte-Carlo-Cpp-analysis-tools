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

#pragma once

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../types.hpp"

#include <numeric>
#include <type_traits>

Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable);
void printBinningInformation(const BinningParameters& parameters, std::string observable, int elementsOfSample);

template<typename T> T getUnbiasEstimateOfNthMoment(std::vector<T> multipleEstimates, const unsigned int N)
{
    if (N > 4)
        throw std::invalid_argument("Asked for an unbias estimate of a moment larger than 4, not implemented.");
    if (multipleEstimates.size() < N)
        throw std::logic_error("At least as many estimates as the moment are needed for an unbiased estimate!");

    using std::pow;  // T can be realFloat or DataSample  =>  https://stackoverflow.com/a/53125226/14967071
    std::vector<T> powerSums(N);
    realFloat normalization = 1.0;
    for (auto i = 0U; i < N; i++) {
        auto accumulateFunction = [i](T partialSum, T x) { return partialSum + pow(x, i + 1); };
        auto startingValue = T{};
        if constexpr (std::is_same_v<T, DataSample>)
            startingValue = DataSample(multipleEstimates[0].getNumberOfElements());
        powerSums[i] = std::accumulate(multipleEstimates.begin(), multipleEstimates.end(), startingValue, accumulateFunction);
        normalization *= multipleEstimates.size() - i;
    }
    T result;
    switch (N) {
        case 1:
            result = powerSums[0];
            break;
        case 2:
            result = (pow(powerSums[0], 2) - powerSums[1]);
            break;
        case 3:
            result = (pow(powerSums[0], 3) - 3. * powerSums[0] * powerSums[1] + 2. * powerSums[2]);
            break;
        case 4:
            result = (pow(powerSums[0], 4) + 8. * powerSums[0] * powerSums[2] + 3. * pow(powerSums[1], 2)
                      - 6. * powerSums[1] * pow(powerSums[0], 2) - 6. * powerSums[3]);
            break;
        default:
            throw std::runtime_error("Default case of switch should not be entered!");
            break;
    }
    return result / normalization;
}
