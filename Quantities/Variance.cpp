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

#include "Variance.hpp"

#include "../IO/io_utilities.hpp"
#include "../dataAnalysisUtilities/binning.hpp"
#include "Tools.hpp"

Variance::Variance() : QuantityAbstract(QuantityAttributes{}) {}

Variance::Variance(DataSample dataSample, BinningParameters parameters, QuantityAttributes options)
    : Variance(MultipleDataSample({dataSample}), parameters, options)
{
}

Variance::Variance(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if ((dataSamples.size() > 1 && useMultipleEstimates == false) || (dataSamples.size() == 1 && useMultipleEstimates == true))
        throw std::logic_error("Variance object instantiated with contradicting parameters!");

    if (dataSamples.size() > 1)
        throw std::invalid_argument("Analysis of Variance with multiple columns not implemented yet!");

    calculateAndSetValueAndError(dataSamples[0], parameters);
    PrintRepeatedSymbol();
}

Variance::Variance(Moments moments, MomentsEstimators estimators, QuantityAttributes options, ErrorCalculationMethod errorMethod)
    : QuantityAbstract(options)
{
    QuantityAbstract::calculateAndSetValueAndError(moments, estimators, errorMethod);
}

static realFloat unbiasedVarianceOfDataSample(DataSample&, bool);
static realFloat unbiasedErrorOfVariance(DataSample&);

void Variance::calculateAndSetValueAndError(DataSample& dataSample, BinningParameters parameters)
{
    /*
     * ATTENTION: It is in general wrong to perform binning on the sample BEFORE calculating
     *            the n-th (central) moment per data point. Binning must be done AFTER!
     *            The reason boils down to a power of a sum VS a sum of powers.
     */
    DataSample varianceSample = isMeanZero ? dataSample.getNthMomentPerDataPoint(2) : dataSample.getNthCentralMomentPerDataPoint(2);
    DataSample binnedVarianceSample(varianceSample);
    if (parameters.performBinning) {
        printCorrectBinningInformation(parameters, dataSample.getNumberOfElements());
        // It is important to do binning on original sample, which then gets resized discarding
        // last elements and then the calculation of variance and error is on consistent samples!
        // TODO: improve, e.g. just resize!
        DEBUG(std::cout << "# Moment 1\n");
        performBinning(dataSample, parameters);
        DEBUG(std::cout << "# Moment 2\n");
        binnedVarianceSample = performBinning(varianceSample, parameters);
    }
    value.estimate = unbiasedVarianceOfDataSample(dataSample, isMeanZero);
    value.error = unbiasedErrorOfVariance(binnedVarianceSample);
}

void Variance::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Variance>, elementsOfSample);
}

functionForEstimatorsForJackknife Variance::getFunctionToBeAppliedToEstimatorsForJackknife()
{
    throw std::logic_error("Forbidden to retrieve function for jackknife from Variance class!");
}

functionForObservable Variance::getFunctionToCalculateObservable()
{
    return pickUpCorrectFunctionForObservable<Variance>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Variance::getFunctionToBeAppliedToEstimators()
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanZero, useMultipleEstimates);
}

DataSample Variance::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpCorrectFunctionForEstimator<Variance>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Variance::getNeededMoments(bool expanded)
{
    return expanded ? constants::neededMomentsExpanded<Variance> : constants::neededMomentsUnexpanded<Variance>;
}

/**
 * A Jackknife analysis of the (naive) sample variance
 *
 *    1/N Sum ( sample[i] - mean )^2
 *
 * yields that the pseudovalues are
 *
 *    N/(N-1) Sum ( sample[i] - mean )^2
 *
 * (Example 3 in "Jackknife.pdf").
 *
 * This means that the jackknife error estimate is
 *
 *    sqrt(1/(N-1) * (Variance of the pseudo-values) )
 *
 * Hence, one can generate a new sample with each entry x_j the "variance" of
 * entry x_j ( (x_j - mean)^2 ) and treat it the same way as an error on a mean.
 *
 * NOTE: Here no bool is present to indicate a zero mean, because we are working
 *       on a sample with the second central moment per original data point and
 *       the mean of this sample is in general different from zero (we do not
 *       consider the very remote case where it is known to be zero).
 */
static realFloat unbiasedErrorOfVariance(DataSample& sampleWithSecondCentralMomentPerDataPoint)
{
    return std::sqrt(1. / realFloat(sampleWithSecondCentralMomentPerDataPoint.getNumberOfElements() - 1)
                     * sampleWithSecondCentralMomentPerDataPoint.getNthCentralMoment(2));
}

/**
 * A biased estimate of the sample variance is the naive definition, the second central moment.
 * An unbiased estimate of variance of the sample is
 *
 *    n/(n-1) * biasedEstimator(varianceOfSample)
 *
 * and the estimate of the variance of the mean of the sample is always:
 *
 *    varianceEstimator(sample) / n
 *
 * because of the central limit theorem. See B.A. Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis"
 * Note that for the mean the unbiased variance yields the same error as jackknifing.
 */
static realFloat unbiasedVarianceOfDataSample(DataSample& sampleIn, bool isMeanKnownToBeZero)
{
    return isMeanKnownToBeZero
               ? realFloat(sampleIn.getNumberOfElements()) / realFloat(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthMoment(2)
               : realFloat(sampleIn.getNumberOfElements()) / realFloat(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthCentralMoment(2);
}
