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
    : Kurtosis(MultipleDataSample(dataSample), parameters, options)
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
    /*
     * NOTE: Code duplication left on purpose for clarification on what is being done as analysis
     */
    if (useMultipleEstimates)
        if (isMeanZero)
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 2)
                    throw std::invalid_argument("Invalid call to Kurtosis function for Jackknife with multiple estimators and zero mean!");
                DataSample m2 = in[0];  // Jackknife estimators second moment
                DataSample m4 = in[1];  // Jackknife estimators third moment
                return m4 / (m2 ^ 2.0);
            };
        else
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 4)
                    throw std::invalid_argument("Invalid call to Kurtosis function for Jackknife with multiple estimators!");
                DataSample m1 = in[0];  // Jackknife estimators first moment
                DataSample m2 = in[1];  // Jackknife estimators second moment
                DataSample m3 = in[2];  // Jackknife estimators third moment
                DataSample m4 = in[3];  // Jackknife estimators fourth moment
                return (m4 - 4.0 * m3 * m1 + 6.0 * m2 * (m1 ^ 2.0) - 3.0 * (m1 ^ 4.0)) / ((m2 - (m1 ^ 2.0)) ^ 2.0);
            };
    else
        /*
         * NOTE: Here no if-else on isMeanZero is needed, since the choice is done
         *       where the needed moments are evaluated!
         */
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 2)
                throw std::invalid_argument("Invalid call to Kurtosis function for Jackknife!");
            DataSample cm2 = in[0];  // Jackknife estimators second central moment
            DataSample cm4 = in[1];  // Jackknife estimators fourth central moment
            return cm4 / (cm2 ^ 2.0);
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

std::initializer_list<unsigned int> Kurtosis::getNeededMoments()
{
    if (useMultipleEstimates)
        return (isMeanZero) ? constants::neededMomentsExpandedWithZeroMean<Kurtosis> : constants::neededMomentsExpanded<Kurtosis>;
    else
        return constants::neededMomentsUnexpanded<Kurtosis>;
}

void Kurtosis::checkCalculatedValue()
{
    if (useMultipleEstimates) {
        if (value.estimate < 0)
            throw std::runtime_error("Obtained negative kurtosis with multiple estimates.");
        if (std::isnan(value.estimate))
            throw std::runtime_error("Obtained nan as kurtosis value with multiple estimates.");
        if (std::isnan(value.error))
            throw std::runtime_error("Obtained nan as kurtosis error with multiple estimates.");
    }
}
