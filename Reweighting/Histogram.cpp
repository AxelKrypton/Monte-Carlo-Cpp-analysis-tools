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
    std::map<int, double>::const_iterator it;
    for(it=copyOfHisto.begin(); it!=copyOfHisto.end(); it++)
    {
        heights.push_back(it->second);
    }
    return heights;
}

double Histogram::getMaxXvalue() const
{
    std::map<int, double>::const_reverse_iterator it;
    it=histogram.rbegin();
    return (it->first+0.5)*binsize + anchor;
}

double Histogram::getMinXvalue() const
{
    std::map<int, double>::const_iterator it;
    it=histogram.begin();
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
    std::map<int, double>::const_iterator it;
    for(it=copyOfHisto.begin(); it!=copyOfHisto.end(); it++)
    {
        bins.push_back(std::pair<double, double>((it->first - 0.5)*binsize+anchor,(it->first + 0.5)*binsize+anchor));
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
    std::map<int,double>::iterator it;
    for(it=histogram.begin(); it!=histogram.end(); it++)
    {
        /*
         * The following might make a height negative, but it can happen in reweighting
         * (indeed it does, since we use logarithms there) and therefore we allow it.
         */
        histogram[it->first]-=shiftTerm;
    }
    return histogram;
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

