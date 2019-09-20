#include "Histogram.hpp"
#include<cmath>
#include<tgmath.h>
#include<iostream>

static std::map<int,double> insertBinsWithZeroHeight(const std::map<int,double>&);

/*****************************************************************************************/

/* 
 * Having a fixpoint at 0, we divide the whole real axis into bins.
 * In the calculation, some of the bins get filled depending on the observables.
 * After that we want to extract only the significant part of the histogram
 * for further treatment.
 */

Histogram::Histogram(double binsizeIn, double anchorIn) : anchor(anchorIn), binsize(binsizeIn)
{
    if(binsize<=0)
    {
        throw std::logic_error("Histogram can't be created with negative or zero binsize.");
    }
}

int Histogram::getNumberOfBins(bool includeZeroBins)
{
    if(histogram.empty())
        return 0;
    else
        return includeZeroBins ? static_cast<int>((getMaxXvalue()-getMinXvalue())/binsize) : histogram.size();
}

double Histogram::getBinsize()
{
    return binsize;
}

std::vector<double> Histogram::getHeightsOfBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histogram);
    if(includeZeroBins){
        copyOfHisto = insertBinsWithZeroHeight(histogram);
    }

    std::vector<double> heights;
    for(std::pair<const int, double> bin : copyOfHisto)
    {
        heights.push_back(bin.second);
    }
    return heights;
}

double Histogram::getMaxXvalue() const
{
    std::map<int, double>::const_reverse_iterator it = histogram.rbegin();
    return (it->first+0.5)*binsize + anchor;
}

double Histogram::getMinXvalue() const
{
    std::map<int, double>::const_iterator it = histogram.begin();
    return (it->first-0.5)*binsize + anchor;
}


std::vector<std::pair<double,double> > Histogram::getBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histogram);
    if(includeZeroBins)
    {
        copyOfHisto=insertBinsWithZeroHeight(histogram);
    }

    std::vector<std::pair<double,double> > bins;
    for(std::pair<const int, double> bin : copyOfHisto)
    {
        bins.push_back(std::pair<double, double>((bin.first - 0.5)*binsize+anchor,(bin.first + 0.5)*binsize+anchor));
    }
    return bins;
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
    int whichbin=ceil((obsvalue-anchor)/binsize-0.5);
    return histogram[whichbin]; 
}

std::map<int,double>& Histogram::operator-=(double shiftTerm)
{
    for(std::pair<const int,double>& bin : histogram)
    {
        /*
         * The following might make a height negative, but it can happen in reweighting
         * (indeed it does, since we use logarithms there) and therefore we allow it.
         */
        //histogram[it->first]-=shiftTerm;
        bin.second -= shiftTerm;
    }
    return histogram;
}

void Histogram::exponentiateHeights()
{
    for(std::pair<const int,double>& bin : histogram)
    {
        bin.second=std::exp(bin.second);
    }
}

void Histogram::shift(double delta)
{
    anchor += delta;
}

/**************************************************************************************/

static std::map<int,double> insertBinsWithZeroHeight(const std::map<int,double>& histoWithoutZeroBins)
{
    std::map<int,double> filledHistogram(histoWithoutZeroBins);
    for(int i=filledHistogram.begin()->first; i<=filledHistogram.rbegin()->first; i++)
    {
        filledHistogram[i]; //I just want to fill with zero not existing bins and I use the map's access-operator feature of adding not existing elements
    }
    return filledHistogram;
}

