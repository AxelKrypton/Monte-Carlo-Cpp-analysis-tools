/*
 *
 *  Copyright (c) 2015,2019-2020 Alessandro Sciarra
 *  Copyright (c) 2019 David Leemueller
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

#ifndef IO_UTILITIES_REWEIGHTING_HPP_
#define IO_UTILITIES_REWEIGHTING_HPP_

#include "../Reweighting/Histogram.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"
#include "../types.hpp"

#include <iostream>
#include <vector>

void writeLqcdReweightedObservablesToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                          const std::vector<std::vector<Observables>>& reweightedData, std::string outputfilePrefix);
void writeLqcdReweightedObservablesEstimatorsToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                                    const std::vector<std::vector<std::map<std::string, DataSample>>>& reweightedEstimators,
                                                    std::string outputfilePrefix);
void writeLqcdReweightedProbabilityDistributionsToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                                       const std::vector<std::vector<ProbabilityDistribution>>& reweightedProbabilityDistribution,
                                                       std::string outputfilePrefix);

#endif /* IO_UTILITIES_REWEIGHTING_HPP_ */
