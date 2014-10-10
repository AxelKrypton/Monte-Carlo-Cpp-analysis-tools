#ifndef BINNING_HPP_
#define BINNING_HPP_

#include "DataSample.hpp"
#include "../Parameters/Parameters.hpp"

DataSample performBinning(DataSample & rawData, const Parameters parameters);

DataSample performBinningFromNumberOfBins(DataSample & rawData, int numberOfBins, bool adjustDataSampleSizeToBinning = true, bool binningMustFitDataSampleSize = false);

DataSample performBinningFromBinsize(DataSample & rawData, int binsize, bool adjustDataSampleSizeToBinning = true, bool binningMustFitDataSampleSize = false);

#endif