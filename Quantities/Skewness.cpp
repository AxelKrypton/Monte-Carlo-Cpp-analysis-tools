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
    : Skewness(MultipleDataSample(dataSample), parameters, options)
{
}

Skewness::Skewness(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if ((dataSamples.size() > 1 && useMultipleEstimates == false) || (dataSamples.size() == 1 && useMultipleEstimates == true))
        throw std::logic_error("Skewness object instantiated with contradicting parameters!");

    calculateAndSetValueAndError(dataSamples, parameters);
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

functionForEstimatorsForJackknife Skewness::getFunctionToBeAppliedToJackknifeEstimators()
{
    /*
     * NOTE: Code duplication left on purpose for clarification on what is being done as analysis
     */
    if (useMultipleEstimates)
        if (isMeanZero)
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 2)
                    throw std::invalid_argument("Invalid call to Skewness function for Jackknife with multiple estimators and zero mean!");
                DataSample m2 = in[0];  // Jackknife estimators second moment
                DataSample m3 = in[1];  // Jackknife estimators third moment
                return m3 / (m2 ^ 1.5);
            };
        else
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 3)
                    throw std::invalid_argument("Invalid call to Skewness function for Jackknife with multiple estimators!");
                DataSample m1 = in[0];  // Jackknife estimators first moment
                DataSample m2 = in[1];  // Jackknife estimators second moment
                DataSample m3 = in[2];  // Jackknife estimators third moment
                return (m3 - 3.0 * m2 * m1 + 2.0 * (m1 ^ 3.0)) / ((m2 - (m1 ^ 2.0)) ^ 1.5);
            };
    else
        /*
         * NOTE: Here no if-else on isMeanZero is needed, since the choice is done
         *       where the needed moments are evaluated!
         */
        return [](std::vector<DataSample> in) -> DataSample {
            if (in.size() != 2)
                throw std::invalid_argument("Invalid call to Skewness function for Jackknife!");
            DataSample cm2 = in[0];  // Jackknife estimators second central moment
            DataSample cm3 = in[1];  // Jackknife estimators third  central moment
            return cm3 / (cm2 ^ 1.5);
        };
}

functionForObservable Skewness::getFunctionToToBeAppliedToMoments()
{
    return pickUpFunctionToToBeAppliedToMoments<Skewness>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Skewness::getFunctionToBeAppliedToMomentsEstimators()
{
    return pickUpFunctionToBeAppliedToMomentsEstimator<Skewness>(isMeanZero, useMultipleEstimates);
}

DataSample Skewness::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpFunctionToBeAppliedToMomentsEstimator<Skewness>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Skewness::getNeededMoments()
{
    if (useMultipleEstimates)
        return (isMeanZero) ? constants::neededMomentsExpandedWithZeroMean<Skewness> : constants::neededMomentsExpanded<Skewness>;
    else
        return constants::neededMomentsUnexpanded<Skewness>;
}

void Skewness::checkCalculatedValue()
{
    if (useMultipleEstimates) {
        if (std::isnan(value.estimate))
            throw std::runtime_error("Obtained nan as skewness value with multiple estimates.");
        if (std::isnan(value.error))
            throw std::runtime_error("Obtained nan as skewness error with multiple estimates.");
    }
}
