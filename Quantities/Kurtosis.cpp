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

Kurtosis::Kurtosis() : QuantityAbstract() {}

Kurtosis::Kurtosis(DataSample& dataSample, BinningParameters parameters, bool isMeanZero) : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Kurtosis::Kurtosis(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

void Kurtosis::printCorrectBinningInformation(const BinningParameters& parameters)
{
    printBinningInformation(parameters, constants::observableName<Kurtosis>);
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
    return isMeanZero ? constants::neededMomentsWithZeroMean<Kurtosis> : constants::neededMoments<Kurtosis>;
}
