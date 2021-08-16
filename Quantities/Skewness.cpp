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

#include "Skewness.hpp"

#include "../IO/io_utilities.hpp"
#include "Tools.hpp"

Skewness::Skewness() : QuantityAbstract(QuantityAttributes{}) {}

Skewness::Skewness(DataSample dataSample, BinningParameters parameters, QuantityAttributes options)
    : Skewness(MultipleDataSample({dataSample}), parameters, options)
{
}

Skewness::Skewness(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if (dataSamples.size() > 1)
        throw std::invalid_argument("Analysis of Skewness with multiple columns not implemented yet!");

    calculateAndSetValueAndError(dataSamples[0], parameters);
    PrintRepeatedSymbol();
}

Skewness::Skewness(Moments moments, MomentsEstimators estimators, QuantityAttributes options, ErrorCalculationMethod errorMethod)
    : QuantityAbstract(options)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod);
}

void Skewness::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Skewness>, elementsOfSample);
}

functionForEstimatorsForJackknife Skewness::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    return [](std::vector<DataSample> in) -> DataSample {
        if (in.size() != 2)
            throw std::invalid_argument("Invalid call to Skewness function for Jackknife!");
        DataSample ex2 = in[0];  // estimator second central moment
        DataSample ex3 = in[1];  // estimator third  central moment
        return ex3 / (ex2 ^ 1.5);
    };
}

functionForObservable Skewness::getFunctionToCalculateObservable()
{
    return pickUpCorrectFunctionForObservable<Skewness>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Skewness::getFunctionToBeAppliedToEstimators()
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanZero, useMultipleEstimates);
}

DataSample Skewness::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Skewness::getNeededMoments(bool expanded)
{
    return expanded ? constants::neededMomentsExpanded<Skewness> : constants::neededMomentsUnexpanded<Skewness>;
}
