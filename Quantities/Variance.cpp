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
    : Variance(MultipleDataSample(dataSample), parameters, options)
{
}

Variance::Variance(MultipleDataSample dataSamples, BinningParameters parameters, QuantityAttributes options) : QuantityAbstract(options)
{
    if ((dataSamples.size() > 1 && useMultipleEstimates == false) || (dataSamples.size() == 1 && useMultipleEstimates == true))
        throw std::logic_error("Variance object instantiated with contradicting parameters!");

    if (dataSamples.size() > 1)
        QuantityAbstract::calculateAndSetValueAndError(dataSamples, parameters);
    else
        calculateAndSetValueAndError(dataSamples, parameters);
    PrintRepeatedSymbol();
}

Variance::Variance(Moments moments, MomentsEstimators estimators, QuantityAttributes options, ErrorCalculationMethod errorMethod)
    : QuantityAbstract(options)
{
    QuantityAbstract::calculateAndSetValueAndError(moments, estimators, errorMethod);
}

static realFloat unbiasedVarianceOfDataSample(DataSample&, bool);
static realFloat unbiasedErrorOfVariance(DataSample&);

void Variance::calculateAndSetValueAndError(MultipleDataSample& dataSamples, BinningParameters parameters)
{
    if (dataSamples.size() > 1)
        throw std::logic_error("The function \"Variance::calculateAndSetValueAndError\" should not be called with multiple estimates!");

    /*
     * ATTENTION: It is in general wrong to perform binning on the sample BEFORE calculating
     *            the n-th (central) moment per data point. Binning must be done AFTER!
     *            The reason boils down to a power of a sum VS a sum of powers.
     */
    DataSample varianceSample = isMeanZero ? dataSamples[0].getNthMomentPerDataPoint(2) : dataSamples[0].getNthCentralMomentPerDataPoint(2);
    DataSample binnedVarianceSample(varianceSample);
    if (parameters.performBinning) {
        printCorrectBinningInformation(parameters, dataSamples[0].getNumberOfElements());
        // It is important to do binning on original sample, which then gets resized discarding
        // last elements and then the calculation of variance and error is on consistent samples!
        // TODO: improve, e.g. just resize!
        DEBUG(std::cout << "# Moment 1\n");
        performBinning(dataSamples[0], parameters);
        DEBUG(std::cout << "# Moment 2\n");
        binnedVarianceSample = performBinning(varianceSample, parameters);
    }
    value.estimate = unbiasedVarianceOfDataSample(dataSamples[0], isMeanZero);
    value.error = unbiasedErrorOfVariance(binnedVarianceSample);
}

void Variance::printCorrectBinningInformation(const BinningParameters& parameters, int elementsOfSample)
{
    printBinningInformation(parameters, constants::observableName<Variance>, elementsOfSample);
}

functionForEstimatorsForJackknife Variance::getFunctionToBeAppliedToJackknifeEstimators()
{
    if (useMultipleEstimates)
        if (isMeanZero)
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 1)
                    throw std::invalid_argument("Invalid call to Variance function for Jackknife!");
                return in[0];  // Jackknife estimators second moment
            };
        else
            return [](std::vector<DataSample> in) -> DataSample {
                if (in.size() != 2)
                    throw std::invalid_argument("Invalid call to Variance function for Jackknife!");
                DataSample m1 = in[0];  // Jackknife estimators first moment
                DataSample m2 = in[1];  // Jackknife estimators second moment
                return m2 - (m1 ^ 2.0);
            };
    else
        throw std::logic_error("Forbidden to retrieve function for jackknife from Variance class!");
}

functionForObservable Variance::getFunctionToToBeAppliedToMoments()
{
    return pickUpFunctionToToBeAppliedToMoments<Variance>(isMeanZero, useMultipleEstimates);
}

functionForEstimators Variance::getFunctionToBeAppliedToMomentsEstimators()
{
    return pickUpFunctionToBeAppliedToMomentsEstimator<Variance>(isMeanZero, useMultipleEstimates);
}

DataSample Variance::evaluateObservableOnMomentEstimators(MomentsEstimators estimators, bool isMeanKnownToBeZero, bool useMultipleEstimate)
{
    return pickUpFunctionToBeAppliedToMomentsEstimator<Variance>(isMeanKnownToBeZero, useMultipleEstimate)(estimators);
}

std::initializer_list<unsigned int> Variance::getNeededMoments()
{
    if (useMultipleEstimates)
        return (isMeanZero) ? constants::neededMomentsExpandedWithZeroMean<Variance> : constants::neededMomentsExpanded<Variance>;
    else
        throw std::logic_error("Forbidden to ask for needed moments from Variance class without multiple estimates!");
}

void Variance::checkCalculatedValue()
{
    if (useMultipleEstimates && value.estimate < 0)
        throw std::runtime_error("Obtained negative variance with multiple estimates.");
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
