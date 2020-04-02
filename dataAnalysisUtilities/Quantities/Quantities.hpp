/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2016,2018-2020 Alessandro Sciarra
 *  Copyright (c) 2016 Francesca Cuteri
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

#ifndef OBSERVABLES_HPP_
#define OBSERVABLES_HPP_

#include "../../types.hpp"
#include "../DataSample.hpp"
#include "EstimateAndError.hpp"

#include <functional>
#include <sstream>

class Parameters;
class Moments;
class MomentsEstimators;

enum ErrorCalculationMethod { bootstrap = 1, jackknife };
realFloat evaluateErrorBasedOnMethod(DataSample, ErrorCalculationMethod);

/*
 * TODO: So far the error method is not a private member of the class, since for the raw data only Jackknife is used!
 *       Think whether put it as private member and in case do it.
 */

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
 *
 *       This is also the reason why for the moment there are no static members with this function that is hard coded in the getter.
 */

class QuantityAbstract {
  public:
    QuantityAbstract(bool isMeanKnownToBeZero);
    virtual ~QuantityAbstract(){};
    const realFloat& estimate;
    const realFloat& error;

  protected:
    // Calculation from raw data
    void calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters);
    // Calculation for Reweighting
    void calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod,
                                      bool useMultipleEstimate);
    bool isMeanZero;
    EstimateAndError observableEstimateAndError;

  private:
    std::vector<DataSample> getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters);
    std::vector<DataSample> calculateNeededMomentsPerDataPoint(DataSample& dataSample);
    // Virtual method that must be provided by children classes
    virtual Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters) = 0;
    virtual void printCorrectBinningInformation(const Parameters& parameters) = 0;
    virtual functionForEstimatorsForJackknife getFunctionToBeAppliedToEstimatorsForJackknife() = 0;
    virtual functionForEstimators getFunctionToBeAppliedToEstimators(bool useMultipleEstimate = false) = 0;
    virtual functionForObservable getFunctionToCalculateObservable(bool useMultipleEstimate = false) = 0;
    virtual std::initializer_list<unsigned int> getNeededMoments() = 0;
};

/*
 * TODO: The following class should be a container for the quantities above and it should allow the possibility to set only some.
 *       One idea could be to use the access operator with the observable name as argument to recover values and do something
 *       when it is asked for an unset quantity (implement operator[] and function at or something like that)
 */

/*
 * This class is meant to be the only one to handle quantities by composition.
 */
class Quantities {
  public:
    Quantities() : mean(NAN, NAN), susceptibility(NAN, NAN), skewness(NAN, NAN), kurtosis(NAN, NAN)
    {
        observableNames.push_back("mean");
        observableNames.push_back("variance");
        observableNames.push_back("skewness");
        observableNames.push_back("kurtosis");
    }

    std::string getMetaInformation()
    {
        std::string metaInfos = "";
        for (unsigned int index = 0; index < observableNames.size(); index++) {
            metaInfos += observableNames[index] + "\t\t\terror\t\t\t";
        }
        return metaInfos;
    }

    std::string getObservablesAsString()
    {
        std::stringstream values;
        values.precision(12);
        values << std::scientific;
        values << mean.estimate << "\t" << mean.error << "\t";
        values << susceptibility.estimate << "\t" << susceptibility.error << "\t";
        values << skewness.estimate << "\t" << skewness.error << "\t";
        values << kurtosis.estimate << "\t" << kurtosis.error;
        return values.str();
    }

    /*
     * Here in the following the observables we deal with around in the
     * library, together with their definition (thinking to a sample set X
     * with mean mu, x1,x2,x3,x4 are the 1st,2nd,3rd,4th moment of the sample)
     */
    EstimateAndError mean;            // <x>                            --->  x1
    EstimateAndError susceptibility;  // <(x-mu)^2>                     --->  x2-x1^2
    EstimateAndError skewness;        // <(x-mu)^3> / <(x-mu)^2>^(3/2)  --->  (x3-3x2*x1+2*x1^3)/(x2-x1^2)^(3/2)
    EstimateAndError kurtosis;        // <(x-mu)^4> / <(x-mu)^2>^2      --->  (x4-4*x3*x1+6*x2*x1^2-3*x1^4)/(x2-x1^2)^2

    // TODO: Implement functions to calculate here the observables given either the DataSample or
    //      the moments and the estimators (the latter needed in Reweighting). The idea would be to
    //      have an object to which delegate the calculation of the observables all together: it could
    //      use the object in DataSampleAnalyzer that should be then local to the Observables.cpp file!

    std::vector<std::string> observableNames;  // TODO: to be removed!
};

#endif /* OBSERVABLES_HPP_ */
