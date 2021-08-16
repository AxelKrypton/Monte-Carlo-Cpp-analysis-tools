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

typedef std::function<realFloat(Moments)> functionForObservable;
typedef std::function<DataSample(MomentsEstimators)> functionForEstimators;
typedef DataSample (*functionForEstimatorsForJackknife)(std::vector<DataSample>);

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
    // TODO: Implement the following function in a general way
    template<typename T> static T getPowerOfFirstMomentUsingSeveralEstimate(const std::vector<T>& estimates, const int power)
    {
        if (estimates.size() != 4)
            throw std::invalid_argument("So far only 4 multiple estimates are allowed in quantities calculations! Aborting...");

        switch (power) {
            case 1:
                return (estimates[0] + estimates[1] + estimates[2] + estimates[3]) / (realFloat)4.0;
            case 2:
                return ((estimates[0] * estimates[1]) + (estimates[0] * estimates[2]) + (estimates[0] * estimates[3])
                        + (estimates[1] * estimates[2]) + (estimates[1] * estimates[3]) + (estimates[2] * estimates[3]))
                       / (realFloat)6.0;
            case 3:
                return ((estimates[0] * estimates[1] * estimates[2]) + (estimates[0] * estimates[1] * estimates[3])
                        + (estimates[0] * estimates[2] * estimates[3]) + (estimates[1] * estimates[2] * estimates[3]))
                       / (realFloat)4.0;
            case 4:
                return (estimates[0] * estimates[1] * estimates[2] * estimates[3]);
            default:
                throw std::invalid_argument(
                    "Asked for a not implemented power in \"getPowerOfFirstMomentUsingSeveralEstimate\" function! Aborting...");
        }
    }

    template<class T> const std::string observableName = "UNKNOWN";
    template<class T> const std::initializer_list<unsigned int> neededMomentsExpanded = {UINT_MAX};
    template<class T> const std::initializer_list<unsigned int> neededMomentsUnexpanded = {UINT_MAX};
    template<class T>
    const functionForObservable functionToCalculateQuantityWithZeroMean = [](Moments in) -> realFloat {
        throw std::logic_error("General template definition for functionToCalculateQuantityWithZeroMean used!");
    };
    template<class T>
    const functionForObservable functionToCalculateQuantityWithNonZeroMean = [](Moments in) -> realFloat {
        throw std::logic_error("General template definition for functionToCalculateQuantityWithNonZeroMean used!");
    };
    template<class T>
    const functionForObservable functionToCalculateQuantityWithMultipleEstimates = [](Moments in) -> realFloat {
        throw std::logic_error("General template definition for functionToCalculateQuantityWithMultipleEstimates used!");
    };
    template<class T>
    const functionForEstimators functionToBeAppliedToEstimatorsWithZeroMean = [](MomentsEstimators in) -> DataSample {
        throw std::logic_error("General template definition for functionToBeAppliedToEstimatorsWithZeroMean used!");
    };
    template<class T>
    const functionForEstimators functionToBeAppliedToEstimatorsWithNonZeroMean = [](MomentsEstimators in) -> DataSample {
        throw std::logic_error("General template definition for functionToBeAppliedToEstimatorsWithNonZeroMean used!");
    };
    template<class T>
    const functionForEstimators functionToBeAppliedToEstimatorsWithMultipleEstimates = [](MomentsEstimators in) -> DataSample {
        throw std::logic_error("General template definition for functionToBeAppliedToEstimatorsWithMultipleEstimates used!");
    };

    // Template specialisations -> inline to avoid linking errors! https://stackoverflow.com/a/51987559
    template<> inline const std::string observableName<Mean> = "MEAN";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Mean> = {1};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Mean> = {1};  // Needed for the jackknife error!
    template<>
    inline const functionForObservable functionToCalculateQuantityWithNonZeroMean<Mean> = [](Moments in) -> realFloat { return in[1]; };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithMultipleEstimates<Mean> =
        [](Moments in) -> realFloat { return getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1); };
    template<>
    inline const functionForEstimators
        functionToBeAppliedToEstimatorsWithNonZeroMean<Mean> = [](MomentsEstimators in) -> DataSample { return in[1]; };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithMultipleEstimates<Mean> =
        [](MomentsEstimators in) -> DataSample { return getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1); };

    template<> inline const std::string observableName<Variance> = "VARIANCE";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Variance> = {1, 2};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Variance> = {2};
    template<>
    inline const functionForObservable functionToCalculateQuantityWithZeroMean<Variance> = [](Moments in) -> realFloat { return in[2]; };
    template<>
    inline const functionForObservable
        functionToCalculateQuantityWithNonZeroMean<Variance> = [](Moments in) -> realFloat { return in[2] - in[1] * in[1]; };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithMultipleEstimates<Variance> = [](Moments in) -> realFloat {
        realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
        return in[2] - firstMoment * firstMoment;
    };
    template<>
    inline const functionForEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Variance> = [](MomentsEstimators in) -> DataSample { return in[2]; };
    template<>
    inline const functionForEstimators
        functionToBeAppliedToEstimatorsWithNonZeroMean<Variance> = [](MomentsEstimators in) -> DataSample { return in[2] - (in[1] ^ 2); };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithMultipleEstimates<Variance> = [](MomentsEstimators in) -> DataSample {
        DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
        return in[2] - firstMoment * firstMoment;
    };

    template<> inline const std::string observableName<Skewness> = "SKEWNESS";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Skewness> = {1, 2, 3};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Skewness> = {2, 3};
    template<>
    inline const functionForObservable
        functionToCalculateQuantityWithZeroMean<Skewness> = [](Moments in) -> realFloat { return in[3] / std::pow(in[2], 1.5); };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithNonZeroMean<Skewness> = [](Moments in) -> realFloat {
        realFloat x1 = in[1];
        realFloat x2 = in[2];
        realFloat x3 = in[3];
        return (x3 - 3 * x2 * x1 + 2 * x1 * x1 * x1) / (std::pow(x2 - x1 * x1, 1.5));
    };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithMultipleEstimates<Skewness> = [](Moments in) -> realFloat {
        realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
        realFloat x2 = in[2];
        realFloat x3 = in[3];
        return (x3 - 3 * x2 * firstMoment + 2 * firstMoment * firstMoment * firstMoment) / (std::pow(x2 - firstMoment * firstMoment, 1.5));
    };
    template<>
    inline const functionForEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Skewness> = [](MomentsEstimators in) -> DataSample { return in[3] / (in[2] ^ 1.5); };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithNonZeroMean<Skewness> = [](MomentsEstimators in) -> DataSample {
        return (in[3] - ((3 * in[2]) * in[1]) + (2 * (in[1] ^ 3))) / ((in[2] - (in[1] ^ 2)) ^ 1.5);
    };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithMultipleEstimates<Skewness> = [](MomentsEstimators in) -> DataSample {
        DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
        return (in[3] - ((3 * in[2]) * firstMoment) + (2 * firstMoment * firstMoment * firstMoment))
               / ((in[2] - firstMoment * firstMoment) ^ 1.5);
    };

    template<> inline const std::string observableName<Kurtosis> = "KURTOSIS";
    template<> inline const std::initializer_list<unsigned int> neededMomentsExpanded<Kurtosis> = {1, 2, 3, 4};
    template<> inline const std::initializer_list<unsigned int> neededMomentsUnexpanded<Kurtosis> = {2, 4};
    template<>
    inline const functionForObservable
        functionToCalculateQuantityWithZeroMean<Kurtosis> = [](Moments in) -> realFloat { return in[4] / std::pow(in[2], 2.0); };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithNonZeroMean<Kurtosis> = [](Moments in) -> realFloat {
        realFloat x1 = in[1];
        realFloat x2 = in[2];
        realFloat x3 = in[3];
        realFloat x4 = in[4];
        return (x4 - 4 * x3 * x1 + 6 * x2 * x1 * x1 - 3 * x1 * x1 * x1 * x1) / (std::pow(x2 - x1 * x1, 2.0));
    };
    template<>
    inline const functionForObservable functionToCalculateQuantityWithMultipleEstimates<Kurtosis> = [](Moments in) -> realFloat {
        realFloat firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<realFloat>(in(1), 1);
        realFloat x2 = in[2];
        realFloat x3 = in[3];
        realFloat x4 = in[4];
        return (x4 - 4 * x3 * firstMoment + 6 * x2 * firstMoment * firstMoment - 3 * firstMoment * firstMoment * firstMoment * firstMoment)
               / (std::pow(x2 - firstMoment * firstMoment, 2.0));
    };
    template<>
    inline const functionForEstimators
        functionToBeAppliedToEstimatorsWithZeroMean<Kurtosis> = [](MomentsEstimators in) -> DataSample { return in[4] / (in[2] ^ 2.0); };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithNonZeroMean<Kurtosis> = [](MomentsEstimators in) -> DataSample {
        return (in[4] - (4 * in[3] * in[1]) + (6 * in[2] * in[1] * in[1]) - (3 * in[1] * in[1] * in[1] * in[1])) / ((in[2] - (in[1] ^ 2)) ^ 2);
    };
    template<>
    inline const functionForEstimators functionToBeAppliedToEstimatorsWithMultipleEstimates<Kurtosis> = [](MomentsEstimators in) -> DataSample {
        DataSample firstMoment = getPowerOfFirstMomentUsingSeveralEstimate<DataSample>(in(1), 1);
        return (in[4] - (4 * in[3] * firstMoment) + (6 * in[2] * firstMoment * firstMoment)
                - (3 * firstMoment * firstMoment * firstMoment * firstMoment))
               / ((in[2] - (firstMoment ^ 2)) ^ 2);
    };
}
