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

#include "../../Parameters/Parameters.hpp"
#include "../../types.hpp"
#include "../DataSample.hpp"
#include "Kurtosis.hpp"
#include "Mean.hpp"
#include "Skewness.hpp"
#include "Variance.hpp"

Parameters buildLocalParametersWithCorrectBinningInformation(const Parameters& parameters, std::string observable);
void printBinningInformation(const Parameters& parameters, std::string observable);

// TODO: Implement the following function in a general way
template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>& estimates, const int power)
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
    if (isMeanZero)
        return OBSERVABLE::functionToCalculateQuantityWithZeroMean;
    else {
        if (useMultipleEstimate)
            return OBSERVABLE::functionToCalculateQuantityWithMultipleEstimates;
        else
            return OBSERVABLE::functionToCalculateQuantityWithNonZeroMean;
    }
}
template<typename OBSERVABLE> functionForEstimators pickUpCorrectFunctionForEstimator(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero)
        return OBSERVABLE::functionToBeAppliedToEstimatorsWithZeroMean;
    else {
        if (useMultipleEstimate)
            return OBSERVABLE::functionToBeAppliedToEstimatorsWithMultipleEstimates;
        else
            return OBSERVABLE::functionToBeAppliedToEstimatorsWithNonZeroMean;
    }
}
// Specialization for Mean case (we want to throw in the zeroMean case!) https://stackoverflow.com/a/10536588
template<> inline functionForObservable pickUpCorrectFunctionForObservable<Mean>(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero)
        throw std::logic_error(
            "The Mean::getFunctionToCalculateObservable method should not be called with isMeanZero==true!! Aborting...");
    else {
        if (useMultipleEstimate)
            return Mean::functionToCalculateQuantityWithMultipleEstimates;
        else
            return Mean::functionToCalculateQuantityWithNonZeroMean;
    }
}
template<> inline functionForEstimators pickUpCorrectFunctionForEstimator<Mean>(const bool isMeanZero, const bool useMultipleEstimate)
{
    if (isMeanZero)
        throw std::logic_error(
            "The Mean::getFunctionToBeAppliedToEstimators method should not be called with isMeanZero==true!! Aborting...");
    else {
        if (useMultipleEstimate)
            return Mean::functionToBeAppliedToEstimatorsWithMultipleEstimates;
        else
            return Mean::functionToBeAppliedToEstimatorsWithNonZeroMean;
    }
}
