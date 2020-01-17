/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2014,2020 Alessandro Sciarra
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

#ifndef BINNEDDATASAMPLE_HPP_
#define BINNEDDATASAMPLE_HPP_

#include "DataSample.hpp"

class BinnedDataSample : public DataSample {
  public:
    bool doesBinningFitBinsize() { return binningFitsBinsize; }
    int getNumberOfDiscardedElements() { return discardedElements; }

  protected:
    /**
     * Binning.
     * Following B.A. Berg,
     * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
     * p.52.
     */
    DataSampleBasic performBinning(DataSampleBasic sampleIn);
    void checkDiscardedElements(int valueIn, std::string descriptionIn, int elementsOfSample);

    int numberOfBins;
    int binsize;
    int discardedElements;
    bool binningMustFitSize;
    bool binningFitsBinsize;
    bool warningOutput;
};

class BinnedDataSampleFromNumberOfBins : public BinnedDataSample {
  public:
    BinnedDataSampleFromNumberOfBins(DataSampleBasic sampleIn, int numberOfBinsIn, bool requireBinningToMatchSize = false,
                                     bool warningOutputIn = true);

  protected:
    void checkIfNumberOfBinsIsValid(int elementsOfSample);
    void calcBinsize(int elementsOfSample);
};

class BinnedDataSampleFromBinsize : public BinnedDataSample {
  public:
    BinnedDataSampleFromBinsize(DataSampleBasic sampleIn, int binsize, bool requireBinningToMatchSize = false, bool warningOutputIn = true);

  protected:
    void checkIfBinsizeIsValid(int elementsOfSample);
    void calcNumberOfBins(int elementsOfSample);
};

void resizeRawDataSample(DataSample& rawData, BinnedDataSample& binnedData, bool adjustDataSampleSizeToBinning);

#endif /* BINNEDDATASAMPLE_HPP_ */
