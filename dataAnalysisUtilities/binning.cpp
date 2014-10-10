#include "binning.hpp"

#include "binnedDataSample.hpp"

DataSample performBinning(DataSample & rawData, const Parameters parameters)
{
	if ( parameters.useBinning )
	{
			std::cout << "Perform binning on data sample..." << std::endl;
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

