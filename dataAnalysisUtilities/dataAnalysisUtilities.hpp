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
#include "../Quantities/EstimateAndError.hpp"
#include "DataSample.hpp"

/*
 * In all the quantities calculation, it is taken into account whether the mean is known to be zero or not.
 * This means that, if known, the mean is not estimated out from the sampleIn, but manually set to zero.
 */

realFloat evaluateErrorBasedOnMethod(DataSample, ErrorCalculationMethod);

void calcAutocorrelationAndErrorOfDataSample(DataSample& sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample& sample, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
