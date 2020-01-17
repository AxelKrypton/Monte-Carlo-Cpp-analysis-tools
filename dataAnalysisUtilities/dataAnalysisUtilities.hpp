/*
 *
 *  Copyright (c) 2014 Christopher Pinke
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

#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include "../Parameters/Parameters.hpp"
#include "DataSample.hpp"
#include "EstimateAndError.hpp"

/*
 * In all the quantities calculation, it is taken into account whether the mean is known to be zero or not.
 * This means that, if known, the mean is not estimated out from the sampleIn, but manually set to zero.
 */

/*
 * The following 4 functions DO BINNING(!) on sampleIn and then calculate the observable
 *
 * ATTENTION: The fact that the functions below do binning means that they will modify
 *            the sampleIn cutting at the end some data. Then it is CRUCIAL to pass the
 *            sampleIn by value and not by reference (otherwise, in case of successive
 *            analysis the sample for the second analysis would be the output of the first,
 *            namely not the full data-set in the user file).
 */

EstimateAndError calcMeanAndErrorOfDataSample(DataSample sampleIn, Parameters parameters);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample sampleIn, Parameters parameters);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample sampleIn, Parameters parameters);
EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample sampleIn, Parameters parameters);
// The following two functions are not used in the rest of the project. They are implemented here
// for testing reasons (in tests binning is often not needed and it could be annoying to use the
// functions above). TODO: Improve using the functions above in tests.
EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample& sampleIn, bool isMeanKnownToBeZero = false);
EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample& sampleIn, bool isMeanKnownToBeZero = false);

void calcAutocorrelationAndErrorOfDataSample(DataSample& sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
