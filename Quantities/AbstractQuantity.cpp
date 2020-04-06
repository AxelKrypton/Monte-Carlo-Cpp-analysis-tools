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

#include "AbstractQuantity.hpp"

#include "../dataAnalysisUtilities/binning.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"
#include "../dataAnalysisUtilities/jackknifeAnalysis.hpp"
#include "Tools.hpp"

QuantityAbstract::QuantityAbstract(bool isMeanKnownToBeZero) : value(NAN, NAN), isMeanZero(isMeanKnownToBeZero) {}

void QuantityAbstract::calculateAndSetValueAndError(DataSample& dataSample, BinningParameters parameters)
{
    std::vector<DataSample> neededMomentsPerDataPoint = calculateNeededMomentsPerDataPoint(dataSample);
    printCorrectBinningInformation(parameters);
    std::vector<DataSample> binnedMomentsPerDataPoint
        = (parameters.performBinning) ? getBinnedNeededMoments(neededMomentsPerDataPoint, parameters) : neededMomentsPerDataPoint;
    value = jackknifeAnalysis(binnedMomentsPerDataPoint, getFunctionToBeAppliedToEstimatorsForJackknife());
    if (isMeanZero && dynamic_cast<Mean*>(this) != nullptr) {
        std::cout << "# MEAN is known to be zero, setting calculated value ( " << value.estimate << " ) to zero.\n";
        value.estimate = 0.0;
    }
}

void QuantityAbstract::calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod,
                                                    bool useMultipleEstimate)
{
    value.estimate = getFunctionToCalculateObservable(useMultipleEstimate)(moments);
    DataSample functionAppliedToEstimators = getFunctionToBeAppliedToEstimators(useMultipleEstimate)(estimators);
    value.error = evaluateErrorBasedOnMethod(functionAppliedToEstimators, errorMethod);
}

std::vector<DataSample> QuantityAbstract::calculateNeededMomentsPerDataPoint(DataSample& dataSample)
{
    std::vector<DataSample> returnVec;
    for (auto i : getNeededMoments())
        returnVec.push_back(dataSample.getNthMomentPerDataPoint(i));
    return returnVec;
}

std::vector<DataSample>
QuantityAbstract::getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const BinningParameters& parameters)
{
    std::vector<DataSample> returnData;
    for (size_t i = 0; i < dataSampleToBeBinned.size(); i++)
        returnData.push_back(performBinning(dataSampleToBeBinned[i], parameters));
    return returnData;
}
