/*
 *
 *  Copyright (c) 2016,2018-2020 Alessandro Sciarra
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

#ifndef MOMENTS_HPP_
#define MOMENTS_HPP_

#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../types.hpp"

#include <map>

/*
 * NOTE: In order to handle the possibility to have several estimates per moment in the Moment class
 *       and several sets of estimators per moment in the MomentsEstimators class, we use a std::multimap
 *       member. Then we cannot use the access operator[] in the standard way, i.e. both to get and to set
 *       an entry. This is related to the fact that multimap has no operator[] defined. Another thing it
 *       would be cool to have is a method that gets the number of the moment and returns either a single
 *       object or a set of objects (in the case several were set). Nevertheless overload based on return
 *       value is not allowed in C++. So we decided to do in this way. We have an insert method to set
 *       elements. Then we use the operator[] to get a single value (checking for this case) and we use
 *       the operator() to get a set of values (checking for this case).
 *       TODO: In the functions getFunctionTo[...] we should do a try and catch block when asking the Moments
 *             or the MomentsEstimators with the operator() in the useMultipleEstimate case. This is because,
 *             in general, it could happen the (stupid) case in which the reweighting using multiple columns
 *             is done but there is one only multiple column. Then one only value is set and the operator()
 *             throws an exception.
 */
class Moments {
  public:
    Moments();
    void insert(const unsigned int& whichMoment, const realFloat& momentValue);
    realFloat operator[](const unsigned int& whichMoment);
    std::vector<realFloat> operator()(const unsigned int& whichMoment);

  private:
    std::multimap<unsigned int, realFloat> moments;
};

class MomentsEstimators {
  public:
    MomentsEstimators();
    void insert(const unsigned int& whichMoment, const DataSample& momentEstValues);
    DataSample operator[](const unsigned int& whichMoment);
    std::vector<DataSample> operator[](const std::initializer_list<unsigned int>& whichMoments);
    std::vector<DataSample> operator()(const unsigned int& whichMoment);

  private:
    std::multimap<unsigned int, DataSample> momentsEstimators;
};

#endif /* MOMENTS_HPP_ */
