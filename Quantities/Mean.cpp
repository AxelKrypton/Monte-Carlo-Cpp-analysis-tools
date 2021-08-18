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

#include "Mean.hpp"

#include "../IO/io_utilities.hpp"
#include "../dataAnalysisUtilities/binning.hpp"
#include "Tools.hpp"

Mean::Mean() : QuantityAbstract(QuantityAttributes{}) {}

Mean::Mean(DataSample dataSample, BinningParameters parameters, QuantityAttributes options)
    : Mean(MultipleDataSample(dataSample), parameters, options)
{
}

Mean::Mean(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if ((dataSamples.size() > 1 && useMultipleEstimates == false) || (dataSamples.size() == 1 && useMultipleEstimates == true))
        throw std::logic_error("Mean object instantiated with contradicting parameters!");

    calculateAndSetValueAndError(dataSamples, parameters);
    PrintRepeatedSymbol();
}

Mean::Mean(Moments moments, MomentsEstimators estimators, QuantityAttributes options, ErrorCalculationMethod errorMethod)
    : QuantityAbstract(options)
{
    if (options.isMeanZero)
        value = EstimateAndError(0.0, 0.0);  // TODO: Shouldn't the error still be calculated!?
    else
        QuantityAbstract::calculateAndSetValueAndError(moments, estimators, errorMethod);
}

void Mean::calculateAndSetValueAndError(MultipleDataSample& dataSamples, BinningParameters parameters)
{
    DataSample preprocessedData(dataSamples[0]);
    if (dataSamples.size() > 1)
        preprocessedData = dataSamples.getNthMomentPerDataPoint(1);

    DataSample binnedSample(preprocessedData);
    if (parameters.performBinning) {
        printCorrectBinningInformation(parameters, preprocessedData.getNumberOfElements());
        DEBUG(std::cout << "# Moment 1\n");
        binnedSample = performBinning(preprocessedData, parameters);
    }
    if (isMeanZero) {
        value.estimate = 0.0;
        value.error = std::sqrt(1. / realFloat(binnedSample.getNumberOfElements() - 1) * binnedSample.getNthMoment(2));
    } else {
        value.estimate = binnedSample.getNthMoment(1);
        value.error = std::sqrt(1. / realFloat(binnedSample.getNumberOfElements() - 1) * binnedSample.getNthCentralMoment(2));
    }
    /*
     * TODO: Clarify whether it is correct to calculate the second central moment to get the standard deviation
     *       in the error of the mean in case of being using multiple estimates per trajectories.
     */
}

void Mean::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Mean>, elementsOfSample);
}

functionForEstimatorsForJackknife Mean::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    throw std::logic_error("Forbidden to retrieve function for jackknife from Mean class!");
}

functionForObservable Mean::getFunctionToCalculateObservable()
{
    return pickUpCorrectFunctionForObservable<Mean>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Mean::getFunctionToBeAppliedToEstimators()
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanZero, useMultipleEstimates);
}

DataSample Mean::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Mean>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Mean::getNeededMoments()
{
    throw std::logic_error("Forbidden to ask for needed moments from Mean class!");
}
