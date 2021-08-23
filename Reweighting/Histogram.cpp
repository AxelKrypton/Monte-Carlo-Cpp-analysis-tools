/*
 *
 *  Copyright (c) 2019 David Leemueller
 *  Copyright (c) 2019-2020 Alessandro Sciarra
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

#include "Histogram.hpp"

#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"

#include <cmath>
#include <iostream>
#include <tgmath.h>
#include <valarray>

static std::map<int, double> insertBinsWithZeroHeight(const std::map<int, double>&);
static std::map<int, EstimateAndError> insertBinsWithZeroEstimateAndError(const std::map<int, EstimateAndError>&);
static std::multimap<int, double> includeBinsWithZeroHeightInMultiMap(std::multimap<int, double>);

/*****************************************************************************************/

/*
 * Having a fixpoint at 0, we divide the whole real axis into bins.
 * In the calculation, some of the bins get filled depending on the observables.
 * After that we want to extract only the significant part of the histogram
 * for further treatment.
 */

Histogram::Histogram(double binsizeIn, double anchorIn) : anchor(anchorIn), binsize(binsizeIn)
{
    if (binsize <= 0) {
        throw std::logic_error("Histogram can't be created with negative or zero binsize.");
    }
}

int Histogram::getNumberOfBins(bool includeZeroBins) const
{
    if (histogram.empty())
        return 0;
    else
        return includeZeroBins ? static_cast<int>((getMaxXvalue() - getMinXvalue()) / binsize) : histogram.size();
}

double Histogram::getBinsize() const
{
    return binsize;
}

double Histogram::getAnchor() const
{
    return anchor;
}

std::vector<double> Histogram::getHeightsOfBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histogram);
    if (includeZeroBins) {
        copyOfHisto = insertBinsWithZeroHeight(histogram);
    }

    std::vector<double> heights;
    for (std::pair<const int, double> bin : copyOfHisto) {
        heights.push_back(bin.second);
    }
    return heights;
}

double Histogram::getHeightOfSpecificBin(double obsvalue) const
{
    int whichbin = static_cast<int>(ceil((obsvalue - anchor) / binsize - 0.5));
    std::map<int, double> copyOfHisto(histogram);
    return copyOfHisto[whichbin];
}

double Histogram::getMaxXvalue() const
{
    std::map<int, double>::const_reverse_iterator it = histogram.rbegin();
    return (it->first + 0.5) * binsize + anchor;
}

double Histogram::getMinXvalue() const
{
    std::map<int, double>::const_iterator it = histogram.begin();
    return (it->first - 0.5) * binsize + anchor;
}

std::vector<std::pair<double, double>> Histogram::getBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histogram);
    if (includeZeroBins) {
        copyOfHisto = insertBinsWithZeroHeight(histogram);
    }

    std::vector<std::pair<double, double>> bins;
    for (std::pair<const int, double> bin : copyOfHisto) {
        bins.push_back(std::pair<double, double>((bin.first - 0.5) * binsize + anchor, (bin.first + 0.5) * binsize + anchor));
    }
    return bins;
}

std::vector<double> Histogram::getMiddleOfBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histogram);
    if (includeZeroBins) {
        copyOfHisto = insertBinsWithZeroHeight(histogram);
    }

    std::vector<double> middleOfBins;
    for (std::pair<const int, double> bin : copyOfHisto) {
        middleOfBins.push_back(bin.first * binsize + anchor);
    }
    return middleOfBins;
}

/*
 * The following operator allows us to fill the histogram by writing
 * 'histo[obsvalue]+=term'. What happens there is that the operator looks for
 * the bin where the value of 'obsvalue' is in.
 * The height of that bin is then increased by 'term'.
 */

double& Histogram::operator[](double obsvalue)
{
    /*
     * To understand which bin the obs is in, we subtract half binsize (because
     * by definition the anchor is in the middle of the bin) then we subract
     * the anchor and we finally divide by binsize (taking the integer after the
     * result):
     *              ceil(obsvalue-0.5*binsize-anchor)/binsize
     */
    int whichbin = static_cast<int>(ceil((obsvalue - anchor) / binsize - 0.5));
    return histogram[whichbin];
}

std::map<int, double>& Histogram::operator-=(double shiftTerm)
{
    for (std::pair<const int, double>& bin : histogram) {
        /*
         * The following might make a height negative, but it can happen in reweighting
         * (indeed it does, since we use logarithms there) and therefore we allow it.
         */
        // histogram[it->first]-=shiftTerm;
        bin.second -= shiftTerm;
    }
    return histogram;
}

void Histogram::exponentiateHeights()
{
    for (std::pair<const int, double>& bin : histogram) {
        bin.second = std::exp(bin.second);
    }
}

void Histogram::shift(double delta)
{
    anchor += delta;
}

void Histogram::normalize()
{
    double areaOfHistogram = 0.0;
    for (std::pair<const int, double> bin : histogram) {
        areaOfHistogram += bin.second * binsize;
    }
    for (std::pair<const int, double>& bin : histogram) {
        bin.second /= areaOfHistogram;
    }
}

/***********************************************
 *Histogram Estimator class members get defined*
 ***********************************************/
HistogramEstimator::HistogramEstimator(double binsizeIn) : binsize(binsizeIn)
{
    if (binsize <= 0) {
        throw std::logic_error("HistogramEstimator can't be created with negative or zero binsize.");
    }
}

/*
 * The following function returns the heights of the histogram estimators.
 * Since now for every bin there are multiple heights it returns a vector of vectors.
 * The first vector are the bins of the histogram. The second vector are the different heights from the estimators.
 */
std::vector<std::vector<double>> HistogramEstimator::getMultipleHeightsOfBins(bool includeZeroBins) const
{
    std::multimap<int, double> copyOfHisto(histogramEstimators);
    if (includeZeroBins) {
        copyOfHisto = includeBinsWithZeroHeightInMultiMap(histogramEstimators);
    }
    std::vector<std::vector<double>> multipleHeightsOfBins;
    std::multimap<int, double>::iterator it;
    for (it = copyOfHisto.begin(); it != copyOfHisto.end(); it++) {
        int whichbin = it->first;
        std::vector<double> heights;
        typedef std::multimap<int, double>::iterator copyOfHistoIt;
        std::pair<copyOfHistoIt, copyOfHistoIt> range = copyOfHisto.equal_range(whichbin);
        for (copyOfHistoIt histoIt = range.first; histoIt != range.second; histoIt++) {
            heights.push_back(histoIt->second);
        }
        // In the multimap keys can appear multiple times. After extracting the values of one key the iterator skips the same keys and goes to the next one.
        for (int i = 0; i < std::distance(range.first, range.second) - 1; i++) {
            it++;
        }
        multipleHeightsOfBins.push_back(heights);
    }
    return multipleHeightsOfBins;
}

std::vector<double> HistogramEstimator::getHeightsOfBin(double obsvalue)
{
    int whichbin = static_cast<int>(ceil(obsvalue / binsize - 0.5));
    std::vector<double> heights;
    typedef std::multimap<int, double>::iterator histogramEstimatorsIt;
    std::pair<histogramEstimatorsIt, histogramEstimatorsIt> range = histogramEstimators.equal_range(whichbin);
    for (histogramEstimatorsIt histoIt = range.first; histoIt != range.second; histoIt++) {
        heights.push_back(histoIt->second);
    }
    return heights;
}

void HistogramEstimator::insert(double obsvalue, std::vector<double> heights)
{
    if (heights.empty())
        throw std::logic_error("Estimator can't be filled with an empty vector of heights!");
    int whichbin = static_cast<int>(ceil(obsvalue / binsize - 0.5));
    for (size_t i = 0; i < heights.size(); i++)
        histogramEstimators.insert(std::pair<int, double>(whichbin, heights[i]));
}

/******************************************************
 **Probability Distribution class members get defined**
 ******************************************************/
ProbabilityDistribution::ProbabilityDistribution() : anchor(NAN), binsize(NAN) {}

ProbabilityDistribution::ProbabilityDistribution(Histogram reweightedHistogram, HistogramEstimator reweightedHistogramEstimator,
                                                 ErrorCalculationMethod errorMethod)
{
    binsize = reweightedHistogram.getBinsize();
    anchor = reweightedHistogram.getAnchor();
    std::vector<double> middleOfBins = reweightedHistogram.getMiddleOfBins();
    for (double middleOfBin : middleOfBins) {
        std::vector<double> tmpHeightsOfOneBin = reweightedHistogramEstimator.getHeightsOfBin(middleOfBin);
        std::valarray<double> heightsOfOneBin(tmpHeightsOfOneBin.data(), tmpHeightsOfOneBin.size());
        DataSample heightDataOfOneBin(heightsOfOneBin);
        realFloat estimate = 0.0, error = 0.0;
        estimate = reweightedHistogram[middleOfBin];
        error = evaluateErrorBasedOnMethod(heightDataOfOneBin, errorMethod);
        this->operator[](middleOfBin) = EstimateAndError(estimate, error);
    }
}

int ProbabilityDistribution::getNumberOfBins(bool includeZeroBins) const
{
    if (probabilityDistribution.empty())
        return 0;
    else
        return includeZeroBins ? static_cast<int>((getMaxXvalue() - getMinXvalue()) / binsize) : probabilityDistribution.size();
}

double ProbabilityDistribution::getBinsize() const
{
    return binsize;
}

std::vector<EstimateAndError> ProbabilityDistribution::getHeightsOfBins(bool includeZeroBins) const
{
    std::map<int, EstimateAndError> copyOfProbabilityDistribution(probabilityDistribution);
    if (includeZeroBins) {
        copyOfProbabilityDistribution = insertBinsWithZeroEstimateAndError(probabilityDistribution);
    }

    std::vector<EstimateAndError> heights;
    for (std::pair<const int, EstimateAndError> bin : copyOfProbabilityDistribution) {
        heights.push_back(bin.second);
    }
    return heights;
}

EstimateAndError ProbabilityDistribution::getHeightOfSpecificBin(double obsvalue) const
{
    int whichbin = static_cast<int>(ceil((obsvalue - anchor) / binsize - 0.5));
    std::map<int, EstimateAndError> copyOfProbabilityDistribution(probabilityDistribution);
    EstimateAndError height = copyOfProbabilityDistribution[whichbin];
    return height;
}

double ProbabilityDistribution::getMaxXvalue() const
{
    std::map<int, EstimateAndError>::const_reverse_iterator it = probabilityDistribution.rbegin();
    return (it->first + 0.5) * binsize + anchor;
}

double ProbabilityDistribution::getMinXvalue() const
{
    std::map<int, EstimateAndError>::const_iterator it = probabilityDistribution.begin();
    return (it->first - 0.5) * binsize + anchor;
}

std::vector<std::pair<double, double>> ProbabilityDistribution::getBins(bool includeZeroBins) const
{
    std::map<int, EstimateAndError> copyOfProbabilityDistribution(probabilityDistribution);
    if (includeZeroBins) {
        copyOfProbabilityDistribution = insertBinsWithZeroEstimateAndError(probabilityDistribution);
    }

    std::vector<std::pair<double, double>> bins;
    for (std::pair<const int, EstimateAndError> bin : copyOfProbabilityDistribution) {
        bins.push_back(std::pair<double, double>((bin.first - 0.5) * binsize + anchor, (bin.first + 0.5) * binsize + anchor));
    }
    return bins;
}

std::vector<double> ProbabilityDistribution::getMiddleOfBins(bool includeZeroBins) const
{
    std::map<int, EstimateAndError> copyOfProbabilityDistribution(probabilityDistribution);
    if (includeZeroBins) {
        copyOfProbabilityDistribution = insertBinsWithZeroEstimateAndError(probabilityDistribution);
    }

    std::vector<double> middleOfBins;
    for (std::pair<const int, EstimateAndError> bin : copyOfProbabilityDistribution) {
        middleOfBins.push_back(bin.first * binsize + anchor);
    }
    return middleOfBins;
}

/*
 * The following operator allows us to fill the histogram by writing
 * 'histo[obsvalue]+=term'. What happens there is that the operator looks for
 * the bin where the value of 'obsvalue' is in.
 * The height of that bin is then increased by 'term'.
 */

EstimateAndError& ProbabilityDistribution::operator[](double obsvalue)
{
    /*
     * To understand which bin the obs is in, we subtract half binsize (because
     * by definition the anchor is in the middle of the bin) then we subract
     * the anchor and we finally divide by binsize (taking the integer after the
     * result):
     *              ceil(obsvalue-0.5*binsize-anchor)/binsize
     */
    int whichbin = static_cast<int>(ceil((obsvalue - anchor) / binsize - 0.5));
    return probabilityDistribution[whichbin];
}

/*****************************************STATIC FUNCTIONS*********************************************************************************************/

static std::map<int, double> insertBinsWithZeroHeight(const std::map<int, double>& histoWithoutZeroBins)
{
    std::map<int, double> filledHistogram(histoWithoutZeroBins);
    for (int i = filledHistogram.begin()->first; i <= filledHistogram.rbegin()->first; i++) {
        filledHistogram[i];  // I just want to fill with zero not existing bins and I use the map's access-operator feature of adding not
                             // existing elements
    }
    return filledHistogram;
}

static std::map<int, EstimateAndError> insertBinsWithZeroEstimateAndError(const std::map<int, EstimateAndError>& probDistributionWithoutZeroBins)
{
    std::map<int, EstimateAndError> filledProbDistribution(probDistributionWithoutZeroBins);
    for (int i = filledProbDistribution.begin()->first; i <= filledProbDistribution.rbegin()->first; i++) {
        filledProbDistribution[i];
    }
    return filledProbDistribution;
}

static std::multimap<int, double> includeBinsWithZeroHeightInMultiMap(std::multimap<int, double> histoWithoutZeroBins)
{
    std::multimap<int, double> filledHistogram(histoWithoutZeroBins);
    std::multimap<int, double>::iterator it;
    for (int i = filledHistogram.begin()->first; i <= filledHistogram.rbegin()->first; i++) {
        it = histoWithoutZeroBins.find(i);
        if (it == histoWithoutZeroBins.end())
            filledHistogram.insert(std::pair<int, double>(i, 0));  // Inserting bins with zero height
    }
    return filledHistogram;
}
