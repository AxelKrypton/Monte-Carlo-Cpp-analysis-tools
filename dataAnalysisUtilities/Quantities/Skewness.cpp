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

#include "Skewness.hpp"

#include "Tools.hpp"

// Definition of the static member for the linker
const std::initializer_list<unsigned int> Skewness::neededMoments = {1, 2, 3};
const std::initializer_list<unsigned int> Skewness::neededMomentsWithZeroMean = {2, 3};
const std::string Skewness::observableName = "SKEWNESS";
const functionForObservable Skewness::functionToCalculateOservableWithZeroMean
    = [](Moments in) -> realFloat { return in[3] / pow(in[2], 1.5); };
const functionForObservable Skewness::functionToCalculateOservableWithNonZeroMean = [](Moments in) -> realFloat {
    realFloat x1 = in[1];
    realFloat x2 = in[2];
    realFloat x3 = in[3];
    return (x3 - 3 * x2 * x1 + 2 * x1 * x1 * x1) / (pow(x2 - x1 * x1, 1.5));
};
const functionForObservable Skewness::functionToCalculateOservableWithMultipleEstimates = [](Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    realFloat x2 = in[2];
    realFloat x3 = in[3];
    return (x3 - 3 * x2 * firstMoment + 2 * firstMoment * firstMoment * firstMoment) / (pow(x2 - firstMoment * firstMoment, 1.5));
};
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithZeroMean
    = [](MomentsEstimators in) -> DataSample { return in[3] / (in[2] ^ 1.5); };
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithNonZeroMean
    = [](MomentsEstimators in) -> DataSample { return (in[3] - ((3 * in[2]) * in[1]) + (2 * (in[1] ^ 3))) / ((in[2] - (in[1] ^ 2)) ^ 1.5); };
const functionForEstimators Skewness::functionToBeAppliedToEstimatorsWithMultipleEstimates = [](MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return (in[3] - ((3 * in[2]) * firstMoment) + (2 * firstMoment * firstMoment * firstMoment)) / ((in[2] - firstMoment * firstMoment) ^ 1.5);
};

Skewness::Skewness(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Skewness::Skewness(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : ObservableAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Skewness::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
    return buildLocalParametersWithCorrectBinningInformation(parameters, Skewness::observableName);
}

void Skewness::printCorrectBinningInformation(const Parameters& parameters)
{
    printBinningInformation(parameters, Skewness::observableName);
}

functionForEstimatorsForJackknife Skewness::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    if (isMeanZero)
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 2)
                throw std::invalid_argument("Invalid call to Skewness function with zero mean for estimators!");
            DataSample ex2 = in[0];  // estimator second moment
            DataSample ex3 = in[1];  // estimator third moment
            return ex3 / (ex2 ^ 1.5);
        };
    else
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 3)
                throw std::invalid_argument("Invalid call to Skewness function for estimators!");
            DataSample ex1 = in[0];  // estimator first moment
            DataSample ex2 = in[1];  // estimator second moment
            DataSample ex3 = in[2];  // estimator third moment
            return (ex3 - ((3 * ex2) * ex1) + (2 * (ex1 ^ 3))) / ((ex2 - (ex1 ^ 2)) ^ 1.5);
        };
}

functionForEstimators Skewness::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanZero, useMultipleEstimate);
}

functionForObservable Skewness::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Skewness>(isMeanZero, useMultipleEstimate);
}

DataSample Skewness::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Skewness::getNeededMoments()
{
    return isMeanZero ? Skewness::neededMomentsWithZeroMean : Skewness::neededMoments;
}
