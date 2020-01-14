/*
 *
 *  Copyright (c)
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

#ifndef BOOTSTRAPANALYSIS_HPP_
#define BOOTSTRAPANALYSIS_HPP_

#include "DataSample.hpp"

/*
 * Here the Barkema's book is followed (in particular Eq.(3.41) at page 71). The name "Bootstrap Estimator" is not
 * common in the literature, it is used here just to mean the result of the calculation of the chosen observable on
 * the re-sampled data set
 */
realFloat calculateBootstrapError(DataSample& sampleWithBootstrapEstimators);

#endif /* BOOTSTRAPANALYSIS_HPP_ */
