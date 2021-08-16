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

#include "Kurtosis.hpp"

#include "../IO/io_utilities.hpp"
#include "Tools.hpp"

Kurtosis::Kurtosis() : QuantityAbstract(QuantityAttributes{}) {}

Kurtosis::Kurtosis(DataSample dataSample, BinningParameters parameters, QuantityAttributes options)
    : Kurtosis(MultipleDataSample({dataSample}), parameters, options)
{
}

Kurtosis::Kurtosis(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if ((dataSamples.size() > 1 && useMultipleEstimates == false) || (dataSamples.size() == 1 && useMultipleEstimates == true))
        throw std::logic_error("Kurtosis object instantiated with contradicting parameters!");

    calculateAndSetValueAndError(dataSamples, parameters);
    PrintRepeatedSymbol();
}
Kurtosis::Kurtosis(Moments moments, MomentsEstimators estimators, QuantityAttributes options, ErrorCalculationMethod errorMethod)
    : QuantityAbstract(options)
{
    calculateAndSetValueAndError(moments, estimators, errorMethod);
}

void Kurtosis::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Kurtosis>, elementsOfSample);
}

functionForEstimatorsForJackknife Kurtosis::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    return [](std::vector<DataSample> in) -> DataSample {
        if (in.size() != 2)
            throw std::invalid_argument("Invalid call to Kurtosis function for Jackknife!");
        DataSample ex2 = in[0];  // estimator second central moment
        DataSample ex4 = in[1];  // estimator fourth central moment
        return ex4 / (ex2 ^ 2.0);
    };
}

functionForObservable Kurtosis::getFunctionToCalculateObservable()
{
    return pickUpCorrectFunctionForObservable<Kurtosis>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Kurtosis::getFunctionToBeAppliedToEstimators()
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanZero, useMultipleEstimates);
}

DataSample Kurtosis::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Kurtosis>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Kurtosis::getNeededMoments(bool expanded)
{
    return expanded ? constants::neededMomentsExpanded<Kurtosis> : constants::neededMomentsUnexpanded<Kurtosis>;
}
