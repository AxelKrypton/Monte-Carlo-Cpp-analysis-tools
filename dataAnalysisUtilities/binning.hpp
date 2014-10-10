#ifndef BINNING_HPP_
#define BINNING_HPP_

#include "DataSample.hpp"

DataSample performBinningFromNumberOfBins(DataSample & rawData, int numberOfBins, bool adjustDataSampleSizeToBinning = true, bool binningMustFitDataSampleSize = false);

DataSample performBinningFromBinsize(DataSample & rawData, int binsize, bool adjustDataSampleSizeToBinning = true, bool binningMustFitDataSampleSize = false);

#endif