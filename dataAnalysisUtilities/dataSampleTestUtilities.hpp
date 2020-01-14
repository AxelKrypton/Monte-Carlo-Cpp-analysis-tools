/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2015,2019-2020 Alessandro Sciarra
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

#ifndef DATASAMPLETESTUTITLITIES_HPP_
#define DATASAMPLETESTUTITLITIES_HPP_

#include <valarray>
#include "../types.hpp"

/**
 * realFloat should be correct up to 15 digits (at least)
 * this means two realFloats should be the same relative
 * to each other up to 14 digits
 * */
realFloat realFloatPrecisionInPercent = 1e-12;

/**
 * Arrays filled with constants have a mean equal to the constant and zero variance.
 * Arrays proportional to sums over the iteration variable can be evaluated using the triangular numbers,
 * the  square pyramidal numbers or Faulhaber's formula in general.
 */
std::valarray<realFloat> makeValarrayWithOnes(int length)
{
	return std::valarray<realFloat>(1., length);
}

std::valarray<realFloat> makeValarrayWithOnesAndMinusOnes(int length)
{
	std::valarray<realFloat> returnValarray(1., length);
	for (int iteration = 0; iteration < (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = pow(-1.,iteration);
	}
	return returnValarray;
}

std::valarray<realFloat> makeValarrayWithArrayPosition(int length)
{
	std::valarray<realFloat> returnValarray(length);
	for (int iteration = 0; iteration < (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = iteration;
	}
	return returnValarray;
}

std::valarray<realFloat> makeValarrayWithSquaredArrayPosition(int length)
{
	std::valarray<realFloat> returnValarray(length);
	for (int iteration = 0; iteration < (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = iteration*iteration;
	}
	return returnValarray;
}

std::valarray<realFloat> makeValarrayWithEntriesBetweenZeroAndOne(int length)
{
	std::valarray<realFloat> returnValarray(length);
	for (int iteration = 0; iteration <  (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = (realFloat(iteration)) / (returnValarray.size() - 1);
	}
	return returnValarray;
}

std::valarray<realFloat> makeValarrayWithBigAndSmallEntries(int length)
{
	std::valarray<realFloat> returnValarray(length*2);
	for (int iteration = 0; iteration < length; iteration ++)
	{
		returnValarray[iteration*2] = (realFloat(iteration)) / (length - 1) * 5e6;
		returnValarray[iteration*2+1] = (realFloat(iteration)) / (length - 1) * 5e-6;
	}
	return returnValarray;
}

std::valarray<realFloat> makeValarrayWithEntriesBetweenOneAndEight(int length)
{
	if (length % 8 != 0)
		throw std::invalid_argument("length must be multiple of 8!");
	std::valarray<realFloat> returnValarray(length);
	for (int iteration = 0; iteration < 8; iteration ++)
	{
		for (int iteration2 = 0; iteration2 < length/8; iteration2 ++)
		{
			returnValarray[iteration + 8*iteration2] = (realFloat(iteration+1));
		}
	}
	return returnValarray;
}

enum FillType { zeros, ones, onesMinusOnes, arrayPosition, entriesSymmetricBetweenZeroAndOne, entriesBetweenOneAndEight, bigAndSmallEntries };

#endif /* DATASAMPLETESTUTITLITIES_HPP_ */
