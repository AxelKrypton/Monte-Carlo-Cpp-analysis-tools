#include "Histogram.hpp"
#include<cmath>
#include<tgmath.h>
#include<iostream>

/*****************************************************************************************/

/* 
 * Having a fixpoint at 0, we divide the whole real axis into bins.
 * In the calculation, some of the bins get filled depending on the observables.
 * After that we want to extract only the significant part of the histogram
 * for further treatment.
 */

Histogram::Histogram(double binsizeIn)
{
    if(binsizeIn<=0)
    {
        throw std::logic_error("Histogram can't be created with negative or zero binsize.");
    }
    binsize=binsizeIn;
}

int Histogram::getNumberOfBins(includeZeroBins)
{
    int numberOfBins=(int)(getMaxXvalue()-getMinXvalue())/binsize;
    return includeZeroBins ? numberOfBins : histo.size();
}

double Histogram::getBinsize()
{
    return binsize;
}

std::vector<double> Histogram::getHeightsOfBins() const
{
    std::vector<double> heights;
    std::map<int, double>::const_iterator it;
    int i=0;
    for(it=histo.begin(); it!=histo.end(); it++)
    {
        heights.push_back(it->second);
    }
    return heights;
}

double Histogram::getMaxXvalue() const
{
    std::map<int, double>::const_reverse_iterator it;
    it=histo.rbegin();
    return (it->first+0.5)*binsize;
}

double Histogram::getMinXvalue() const
{
    std::map<int, double>::const_iterator it;
    it=histo.begin();
    return (it->first-0.5)*binsize;
}


std::vector<std::pair<double,double> > Histogram::getBins(includeZeroBins) const
{
    std::vector<std::pair<double,double> > bins;
    std::map<int, double>::const_iterator it;
    if(!includeZeroBins)
    {
        for(it=histo.begin(); it!=histo.end(); it++)
        {
            bins.push_back(std::pair<double, double>((it->first - 0.5)*binsize,(it->first + 0.5)*binsize));
        }
        return bins;
    }
    else
    {
        for(int i=histo.begin()->first; i<=histo.rbegin()->first; i++)
        {
            bins.push_back(std::pair<double, double>((i - 0.5)*binsize,(i + 0.5)*binsize));
        }
        return bins;
    }  
} 


/* 
 * The following operator allows us to fill the histogram by writing
 * 'histo[obsvalue]+=term'. What happens there is that the operator looks for
 * the bin where the value of 'obsvalue' is in. 
 * The height of that bin is then increased by 'term'.
 */

double& Histogram::operator[](double obsvalue)
{
    int whichbin;
    whichbin=ceil(obsvalue/binsize-0.5);
    return histo[whichbin]; 
}