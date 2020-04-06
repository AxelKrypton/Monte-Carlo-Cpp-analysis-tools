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

Skewness::Skewness() : QuantityAbstract() {}

Skewness::Skewness(DataSample& dataSample, BinningParameters parameters, bool isMeanZero) : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Skewness::Skewness(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

void Skewness::printCorrectBinningInformation(const BinningParameters& parameters)
{
    printBinningInformation(parameters, constants::observableName<Skewness>);
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
    return isMeanZero ? constants::neededMomentsWithZeroMean<Skewness> : constants::neededMoments<Skewness>;
}
