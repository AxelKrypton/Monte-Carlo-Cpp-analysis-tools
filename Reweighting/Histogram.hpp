#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_
#include<vector>
#include<math.h>
#include<map>

/* 
 * This class creates an object which is constructed only by binsize.
 * 'histo' is a map<int,double> and works as a container that stores
 * the binnumber as the first and the height as the second entry. 
 * 0 is used as the reference point for the histogram and is the middle
 * of the 0th bin. The Histogram gets a new entry when it gets filled.
 */

class Histogram
{
public:
    Histogram() = delete;
    Histogram(double);
    //Getters
    int getNumberOfBins(bool = false);
    double getBinsize();
    std::vector<double> getHeightsOfBins(bool = false) const;
    double getMaxXvalue() const;
    double getMinXvalue() const;
    std::vector<std::pair<double,double> > getBins(bool = false) const;
    
    double& operator[](double);
    std::map<int,double>& operator-=(double);

private:
    double binsize;
    std::map<int,double> histo;
};

#endif /* HISTOGRAM_HPP_ */
