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

#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../dataAnalysisUtilities/MultipleDataSample.hpp"
#include "../types.hpp"
#include "Constants.hpp"
#include "EstimateAndError.hpp"
#include "Moments.hpp"

class BinningParameters;

/*
 * TODO: So far the error method is not a private member of the class, since for the raw data only Jackknife is used!
 *       Think whether put it as private member and in case do it.
 *
 * TODO: Change the last line above with the following:
 *           typedef std::function<DataSample(std::vector<DataSample>)> functionForEstimatorsForJackknife;
 *       Actually it could be possible that this can be completely removed and only the std::function<DataSample(MomentsEstimators)>
 *       can be used. The thing that one should think of is that in jackknifeAnalysis the functions take as second argument a function that
 *       has in input a vector of DataSample and there the MomentsEstimators are not used. It should be possible to use there the
 *       MomentsEstimators and one could have here just one single functionForEstimators.
 *
 *       This is also the reason why for the moment there are no static members with this function that is hard coded in the getter.
 */

class QuantityAbstract {
  public:
    QuantityAbstract(bool isMeanKnownToBeZero = false);
    virtual ~QuantityAbstract(){};
    EstimateAndError value;

  protected:
    /*
     *  Calculation from raw data. Note that this is virtual since we want to implement here
     *  a general method using jackknife, but override it avoiding jackknife when not needed,
     *  like for the mean and for the variance where jackknife triggers only rounding errors.
     */
    virtual void calculateAndSetValueAndError(DataSample& dataSample, BinningParameters parameters);
    // Calculation for Reweighting
    void calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod,
                                      bool useMultipleEstimate);
    bool isMeanZero;

  private:
    std::vector<DataSample> getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const BinningParameters& parameters);
    std::vector<DataSample> calculateNeededMomentsPerDataPoint(DataSample& dataSample);
    // Virtual method that must be provided by children classes
    // virtual Parameters getLocalParametersWithCorrectBinningInformation(const BinningParameters& parameters) = 0;
    virtual void printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample) = 0;
    virtual functionForEstimatorsForJackknife getFunctionToBeAppliedToEstimatorsForJackknife() = 0;
    virtual functionForEstimators getFunctionToBeAppliedToEstimators(bool useMultipleEstimate = false) = 0;
    virtual functionForObservable getFunctionToCalculateObservable(bool useMultipleEstimate = false) = 0;
    virtual std::initializer_list<unsigned int> getNeededMoments(bool expanded = false) = 0;
};
