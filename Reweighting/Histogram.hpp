#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_
#include<vector>
#include<math.h>
#include<map>

/* 
 * This class creates an object which is constructed specifying binsize
 * and an anchor point. The histogram is internally stored as
 * map<int,double> and works as a container that stores
 * the bin-number as the first and the height as the second entry.
 * The zero-th bin is that having the anchor as middle point.
 * The map gets a new entry when it gets filled.
 */

class Histogram
{
public:
    Histogram() = delete;
    Histogram(double, double = 0.0);
    //Getters
    int getNumberOfBins(bool = false);
    double getBinsize();
    std::vector<double> getHeightsOfBins(bool = false) const;
    double getMaxXvalue() const;
    double getMinXvalue() const;
    std::vector<std::pair<double,double> > getBins(bool = false) const;
    
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

#endif /* HISTOGRAM_HPP_ */
