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

#pragma once

#include "AbstractQuantity.hpp"

class Skewness : public QuantityAbstract {
  public:
    Skewness(DataSample& dataSample, BinningParameters parameters, bool isMeanZero = false);
    Skewness(Moments moments, MomentsEstimators estimators, bool isMeanKnownToBeZero, ErrorCalculationMethod errorMethod,
             bool useMultipleEstimate = false);
    static DataSample
    evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate = false);

  private:
    Skewness();
    void printCorrectBinningInformation(const BinningParameters& parameters);
    functionForEstimatorsForJackknife getFunctionToBeAppliedToEstimatorsForJackknife();
    functionForEstimators getFunctionToBeAppliedToEstimators(bool useMultipleEstimate = false);
    functionForObservable getFunctionToCalculateObservable(bool useMultipleEstimate = false);
    std::initializer_list<unsigned int> getNeededMoments();
    friend class Quantities;
};
