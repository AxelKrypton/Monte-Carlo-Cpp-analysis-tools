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
#include "Constants.hpp"
#include "Kurtosis.hpp"
#include "Mean.hpp"
#include "Skewness.hpp"
#include "Variance.hpp"

#include <type_traits>

Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable);
void printBinningInformation(const BinningParameters& parameters, std::string observable, int elementsOfSample);
realFloat getUnbiasEstimateOfNthMomentPerTrajectory(const std::vector<realFloat>& multipleEstimates, const unsigned int N);

// TODO: Think whether it is possible to unify the following two templates in only one
template<typename OBSERVABLE>
functionForObservable pickUpCorrectFunctionForObservable(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero) {
        if constexpr (std::is_same_v<OBSERVABLE, Mean>)
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
        if constexpr (std::is_same_v<OBSERVABLE, Mean>)
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
