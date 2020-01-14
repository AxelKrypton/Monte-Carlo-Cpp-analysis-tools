/*
 *
 *  Copyright (c)
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

#include "bootstrapAnalysis.hpp"

/*
 * The problem of the following function, if naively implemented, is that it could return nan if
 * the input data sample contains the same entry several times. To avoid this case we take the
 * absolute value before the square root.
 *
 * NOTE: To use the correct abs(x) both when x is double and when it is a multiprecision number,
 *       one has to explicitly declare which abs to use with an using direction. The general rule
 *       is that the compiler looks for an abs function in the namespace x belongs to, then it
 *       looks outside, first considering the using directions. If x is double variable, we want
 *       to use the std::abs function (not the abs outside of std that takes an int as argument!!).
 *       If x is a boost::multiprecision::number then we desire exactly the one in the actual
 *       boost namespace.
 *
 */
realFloat calculateBootstrapError(DataSample& sampleWithBootstrapEstimators)
{
	using std::abs;
	return sqrt(abs(sampleWithBootstrapEstimators.getNthMoment(2) - pow(sampleWithBootstrapEstimators.getNthMoment(1), 2.)));
}
