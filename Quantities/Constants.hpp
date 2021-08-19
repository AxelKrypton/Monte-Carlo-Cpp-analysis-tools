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

#include "Moments.hpp"

#include <climits>
#include <cmath>
#include <functional>
#include <type_traits>

typedef std::function<realFloat(Moments)> functionForQuantity;
typedef std::function<DataSample(MomentsEstimators)> functionForQuantityEstimators;
typedef DataSample (*functionForJackknifeEstimators)(std::vector<DataSample>);

/*
 * TODO: Change the last line above with the following:
 *           typedef std::function<DataSample(std::vector<DataSample>)> functionForEstimatorsForJackknife;
 *       Actually it could be possible that this can be completely removed and only the std::function<DataSample(MomentsEstimators)>
 *       can be used. The thing that one should think of is that in jackknifeAnalysis the functions take as second argument a function that
 *       has in input a vector of DataSample and there the MomentsEstimators are not used. It should be possible to use there the
 *       MomentsEstimators and one could have here just one single functionForEstimators.
 */

class Mean;
class Variance;
class Skewness;
class Kurtosis;

namespace constants {

    template<class T> const std::string observableName = "UNKNOWN";
    template<class T> const std::initializer_list<unsigned int> neededMomentsExpanded = {UINT_MAX};
    template<class T> const std::initializer_list<unsigned int> neededMomentsExpandedWithZeroMean = {UINT_MAX};
    template<class T> const std::initializer_list<unsigned int> neededMomentsUnexpanded = {UINT_MAX};
    template<class T>
    const functionForQuantity functionToCalculateQuantity
        = [](Moments in) -> realFloat { throw std::logic_error("General template definition for function used!"); };
    template<class T>
    const functionForQuantity functionToCalculateQuantityWithZeroMean
        = [](Moments in) -> realFloat { throw std::logic_error("General template definition for function used!"); };
    template<class T>
    const functionForQuantityEstimators functionToBeAppliedToEstimators
        = [](MomentsEstimators in) -> DataSample { throw std::logic_error("General template definition for function used!"); };
    template<class T>
    const functionForQuantityEstimators functionToBeAppliedToEstimatorsWithZeroMean
        = [](MomentsEstimators in) -> DataSample { throw std::logic_error("General template definition for function used!"); };

    /*
     * Template specialisations for each quantity -> inline to avoid linking errors! https://stackoverflow.com/a/51987559
     */
    //==============================================================================================================================
    // Name and moments
    template<> inline const std::string observableName<Mean> = "MEAN";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Mean> = {1};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Mean> = {1};  // Needed in Reweighting!
    // Functions for quantity
    template<> inline const functionForQuantity functionToCalculateQuantity<Mean> = [](Moments in) -> realFloat { return in[1]; };
    // Functions for estimators
    template<>
    inline const functionForQuantityEstimators
        functionToBeAppliedToEstimators<Mean> = [](MomentsEstimators in) -> DataSample { return in[1]; };
    //==============================================================================================================================

    //==============================================================================================================================
    // Name and moments
    template<> inline const std::string observableName<Variance> = "VARIANCE";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Variance> = {1, 2};
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpandedWithZeroMean<Variance> = {2};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Variance> = {2};
    // Functions for quantity
    template<>
    inline const functionForQuantity functionToCalculateQuantity<Variance> = [](Moments in) -> realFloat { return in[2] - in[1] * in[1]; };
    template<>
    inline const functionForQuantity functionToCalculateQuantityWithZeroMean<Variance> = [](Moments in) -> realFloat { return in[2]; };
    // Functions for estimators
    template<>
    inline const functionForQuantityEstimators
        functionToBeAppliedToEstimators<Variance> = [](MomentsEstimators in) -> DataSample { return in[2] - (in[1] ^ 2); };
    template<>
    inline const functionForQuantityEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Variance> = [](MomentsEstimators in) -> DataSample { return in[2]; };
    //==============================================================================================================================

    //==============================================================================================================================
    // Name and moments
    template<> inline const std::string observableName<Skewness> = "SKEWNESS";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Skewness> = {1, 2, 3};
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpandedWithZeroMean<Skewness> = {2, 3};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Skewness> = {2, 3};
    // Functions for quantity
    template<>
    inline const functionForQuantity functionToCalculateQuantity<Skewness> = [](Moments in) -> realFloat {
        return (in[3] - 3 * in[2] * in[1] + 2 * in[1] * in[1] * in[1]) / (std::pow(in[2] - in[1] * in[1], 1.5));
    };
    template<>
    inline const functionForQuantity
        functionToCalculateQuantityWithZeroMean<Skewness> = [](Moments in) -> realFloat { return in[3] / std::pow(in[2], 1.5); };
    // Functions for estimators
    template<>
    inline const functionForQuantityEstimators functionToBeAppliedToEstimators<Skewness> =
        [](MomentsEstimators in) -> DataSample { return (in[3] - 3 * (in[2] * in[1]) + 2 * (in[1] ^ 3)) / ((in[2] - (in[1] ^ 2)) ^ 1.5); };
    template<>
    inline const functionForQuantityEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Skewness> = [](MomentsEstimators in) -> DataSample { return in[3] / (in[2] ^ 1.5); };
    //==============================================================================================================================

    //==============================================================================================================================
    // Name and moments
    template<> inline const std::string observableName<Kurtosis> = "KURTOSIS";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Kurtosis> = {1, 2, 3, 4};
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpandedWithZeroMean<Kurtosis> = {2, 4};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Kurtosis> = {2, 4};
    // Functions for quantity
    template<>
    inline const functionForQuantity functionToCalculateQuantity<Kurtosis> = [](Moments in) -> realFloat {
        return (in[4] - 4 * in[3] * in[1] + 6 * in[2] * in[1] * in[1] - 3 * in[1] * in[1] * in[1] * in[1])
               / (std::pow(in[2] - in[1] * in[1], 2.0));
    };
    template<>
    inline const functionForQuantity
        functionToCalculateQuantityWithZeroMean<Kurtosis> = [](Moments in) -> realFloat { return in[4] / std::pow(in[2], 2); };
    // Functions for estimators
    template<>
    inline const functionForQuantityEstimators functionToBeAppliedToEstimators<Kurtosis> = [](MomentsEstimators in) -> DataSample {
        return (in[4] - 4 * (in[3] * in[1]) + 6 * (in[2] * (in[1] ^ 2)) - 3 * (in[1] ^ 4)) / ((in[2] - (in[1] ^ 2)) ^ 2);
    };
    template<>
    inline const functionForQuantityEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Kurtosis> = [](MomentsEstimators in) -> DataSample { return in[4] / (in[2] ^ 2); };
}

template<typename OBSERVABLE> functionForQuantity pickUpFunctionToToBeAppliedToMoments(const bool isMeanZero)
{
    if (isMeanZero) {
        if constexpr (std::is_same_v<OBSERVABLE, Mean>)
            throw std::logic_error("Attempt to get function to calculate mean but isMeanZero==true!");
        else
            return constants::functionToCalculateQuantityWithZeroMean<OBSERVABLE>;
    } else
        return constants::functionToCalculateQuantity<OBSERVABLE>;
}
template<typename OBSERVABLE> functionForQuantityEstimators pickUpFunctionToBeAppliedToMomentsEstimator(const bool isMeanZero)
{
    if (isMeanZero) {
        if constexpr (std::is_same_v<OBSERVABLE, Mean>)
            throw std::logic_error("Attempt to get function to calculate mean estimators but isMeanZero==true!");
        else
            return constants::functionToBeAppliedToEstimatorsWithZeroMean<OBSERVABLE>;
    } else
        return constants::functionToBeAppliedToEstimators<OBSERVABLE>;
}
