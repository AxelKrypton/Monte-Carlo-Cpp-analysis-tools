/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2014,2016,2020 Alessandro Sciarra
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

#include "binnedDataSample.hpp"

#include "../IO/io_utilities.hpp"
#include "../executables/exceptions.hpp"

DataSampleBasic BinnedDataSample::performBinning(DataSampleBasic sampleIn)
{
    DEBUG(std::cout << "# Perform binning with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl);
    DataSampleBasic binnedDataSample(numberOfBins);
    for (int iteration = 0; iteration < numberOfBins; iteration++) {
        binnedDataSample[iteration] = DataSample(sampleIn.sampleSlice(iteration * binsize, binsize, 1)).getNthMoment(1);
    }
    return binnedDataSample;
}

void BinnedDataSample::checkDiscardedElements(int valueIn, int elementsOfSample)
{
    discardedElements = elementsOfSample % valueIn;
    if (discardedElements != 0) {
        DEBUG(std::cout << "# Warning: " << valueIn << " does not divide " << elementsOfSample << ": " << discardedElements
                        << " elements are discarded!\n");
        binningFitsBinsize = false;
        if (binningMustFitSize) {
            throw wrongBinningParameter();
        }
    } else {
        binningFitsBinsize = true;
    }
}

BinnedDataSampleFromNumberOfBins::BinnedDataSampleFromNumberOfBins(DataSampleBasic sampleIn, int numberOfBinsIn, bool requireBinningToMatchSize)
{
    binningMustFitSize = requireBinningToMatchSize;
    numberOfBins = numberOfBinsIn;
    checkIfNumberOfBinsIsValid(sampleIn.getNumberOfElements());
    calcBinsize(sampleIn.getNumberOfElements());
    setValues(performBinning(sampleIn));
}

static void checkIfBinningParameterIsValid(int valueIn, std::string descriptionIn, int numberOfElements)
{
    if (valueIn <= 0)
        throw std::invalid_argument("Cannot perform binning with " + descriptionIn + " zero or less!");
    if (valueIn > numberOfElements)
        throw std::invalid_argument("Cannot perform binning with " + descriptionIn + " bigger than number of datapoints!");
}

void BinnedDataSampleFromNumberOfBins::checkIfNumberOfBinsIsValid(int elementsOfSample)
{
    checkIfBinningParameterIsValid(numberOfBins, "numberOfBins", elementsOfSample);
}

void BinnedDataSampleFromNumberOfBins::calcBinsize(int elementsOfSample)
{
    checkDiscardedElements(numberOfBins, elementsOfSample);
    binsize = elementsOfSample / numberOfBins;
}

void BinnedDataSampleFromBinsize::checkIfBinsizeIsValid(int elementsOfSample)
{
    checkIfBinningParameterIsValid(binsize, "binsize", elementsOfSample);
}

void BinnedDataSampleFromBinsize::calcNumberOfBins(int elementsOfSample)
{
    checkDiscardedElements(binsize, elementsOfSample);
    numberOfBins = elementsOfSample / binsize;
}

BinnedDataSampleFromBinsize::BinnedDataSampleFromBinsize(DataSampleBasic sampleIn, int binsizeIn, bool requireBinningToMatchSize)
{
    binningMustFitSize = requireBinningToMatchSize;
    binsize = binsizeIn;
    checkIfBinsizeIsValid(sampleIn.getNumberOfElements());
    calcNumberOfBins(sampleIn.getNumberOfElements());
    setValues(performBinning(sampleIn));
}

void resizeRawDataSample(DataSample& rawData, BinnedDataSample& binnedData, bool adjustDataSampleSizeToBinning)
{
    if (adjustDataSampleSizeToBinning && ! binnedData.doesBinningFitBinsize()) {
        DEBUG(std::cout << "# Adjusting data sample size..." << std::endl);
        rawData = rawData.removeLastNElements(binnedData.getNumberOfDiscardedElements());
    } else if (! binnedData.doesBinningFitBinsize()) {
        DEBUG(std::cout << "# WARNING: Elements are discarded for binned quantities only!" << std::endl);
    }
}
