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

#include "AbstractQuantity.hpp"

#include "../dataAnalysisUtilities/binning.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"
#include "../dataAnalysisUtilities/jackknifeAnalysis.hpp"
#include "Tools.hpp"

QuantityAbstract::QuantityAbstract(const QuantityAttributes& options)
    : value(NAN, NAN)
    , isMeanZero(options.isMeanZero)
    , useMultipleEstimates(options.useMultipleEstimates)
{
}

void QuantityAbstract::calculateAndSetValueAndError(MultipleDataSample& dataSamples, BinningParameters parameters)
{
    /*
     * ATTENTION: It is in general wrong to perform binning on the sample BEFORE calculating
     *            the n-th (central) moment per data point. Binning must be done AFTER!
     *            The reason boils down to a power of a sum VS a sum of powers.
     */
    std::vector<DataSample> neededMomentsPerDataPoint = calculateNeededMomentsPerDataPoint(dataSamples);
    std::vector<DataSample> binnedMomentsPerDataPoint(neededMomentsPerDataPoint);
    if (parameters.performBinning) {
        printCorrectBinningInformation(parameters, dataSamples[0].getNumberOfElements());
        binnedMomentsPerDataPoint = getBinnedNeededMoments(neededMomentsPerDataPoint, parameters);
    }
    value = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToBeAppliedToJackknifeEstimators());
    checkCalculatedValue();
}

void QuantityAbstract::calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod)
{
    value.estimate = getFunctionToToBeAppliedToMoments()(moments);
    DataSample functionAppliedToEstimators = getFunctionToBeAppliedToMomentsEstimators()(estimators);
    value.error = evaluateErrorBasedOnMethod(functionAppliedToEstimators, errorMethod);
}

std::vector<DataSample> QuantityAbstract::calculateNeededMomentsPerDataPoint(MultipleDataSample& dataSamples)
{
    std::vector<DataSample> returnVec;
    for (auto i : getNeededMoments()) {
        if (useMultipleEstimates || isMeanZero)
            returnVec.push_back(dataSamples.getNthMomentPerDataPoint(i));
        else
            returnVec.push_back(dataSamples.getNthCentralMomentPerDataPoint(i));
    }
    return returnVec;
}

// ATTENTION: This function takes std::vector<DataSample> by value, since it will change it doing binning inside!
std::vector<DataSample>
QuantityAbstract::getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const BinningParameters& parameters)
{
    std::vector<DataSample> returnData;
    std::vector<unsigned int> moments = getNeededMoments();
    if (moments.size() != dataSampleToBeBinned.size())
        throw std::logic_error("Mismatching sizes detected in \"" + std::string(__FUNCTION__) + "\" function!");
    for (size_t i = 0; i < dataSampleToBeBinned.size(); i++) {
        DEBUG(std::cout << "# Moment " << moments[i] << "\n");
        returnData.push_back(performBinning(dataSampleToBeBinned[i], parameters));
    }
    return returnData;
}
