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

#include "../IO/io_utilities.hpp"
#include "../dataAnalysisUtilities/binning.hpp"
#include "Tools.hpp"

Mean::Mean() : QuantityAbstract() {}

Mean::Mean(DataSample dataSample, BinningParameters parameters, bool isMeanZero)
    : Mean(MultipleDataSample({dataSample}), parameters, isMeanZero)
{
}

Mean::Mean(MultipleDataSample dataSamples, BinningParameters parameters, bool isMeanZero) : QuantityAbstract(isMeanZero)
{
    if (dataSamples.size() > 1)
        throw std::invalid_argument("Analysis of Mean with multiple columns not implemented yet!");

    calculateAndSetValueAndError(dataSamples[0], parameters);
    PrintRepeatedSymbol();
};

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
        printCorrectBinningInformation(parameters, dataSample.getNumberOfElements());
        DEBUG(std::cout << "# Moment 1\n");
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

void Mean::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Mean>, elementsOfSample);
}

functionForEstimatorsForJackknife Mean::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    throw std::logic_error("Forbidden to retrieve function for jackknife from Mean class!");
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

std::initializer_list<unsigned int> Mean::getNeededMoments(bool expanded)
{
    return expanded ? constants::neededMomentsExpanded<Mean> : constants::neededMomentsUnexpanded<Mean>;
}
