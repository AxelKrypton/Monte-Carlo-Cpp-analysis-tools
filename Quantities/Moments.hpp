/*
 *
 *  Copyright (c) 2016,2018-2021 Alessandro Sciarra
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

template<typename T> class GenericMomentContainer {
  public:
    using reference = T&;
    using const_reference = const T&;
    reference operator[](unsigned int whichMoment)
    {
        try {
            return storedData.at(whichMoment);
        } catch (std::out_of_range& e) {
            throw std::out_of_range("Attempt to access unset moment in container!");
        }
    }
    void insert(unsigned int whichMoment, const_reference value)
    {
        auto emplaceResult = storedData.try_emplace(whichMoment, value);
        if (emplaceResult.second == false)
            throw std::invalid_argument("Attempt to overwrite already inserted moment in container!");
    }

  private:
    std::map<unsigned int, T> storedData;
};

using Moments = GenericMomentContainer<realFloat>;
using MomentsEstimators = GenericMomentContainer<DataSample>;

#endif /* MOMENTS_HPP_ */
