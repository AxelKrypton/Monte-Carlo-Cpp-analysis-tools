#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_
#include "../dataAnalysisUtilities/EstimateAndError.hpp"
#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"
#include "../types.hpp"
#include<vector>
#include<math.h>
#include<map>

/* 
 * This class creates an object which is constructed specifying binsize
 * and an anchor point. The histogram is internally stored as
 * map<int,double> and works as a container that stores
 * the bin-number as the first and the height as the second entry.
 * The zero-th bin is that having the anchor as middle point.
 * The lower edge of a bin is part of that bin while the upper edge is part of the next bin.
 * The map gets a new entry when it gets filled.
 */

class Histogram
{
public:
    Histogram() = delete;
    Histogram(double, double = 0.0);
    //Getters
    int getNumberOfBins(bool = false) const;
    double getBinsize() const;
    double getAnchor() const;
    std::vector<double> getHeightsOfBins(bool = false) const;
    double getHeightOfSpecificBin(double) const;
    double getMaxXvalue() const;
    double getMinXvalue() const;
    std::vector<std::pair<double,double> > getBins(bool = false) const;
    std::vector<double> getMiddleOfBins(bool = false) const;
    
    double& operator[](double);
    std::map<int,double>& operator-=(double);
    void exponentiateHeights();
    void shift(double);
    void normalize();

private:
    double anchor;
    double binsize;
    std::map<int,double> histogram;
};


class HistogramEstimator
{
public:
    HistogramEstimator() = delete;
    HistogramEstimator(double);
    std::vector<std::vector<double> > getMultipleHeightsOfBins(bool = false) const;
    std::vector<double> getHeightsOfBin(double);

    void insert(double, std::vector<double>);

private:
    /*The Estimator doesn't need an anchor since in the MomentsReweighter class a valarray
     *of Histograms get filled, restored and then used to fill the Estimators. So the Estimators
     *are already the "restored" ones.
     */
    double binsize;
    std::multimap<int,double> histogramEstimators;
};

class ProbabilityDistribution
{
public:
    ProbabilityDistribution();
    ProbabilityDistribution(Histogram, HistogramEstimator, ErrorCalculationMethod);
    //Getters
    int getNumberOfBins(bool = false) const;
    double getBinsize() const;
    std::vector<EstimateAndError> getHeightsOfBins(bool = false) const;
    EstimateAndError getHeightOfSpecificBin(double) const;
    double getMaxXvalue() const;
    double getMinXvalue() const;
    std::vector<std::pair<double,double> > getBins(bool = false) const;
    std::vector<double> getMiddleOfBins(bool = false) const;

    EstimateAndError& operator[](double);

private:
    double anchor;
    double binsize;
    std::map<int,EstimateAndError> probabilityDistribution;
};

#endif /* HISTOGRAM_HPP_ */
