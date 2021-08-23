/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2016,2018,2020 Alessandro Sciarra
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

#include "dataAnalysisUtilities.hpp"

#include "../IO/io_utilities.hpp"
#include "binnedDataSample.hpp"
#include "bootstrapAnalysis.hpp"
#include "jackknifeAnalysis.hpp"

realFloat evaluateErrorBasedOnMethod(DataSample dataSample, ErrorCalculationMethod errorMethod)
{
    if (errorMethod == jackknife)
        return calculateJacknifeError(dataSample);
    else if (errorMethod == bootstrap)
        return calculateBootstrapError(dataSample);
    else
        throw std::logic_error("Unknown error method in \"evaluateErrorBasedOnMethod\"! Aborting...");
}

/*
 * Here, following the Berg book (pages 201-202), a method to estimate the integrated_autocorrelation_time
 * with an error is implemented. It is based on the Jackknife method. The idea is to build a set of
 * jackknife estimators for the autocorrelation_function C(t) of eq. (4.3) at some FIXED time.
 *
 * Once this set X of estimators is ready one can
 *   - either use the jackknife function above with a trivial f in order to get a value with
 *     error for C(t)
 *      --->  jackknife(X, [] (realFloat val) -> realFloat {return val;})
 *   - or build other sets of estimators like X at different times and use them to build a set Y
 *     of estimators for the integrated_autocorrelation_time at time t (see eq. (4.14)). Again
 *     use the jackknife function above with a trivial f in order to get a value with error for tau_int(t)
 *      --->  jackknife(Y, [] (realFloat val) -> realFloat {return val;})
 *  In both cases, one can produce plots similar to those of Figure 4.1-4.2.
 *
 *  Plotting the resulting data with errors, from the FIRST plateau, one can make the final estimate
 *  of the integrated_autocorrelation_time (read around equation (4.13) to understand why to look at
 *  the first plateau is important).
 *
 *  NOTE: Probably the jackknife call could in principle done with a more complicated function
 *        so that the sets X and Y can be built directly inside the jackknife. This is not however
 *        wise because to apply the jackknife one must have NOT correlated data and here this is
 *        achieved with a binning on the estimators BEFORE calling the jackknife.
 *
 *  NOTE: In principle, if one is interested only to a rough idea of the integrated_autocorrelation_time
 *        he can implement the equation (4.14) and look for a plateau plotting the output data.
 *        Of course this is not so rigorous because no error is estimated.
 *
 *  NOTE: The number of bins in the following function is that used to make binning on the estimators before
 *        the jackknife and it is contained in parameters (numberOfBinsForAutocorrelation).
 *        On page 201 of Berg's book there is written that it has to be much smaller of the total amount of data.
 *        This is up to the user, but if it is not given, then it is set to 10 (in principle fine for a data sample
 *        with more than 1000 data).
 */
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample& sample, int time);
static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample& sample, Parameters parameters);

std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters)
{
    std::vector<BinnedDataSampleFromNumberOfBins> autocorrelationFunctionValuesBinnedSets
        = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

    std::vector<EstimateAndError> result;
    auto identicalFunction = [](DataSample& in) -> DataSample { return in; };
    for (int time = 0; time < parameters.timeMaxAutocorrelationFunction; time++) {
        result.push_back(jackknifeAnalysis(autocorrelationFunctionValuesBinnedSets[time], identicalFunction));
    }

    return result;
}

std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters)
{
    std::vector<BinnedDataSampleFromNumberOfBins> autocorrelationFunctionValuesBinnedSets
        = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

    std::vector<DataSample> integratedTimeBinnedSets;
    // The first array of integratedTimeBinnedSets must be an array of ones
    integratedTimeBinnedSets.push_back(
        DataSample(std::valarray<realFloat>(1.0, autocorrelationFunctionValuesBinnedSets[0].getNumberOfElements())));

    for (int time = 1; time < parameters.timeMaxAutocorrelationFunction; time++) {
        integratedTimeBinnedSets.push_back(
            (integratedTimeBinnedSets[time - 1]
             + 2.0 * autocorrelationFunctionValuesBinnedSets[time] / autocorrelationFunctionValuesBinnedSets[0]));
    }

    std::vector<EstimateAndError> result;
    auto identicalFunction = [](DataSample& in) -> DataSample { return in; };
    for (int time = 0; time < parameters.timeMaxAutocorrelationFunction; time++) {
        result.push_back(jackknifeAnalysis(integratedTimeBinnedSets[time], identicalFunction));
    }

    return result;
}

void calcAutocorrelationAndErrorOfDataSample(DataSample& sample, Parameters parameters)
{
    std::vector<EstimateAndError> result = calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(sample, parameters);
    std::string filename = getFilenameForAutocorrelation(parameters);
    writeEstimateAndErrorArraysToFile(result, filename);
}

/*****************************************************************************************/
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample& sample, int time)
{
    DataSampleBasic x_first = sample.sampleSlice(0, sample.getNumberOfElements() - time, 1);
    DataSampleBasic x_second = sample.sampleSlice(time, sample.getNumberOfElements() - time, 1);
    realFloat data_mean = sample.getNthMoment(1);
    /*
     * Since we do not know in general if the "true" mean value of our sample is zero, we will
     * always substitute it by its estimator (the mean of the sample itself), but then we introduce
     * a bias that we correct here. Read paragraph between (4.19) and (4.20) on Berg's book for
     * additional informations.
     */
    DataSampleBasic result = ((x_first - data_mean) * (x_second - data_mean));
    result = result * sample.getNumberOfElements() / (sample.getNumberOfElements() - 1);
    return result;
}

static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample& sample, Parameters parameters)
{
    std::vector<BinnedDataSampleFromNumberOfBins> autocorrelationFunctionValuesBinnedSets;
    for (int time = 0; time < parameters.timeMaxAutocorrelationFunction; time++) {
        autocorrelationFunctionValuesBinnedSets.push_back(BinnedDataSampleFromNumberOfBins(
            autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(sample, time), parameters.numberOfBinsForAutocorrelation, false));
    }
    return autocorrelationFunctionValuesBinnedSets;
}
