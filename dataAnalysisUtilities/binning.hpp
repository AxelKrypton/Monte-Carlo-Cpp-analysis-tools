/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2020 Alessandro Sciarra
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

#ifndef BINNING_HPP_
#define BINNING_HPP_

#include "../Parameters/Parameters.hpp"
#include "DataSample.hpp"

DataSample performBinning(DataSample& rawData, const Parameters parameters);

DataSample performBinningFromNumberOfBins(DataSample& rawData, int numberOfBins, bool adjustDataSampleSizeToBinning = true,
                                          bool binningMustFitDataSampleSize = false);

DataSample performBinningFromBinsize(DataSample& rawData, int binsize, bool adjustDataSampleSizeToBinning = true,
                                     bool binningMustFitDataSampleSize = false);

#endif
