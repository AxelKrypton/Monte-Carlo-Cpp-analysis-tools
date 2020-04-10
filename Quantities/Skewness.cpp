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

#include "../IO/io_utilities.hpp"
#include "Tools.hpp"

Skewness::Skewness() : QuantityAbstract() {}

Skewness::Skewness(DataSample dataSample, BinningParameters parameters, bool isMeanZero)
    : Skewness(std::vector<DataSample>{dataSample}, parameters, isMeanZero)
{
}

Skewness::Skewness(std::vector<DataSample> dataSamples, BinningParameters parameters, bool isMeanZero) : QuantityAbstract(isMeanZero)
{
    if (dataSamples.size() > 1)
        throw std::invalid_argument("Analysis of Skewness with multiple columns not implemented yet!");

    calculateAndSetValueAndError(dataSamples[0], parameters);
    PrintRepeatedSymbol();
}

Skewness::Skewness(Moments moments, MomentsEstimators estimators, bool isMeanZero, ErrorCalculationMethod errorMethod, bool useMultipleEstimate)
    : QuantityAbstract(isMeanZero)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod, useMultipleEstimate);
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

functionForObservable Skewness::getFunctionToCalculateObservable(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForObservable<Skewness>(isMeanZero, useMultipleEstimate);
}

functionForEstimators Skewness::getFunctionToBeAppliedToEstimators(bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanZero, useMultipleEstimate);
}

DataSample Skewness::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Skewness>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Skewness::getNeededMoments(bool expanded)
{
    return expanded ? constants::neededMomentsExpanded<Skewness> : constants::neededMomentsUnexpanded<Skewness>;
}
