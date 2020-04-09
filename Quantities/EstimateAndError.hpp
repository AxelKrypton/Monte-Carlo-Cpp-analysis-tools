/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2019 David Leemueller
 *  Copyright (c) 2020 Alessandro Sciarra
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

#ifndef ESTIMATEANDERROR_HPP_
#define ESTIMATEANDERROR_HPP_
#include "../types.hpp"

#include <iostream>

class EstimateAndError {
  public:
    // Here default initialisation to 0.0 is crucial, because Histogram classes rely on it!
    EstimateAndError() : estimate(0.0), error(0.0) {}
    EstimateAndError(realFloat value, realFloat error) : estimate(value), error(error) {}

    realFloat estimate;
    realFloat error;
};

inline std::ostream& operator<<(std::ostream& stream, const std::pair<EstimateAndError, std::string>& value)
{
    return stream << value.first.estimate << value.second << value.first.error;
}

inline std::ostream& operator<<(std::ostream& stream, const EstimateAndError& value)
{
    return operator<<(stream, {value, "\t"});
}

#endif
