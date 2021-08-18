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

#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../dataAnalysisUtilities/MultipleDataSample.hpp"
#include "../types.hpp"
#include "Constants.hpp"
#include "EstimateAndError.hpp"
#include "Moments.hpp"

class BinningParameters;
class QuantityAttributes;

/*
 * TODO: So far the error method is not a private member of the class, since for the raw data only Jackknife is used!
 *       Think whether put it as private member and in case do it.
 */

class QuantityAbstract {
  public:
    QuantityAbstract(const QuantityAttributes& options);
    virtual ~QuantityAbstract(){};
    EstimateAndError value;

  protected:
    /*
     *  Calculation from raw data. Note that this is virtual since we want to implement here
     *  a general method using jackknife, but override it avoiding jackknife when not needed,
     *  like for the mean and for the variance where jackknife triggers only rounding errors.
     */
    virtual void calculateAndSetValueAndError(MultipleDataSample&, BinningParameters);

    // Calculation for Reweighting
    void calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod);

    bool isMeanZero;
    bool useMultipleEstimates;

  private:
    std::vector<DataSample> getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const BinningParameters& parameters);
    std::vector<DataSample> calculateNeededMomentsPerDataPoint(MultipleDataSample& dataSample);
    // Virtual method that must be provided by children classes
    // virtual Parameters getLocalParametersWithCorrectBinningInformation(const BinningParameters& parameters) = 0;
    virtual void printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample) = 0;
    virtual functionForJackknifeEstimators getFunctionToBeAppliedToJackknifeEstimators() = 0;
    virtual functionForQuantity getFunctionToToBeAppliedToMoments() = 0;
    virtual functionForQuantityEstimators getFunctionToBeAppliedToMomentsEstimators() = 0;
    virtual std::initializer_list<unsigned int> getNeededMoments() = 0;
    virtual void checkCalculatedValue() = 0;
};
