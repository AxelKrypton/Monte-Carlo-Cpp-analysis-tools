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
#include "Mean.hpp"

#include <functional>
#include <sstream>

class Parameters;
class Moments;
class MomentsEstimators;

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
    Quantities();
    std::string getMetaInformation();
    std::string getObservablesAsString();

    /*
     * Here in the following the observables we deal with around in the
     * library, together with their definition (thinking to a sample set X
     * with mean mu, x1,x2,x3,x4 are the 1st,2nd,3rd,4th moment of the sample)
     */
    EstimateAndError mean;      // <x>                            --->  x1
    EstimateAndError variance;  // <(x-mu)^2>                     --->  x2-x1^2
    EstimateAndError skewness;  // <(x-mu)^3> / <(x-mu)^2>^(3/2)  --->  (x3-3x2*x1+2*x1^3)/(x2-x1^2)^(3/2)
    EstimateAndError kurtosis;  // <(x-mu)^4> / <(x-mu)^2>^2      --->  (x4-4*x3*x1+6*x2*x1^2-3*x1^4)/(x2-x1^2)^2

    // TODO: Implement functions to calculate here the observables given either the DataSample or
    //      the moments and the estimators (the latter needed in Reweighting). The idea would be to
    //      have an object to which delegate the calculation of the observables all together: it could
    //      use the object in DataSampleAnalyzer that should be then local to the Observables.cpp file!

    std::vector<std::string> observableNames;  // TODO: to be removed!
};

#endif /* OBSERVABLES_HPP_ */
