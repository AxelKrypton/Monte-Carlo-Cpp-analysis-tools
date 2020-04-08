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

#pragma once

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../types.hpp"
#include "Constants.hpp"
#include "Kurtosis.hpp"
#include "Mean.hpp"
#include "Skewness.hpp"
#include "Variance.hpp"

#include <type_traits>

Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable);
void printBinningInformation(const BinningParameters& parameters, std::string observable);

// TODO: Implement the following function in a general way
template<typename T> T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>& estimates, const int power)
{
    if (estimates.size() != 4)
        throw std::invalid_argument("So far only 4 multiple estimates are allowed in quantities calculations! Aborting...");

    switch (power) {
        case 1:
            return (estimates[0] + estimates[1] + estimates[2] + estimates[3]) / (realFloat)4.0;
        case 2:
            return ((estimates[0] * estimates[1]) + (estimates[0] * estimates[2]) + (estimates[0] * estimates[3])
                    + (estimates[1] * estimates[2]) + (estimates[1] * estimates[3]) + (estimates[2] * estimates[3]))
                   / (realFloat)6.0;
        case 3:
            return ((estimates[0] * estimates[1] * estimates[2]) + (estimates[0] * estimates[1] * estimates[3])
                    + (estimates[0] * estimates[2] * estimates[3]) + (estimates[1] * estimates[2] * estimates[3]))
                   / (realFloat)4.0;
        case 4:
            return (estimates[0] * estimates[1] * estimates[2] * estimates[3]);
        default:
            throw std::invalid_argument(
                "Asked for a not implemented power in \"getPowerOfFirstMomentUsingSeveralEstimate\" function! Aborting...");
    }
}

// TODO: Think whether it is possible to unify the following two templates in only one
template<typename OBSERVABLE>
functionForObservable pickUpCorrectFunctionForObservable(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero) {
        if (std::is_same_v<OBSERVABLE, Mean>)
            throw std::logic_error("Attempt to get function to calculate mean but isMeanZero==true!");
        else
            return constants::functionToCalculateQuantityWithZeroMean<OBSERVABLE>;
    } else {
        if (useMultipleEstimate)
            return constants::functionToCalculateQuantityWithMultipleEstimates<OBSERVABLE>;
        else
            return constants::functionToCalculateQuantityWithNonZeroMean<OBSERVABLE>;
    }
}
template<typename OBSERVABLE> functionForEstimators pickUpCorrectFunctionForEstimator(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero) {
        if (std::is_same_v<OBSERVABLE, Mean>)
            throw std::logic_error("Attempt to get function to calculate mean estimators but isMeanZero==true!");
        else
            return constants::functionToBeAppliedToEstimatorsWithZeroMean<OBSERVABLE>;
    } else {
        if (useMultipleEstimate)
            return constants::functionToBeAppliedToEstimatorsWithMultipleEstimates<OBSERVABLE>;
        else
            return constants::functionToBeAppliedToEstimatorsWithNonZeroMean<OBSERVABLE>;
    }
}
