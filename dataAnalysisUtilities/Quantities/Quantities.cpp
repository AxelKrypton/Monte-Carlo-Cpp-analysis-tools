/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018-2020 Alessandro Sciarra
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

#include "Quantities.hpp"

#include "../binning.hpp"
#include "../bootstrapAnalysis.hpp"
#include "../jackknifeAnalysis.hpp"
#include "Moments.hpp"
#include "Tools.hpp"

realFloat evaluateErrorBasedOnMethod(DataSample dataSample, ErrorCalculationMethod errorMethod)
{
    if (errorMethod == jackknife)
        return calculateJacknifeError(dataSample);
    else if (errorMethod == bootstrap)
        return calculateBootstrapError(dataSample);
    else
        throw std::logic_error("Unknown error method in \"evaluateErrorBasedOnMethod\"! Aborting...");
}

QuantityAbstract::QuantityAbstract(bool isMeanKnownToBeZero)
    : estimate(observableEstimateAndError.estimate)
    , error(observableEstimateAndError.error)
    , isMeanZero(isMeanKnownToBeZero)
    , observableEstimateAndError(NAN, NAN)
{
}

void QuantityAbstract::calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters)
{
    std::vector<DataSample> neededMomentsPerDataPoint = calculateNeededMomentsPerDataPoint(dataSample);
    Parameters binningParameters = getLocalParametersWithCorrectBinningInformation(parameters);
    printCorrectBinningInformation(binningParameters);
    std::vector<DataSample> binnedMomentsPerDataPoint = getBinnedNeededMoments(neededMomentsPerDataPoint, binningParameters);
    observableEstimateAndError = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToBeAppliedToEstimatorsForJackknife());
}

void QuantityAbstract::calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod,
                                                    bool useMultipleEstimate)
{
    observableEstimateAndError.estimate = getFunctionToCalculateObservable(useMultipleEstimate)(moments);
    DataSample functionAppliedToEstimators = getFunctionToBeAppliedToEstimators(useMultipleEstimate)(estimators);
    observableEstimateAndError.error = evaluateErrorBasedOnMethod(functionAppliedToEstimators, errorMethod);
}

std::vector<DataSample> QuantityAbstract::calculateNeededMomentsPerDataPoint(DataSample& dataSample)
{
    return getMomentsPerDataPoint(dataSample, getNeededMoments(), isMeanZero);
}

std::vector<DataSample> QuantityAbstract::getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters)
{
    std::vector<DataSample> returnData;
    for (size_t i = 0; i < dataSampleToBeBinned.size(); i++)
        returnData.push_back(performBinning(dataSampleToBeBinned[i], parameters));
    return returnData;
}
