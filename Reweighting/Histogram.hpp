#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_
#include<vector>
#include<math.h>
#include<map>

/* 
 * This class creates an object which is constructed only by binsize.
 * 'Histo' is a map<int,double> and works as a container that stores 
 * the binnumber as the first and the height as the second entry. 
 * 0 is used as the reference point for the histogram and is the middle
 * of the 0th bin. The Histogram gets a new entry when it gets filled.
 */

class Histogram
{
public:
    Histogram(double binsizeIn); 
    //Getters
    int getNumberOfBins(bool);
    double getBinsize();
    std::vector<double> getHeightsOfBins() const;
    double getMaxXvalue() const;
    double getMinXvalue() const;
    std::vector<std::pair<double,double> > getBins(bool) const;
    
    double& operator[](double obsvalue);

private:
    double binsize;
    std::map<int,double> histo;
    bool includeZeroBins;
};

#endif /* HISTOGRAM_HPP_ */