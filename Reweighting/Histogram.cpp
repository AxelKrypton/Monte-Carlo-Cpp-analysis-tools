#include "Histogram.hpp"
#include<cmath>
#include<tgmath.h>
#include<iostream>

static std::map<int,double> insertBinsWithZeroHeight(std::map<int,double>);

/*****************************************************************************************/

/* 
 * Having a fixpoint at 0, we divide the whole real axis into bins.
 * In the calculation, some of the bins get filled depending on the observables.
 * After that we want to extract only the significant part of the histogram
 * for further treatment.
 */

Histogram::Histogram(double binsizeIn) : binsize(binsizeIn)
{
    if(binsize<=0)
    {
        throw std::logic_error("Histogram can't be created with negative or zero binsize.");
    }
}

int Histogram::getNumberOfBins(bool includeZeroBins)
{
    int numberOfBins=static_cast<int>((getMaxXvalue()-getMinXvalue())/binsize);
    return includeZeroBins ? numberOfBins : histo.size();
}

double Histogram::getBinsize()
{
    return binsize;
}

std::vector<double> Histogram::getHeightsOfBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histo);
    if(includeZeroBins){
        copyOfHisto = insertBinsWithZeroHeight(histo);
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
    it=histo.rbegin();
    return (it->first+0.5)*binsize;
}

double Histogram::getMinXvalue() const
{
    std::map<int, double>::const_iterator it;
    it=histo.begin();
    return (it->first-0.5)*binsize;
}


std::vector<std::pair<double,double> > Histogram::getBins(bool includeZeroBins) const
{
    std::map<int, double> copyOfHisto(histo);
    if(includeZeroBins)
    {
        copyOfHisto=insertBinsWithZeroHeight(histo);
    }   
    std::vector<std::pair<double,double> > bins;
    std::map<int, double>::const_iterator it;
    for(it=copyOfHisto.begin(); it!=copyOfHisto.end(); it++)
    {
        bins.push_back(std::pair<double, double>((it->first - 0.5)*binsize,(it->first + 0.5)*binsize));
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
    int whichbin=ceil(obsvalue/binsize-0.5);
    return histo[whichbin]; 
}

std::map<int,double>& Histogram::operator-=(double shiftTerm)
{
    std::map<int,double>::iterator it;
    for(it=histo.begin(); it!=histo.end(); it++)
    {
        this->histo.at(it->first)-=shiftTerm;
    }
    return histo;
}

/**************************************************************************************/

static std::map<int,double> insertBinsWithZeroHeight(std::map<int,double> histoWithoutZeroBins)
{
    std::map<int,double> filledHisto(histoWithoutZeroBins);
    for(int i=filledHisto.begin()->first; i<=filledHisto.rbegin()->first; i++)
        {
            filledHisto[i]; //I just want to fill with zero not existing bins and I use the map's access-operator feature of adding not existing elements
        }
    return filledHisto;
}

