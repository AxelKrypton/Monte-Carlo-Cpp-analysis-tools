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

#include "../dataAnalysisUtilities/binning.hpp"
#include "Tools.hpp"

Mean::Mean() : QuantityAbstract() {}

Mean::Mean(DataSample dataSample, BinningParameters parameters, bool isMeanZero) : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(dataSample, parameters);
}

Mean::Mean(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    if (isMeanZero)
        value = EstimateAndError(0.0, 0.0);  // TODO: Shouldn't the error still be calculated!?
    else
        QuantityAbstract::calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
}

void Mean::calculateAndSetValueAndError(DataSample& dataSample, BinningParameters parameters)
{
    DataSample binnedSample(dataSample);
    if (parameters.performBinning) {
        printCorrectBinningInformation(parameters);
        binnedSample = performBinning(dataSample, parameters);
    }
    if (isMeanZero) {
        value.estimate = 0.0;
        value.error = std::sqrt(1. / realFloat(binnedSample.getNumberOfElements() - 1) * binnedSample.getNthMoment(2));
    } else {
        value.estimate = binnedSample.getNthMoment(1);
        value.error = std::sqrt(1. / realFloat(binnedSample.getNumberOfElements() - 1) * binnedSample.getNthCentralMoment(2));
    }
}

void Mean::printCorrectBinningInformation(const BinningParameters& parameters)
{
    printBinningInformation(parameters, constants::observableName<Mean>);
}

functionForEstimatorsForJackknife Mean::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    /*
     * Here for the jackknife we always need f(x)=x also if the mean is known to be zero.
     * This is necessary to get the standard deviation of the sample evaluated and the
     * idea if the mean is known to be zero is then to set it by hand ignoring the value
     * returned by the jackknife -> see QuantityAbstract::calculateAndSetValueAndError
     */
    return [](std::vector<DataSample> in) -> DataSample {
        if (in.size() != 1)
            throw std::invalid_argument("Invalid call to Mean function for estimators!");
        return in[0];
    };
}

functionForObservable Mean::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Mean>(isMeanZero, useMultipleEstimate);
}

functionForEstimators Mean::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanZero, useMultipleEstimate);
}

DataSample Mean::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Mean::getNeededMoments()
{
    return isMeanZero ? constants::neededMomentsWithZeroMean<Mean> : constants::neededMoments<Mean>;
}
