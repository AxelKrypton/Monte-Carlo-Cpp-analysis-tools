/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2015-2016,2020 Alessandro Sciarra
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

#include "binning.hpp"

#include "binnedDataSample.hpp"

DataSample performBinning(DataSample & rawData, const Parameters parameters)
{
	if ( ! parameters.doNotUseBinning )
	{
			std::cout << "# Perform binning on data sample..." << std::endl;
			if ( parameters.useNumberOfBinsForBinning)
			{
				return performBinningFromNumberOfBins(rawData, parameters.numberOfBins, parameters.adjustDataSampleSizeToBinning, parameters.binningMustFitDataSampleSize);
			}
			else
			{
				return performBinningFromBinsize(rawData, parameters.binsize, parameters.adjustDataSampleSizeToBinning, parameters.binningMustFitDataSampleSize);
			}
	}
	else
	{
		throw std::invalid_argument("Binning requested, but have different inputparameters!");
	}
}

DataSample performBinningFromNumberOfBins(DataSample & rawData, int numberOfBins, bool adjustDataSampleSizeToBinning, bool binningMustFitDataSampleSize)
{
	BinnedDataSample binnedData;
	binnedData = (BinnedDataSample) BinnedDataSampleFromNumberOfBins(rawData, numberOfBins, binningMustFitDataSampleSize);
	
	resizeRawDataSample(rawData, binnedData, adjustDataSampleSizeToBinning);
	
	return binnedData;
}

DataSample performBinningFromBinsize(DataSample & rawData, int binsize, bool adjustDataSampleSizeToBinning, bool binningMustFitDataSampleSize)
{
	BinnedDataSample binnedData;
	binnedData = (BinnedDataSample) BinnedDataSampleFromBinsize(rawData, binsize, binningMustFitDataSampleSize);
	
	resizeRawDataSample(rawData, binnedData, adjustDataSampleSizeToBinning);
	
	return binnedData;
}

