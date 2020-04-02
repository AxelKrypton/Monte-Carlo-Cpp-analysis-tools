/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015,2020 Alessandro Sciarra
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

#ifndef TYPES_HPP_
#define TYPES_HPP_

#ifdef _USE_BIG_FLOAT_
    #include <boost/multiprecision/mpfr.hpp>  //this include is here, otherwise linking to mpfr is needed where mpfr is not actually used
typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<NUM_DECIMAL_DIGITS>, boost::multiprecision::et_on> realFloat;
#else
typedef double realFloat;
#endif

enum ErrorCalculationMethod { bootstrap = 1, jackknife };

#endif /* TYPES_HPP_ */
