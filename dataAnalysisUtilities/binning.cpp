#include "binning.hpp"

#include "binnedDataSample.hpp"

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

