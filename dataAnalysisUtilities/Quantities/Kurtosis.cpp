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

#include "Kurtosis.hpp"

#include "Tools.hpp"

// Definition of the static member for the linker
const std::initializer_list<unsigned int> Kurtosis::neededMoments = {1, 2, 3, 4};
const std::initializer_list<unsigned int> Kurtosis::neededMomentsWithZeroMean = {2, 4};
const std::string Kurtosis::observableName = "KURTOSIS";
const functionForObservable Kurtosis::functionToCalculateOservableWithZeroMean
    = [](Moments in) -> realFloat { return in[4] / pow(in[2], 2.0); };
const functionForObservable Kurtosis::functionToCalculateOservableWithNonZeroMean = [](Moments in) -> realFloat {
    realFloat x1 = in[1];
    realFloat x2 = in[2];
    realFloat x3 = in[3];
    realFloat x4 = in[4];
    return (x4 - 4 * x3 * x1 + 6 * x2 * x1 * x1 - 3 * x1 * x1 * x1 * x1) / (pow(x2 - x1 * x1, 2.0));
};
const functionForObservable Kurtosis::functionToCalculateOservableWithMultipleEstimates = [](Moments in) -> realFloat {
    realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
    realFloat x2 = in[2];
    realFloat x3 = in[3];
    realFloat x4 = in[4];
    return (x4 - 4 * x3 * firstMoment + 6 * x2 * firstMoment * firstMoment - 3 * firstMoment * firstMoment * firstMoment * firstMoment)
           / (pow(x2 - firstMoment * firstMoment, 2.0));
};
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithZeroMean
    = [](MomentsEstimators in) -> DataSample { return in[4] / (in[2] ^ 2.0); };
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithNonZeroMean = [](MomentsEstimators in) -> DataSample {
    return (in[4] - (4 * in[3] * in[1]) + (6 * in[2] * in[1] * in[1]) - (3 * in[1] * in[1] * in[1] * in[1])) / ((in[2] - (in[1] ^ 2)) ^ 2);
};
const functionForEstimators Kurtosis::functionToBeAppliedToEstimatorsWithMultipleEstimates = [](MomentsEstimators in) -> DataSample {
    DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
    return (in[4] - (4 * in[3] * firstMoment) + (6 * in[2] * firstMoment * firstMoment)
            - (3 * firstMoment * firstMoment * firstMoment * firstMoment))
           / ((in[2] - (firstMoment ^ 2)) ^ 2);
};

Kurtosis::Kurtosis(DataSample& dataSample, Parameters parameters) : ObservableAbstract(parameters.isMeanKnownToBeZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Kurtosis::Kurtosis(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : ObservableAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

Parameters Kurtosis::getLocalParametersWithCorrectBinningInformation(const Parameters& parameters)
{
    return buildLocalParametersWithCorrectBinningInformation(parameters, Kurtosis::observableName);
}

void Kurtosis::printCorrectBinningInformation(const Parameters& parameters)
{
    printBinningInformation(parameters, Kurtosis::observableName);
}

functionForEstimatorsForJackknife Kurtosis::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    if (isMeanZero)
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 2)
                throw std::invalid_argument("Invalid call to Kurtosis function with zero mean for estimators!");
            DataSample ex2 = in[0];  // estimator second moment
            DataSample ex4 = in[1];  // estimator third moment
            return ex4 / (ex2 ^ 2.0);
        };
    else
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 4)
                throw std::invalid_argument("Invalid call to Kurtosis function for estimators!");
            DataSample ex1 = in[0];  // estimator first moment
            DataSample ex2 = in[1];  // estimator second moment
            DataSample ex3 = in[2];  // estimator third moment
            DataSample ex4 = in[3];  // estimator fourth moment
            return (ex4 - (4 * ex3 * ex1) + (6 * ex2 * ex1 * ex1) - (3 * ex1 * ex1 * ex1 * ex1)) / ((ex2 - (ex1 ^ 2)) ^ 2);
        };
}

functionForEstimators Kurtosis::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanZero, useMultipleEstimate);
}

functionForObservable Kurtosis::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Kurtosis>(isMeanZero, useMultipleEstimate);
}

DataSample Kurtosis::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Kurtosis::getNeededMoments()
{
    return isMeanZero ? Kurtosis::neededMomentsWithZeroMean : Kurtosis::neededMoments;
}
