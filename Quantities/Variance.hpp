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

#pragma once

#include "AbstractQuantity.hpp"

/*
 * ATTENTION: Constructor which takes DataSample must take it by value, since it will
 *            change it doing binning inside!
 */
class Variance : public QuantityAbstract {
  public:
    Variance(DataSample, BinningParameters, QuantityAttributes);
    Variance(MultipleDataSample, BinningParameters, QuantityAttributes);
    Variance(Moments, MomentsEstimators, QuantityAttributes, ErrorCalculationMethod);

    static DataSample
    evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate = false);

  private:
    Variance();
    void calculateAndSetValueAndError(MultipleDataSample&, BinningParameters) override;
    void printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample) override;
    functionForEstimatorsForJackknife getFunctionToBeAppliedToEstimatorsForJackknife() override;
    functionForObservable getFunctionToCalculateObservable() override;
    functionForEstimators getFunctionToBeAppliedToEstimators() override;
    std::initializer_list<unsigned int> getNeededMoments() override;
    friend class Quantities;
};
