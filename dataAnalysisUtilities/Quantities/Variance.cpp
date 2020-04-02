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

#include "Variance.hpp"

#include "Tools.hpp"

// Definition of the static member for the linker
const std::initializer_list<unsigned int> Variance::neededMoments = {1, 2};
const std::initializer_list<unsigned int> Variance::neededMomentsWithZeroMean = {2};
const std::string Variance::observableName = "VARIANCE";
const functionForObservable Variance::functionToCalculateOservableWithZeroMean = [](Moments in) -> realFloat { return in[2]; };
const functionForObservable Variance::functionToCalculateOservableWithNonZeroMean
    = [](Moments in) -> realFloat { return in[2] - in[1] * in[1]; };
const functionForObservable Variance::functionToCalculateOservableWithMultipleEstimates = [](Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    return in[2] - firstMoment * firstMoment;
};
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithZeroMean = [](MomentsEstimators in) -> DataSample { return in[2]; };
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithNonZeroMean
    = [](MomentsEstimators in) -> DataSample { return in[2] - (in[1] ^ 2); };
const functionForEstimators Variance::functionToBeAppliedToEstimatorsWithMultipleEstimates = [](MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return in[2] - firstMoment * firstMoment;
};

Variance::Variance(DataSample& dataSample, Parameters parameters) : QuantityAbstract(parameters.isMeanKnownToBeZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Variance::Variance(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Variance::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
    return buildLocalParametersWithCorrectBinningInformation(parameters, Variance::observableName);
}

void Variance::printCorrectBinningInformation(const Parameters& parameters)
{
    printBinningInformation(parameters, Variance::observableName);
}

functionForEstimatorsForJackknife Variance::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    if (isMeanZero)
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 1)
                throw std::invalid_argument("Invalid call to Variance function with zero mean for estimators!");
            return in[0];
        };
    else
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 2)
                throw std::invalid_argument("Invalid call to Variance function for estimators!");
            DataSample ex1 = in[0];  // estimator first moment
            DataSample ex2 = in[1];  // estimator second moment
            return ex2 - (ex1 ^ 2);
        };
}

functionForEstimators Variance::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanZero, useMultipleEstimate);
}

functionForObservable Variance::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Variance>(isMeanZero, useMultipleEstimate);
}

DataSample Variance::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Variance::getNeededMoments()
{
    return isMeanZero ? Variance::neededMomentsWithZeroMean : Variance::neededMoments;
}
