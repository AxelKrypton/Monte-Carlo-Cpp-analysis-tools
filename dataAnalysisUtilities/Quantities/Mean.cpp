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

#include "Mean.hpp"

#include "Tools.hpp"

// Definition of the static member for the linker
const std::initializer_list<unsigned int> Mean::neededMoments = {1};
const std::initializer_list<unsigned int> Mean::neededMomentsWithZeroMean = {};
const std::string Mean::observableName = "MEAN";
const functionForObservable Mean::functionToCalculateQuantityWithNonZeroMean = [](Moments in) -> realFloat { return in[1]; };
const functionForObservable Mean::functionToCalculateQuantityWithMultipleEstimates
    = [](Moments in) -> realFloat { return getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1); };
const functionForEstimators Mean::functionToBeAppliedToEstimatorsWithNonZeroMean = [](MomentsEstimators in) -> DataSample { return in[1]; };
const functionForEstimators Mean::functionToBeAppliedToEstimatorsWithMultipleEstimates
    = [](MomentsEstimators in) -> DataSample { return getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1); };

Mean::Mean() : QuantityAbstract() {}

Mean::Mean(DataSample& dataSample, Parameters parameters) : QuantityAbstract(parameters.isMeanKnownToBeZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Mean::Mean(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    if (isMeanZero)
        value = EstimateAndError(0.0, 0.0);
    else
        calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Mean::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
    return buildLocalParametersWithCorrectBinningInformation(parameters, Mean::observableName);
}

void Mean::printCorrectBinningInformation(const Parameters& parameters)
{
    printBinningInformation(parameters, Mean::observableName);
}

functionForEstimatorsForJackknife Mean::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    if (isMeanZero)
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 0)
                throw std::invalid_argument("Invalid call to Mean function with zero mean for estimators!");
            return DataSample(std::valarray<realFloat>(0.0, in[0].getNumberOfElements()));
        };
    else
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 1)
                throw std::invalid_argument("Invalid call to Mean function for estimators!");
            return in[0];
        };
}

functionForEstimators Mean::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanZero, useMultipleEstimate);
}

functionForObservable Mean::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Mean>(isMeanZero, useMultipleEstimate);
}

DataSample Mean::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Mean::getNeededMoments()
{
    return isMeanZero ? Mean::neededMomentsWithZeroMean : Mean::neededMoments;
}
