#ifndef DATASAMPLETESTUTITLITIES_HPP_
#define DATASAMPLETESTUTITLITIES_HPP_

/**
 * double should be correct up to 15 digits (at least)
 * this means two doubles should be the same relative
 * to each other up to 14 digits
 * */
double doublePrecisionInPercent = 1e-12;

/**
 * Arrays filled with constants have a mean equal to the constant and zero variance.
 * Arrays proportional to sums over the iteration variable can be evaluated using the triangular numbers,
 * the  square pyramidal numbers or Faulhaber's formula in general.
 */
std::valarray<double> makeValarrayWithOnes(int length)
{
	return std::valarray<double>(1., length);
}

std::valarray<double> makeValarrayWithArrayPosition(int length)
{
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = iteration;
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithSquaredArrayPosition(int length)
{
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = iteration*iteration;
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithEntriesBetweenZeroAndOne(int length)
{
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration <  (int) returnValarray.size(); iteration ++)
	{
		returnValarray[iteration] = (double(iteration)) / (returnValarray.size() - 1);
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithBigAndSmallEntries(int length)
{
	std::valarray<double> returnValarray(length*2);
	for (int iteration = 0; iteration < length; iteration ++)
	{
		returnValarray[iteration*2] = (double(iteration)) / (length - 1) * 5e6;
		returnValarray[iteration*2+1] = (double(iteration)) / (length - 1) * 5e-6;
	}
	return returnValarray;
}

std::valarray<double> makeValarrayWithEntriesBetweenOneAndEight(int length)
{
	if (length % 8 != 0)
		throw std::invalid_argument("length must be multiple of 8!");
	std::valarray<double> returnValarray(length);
	for (int iteration = 0; iteration < 8; iteration ++)
	{
		for (int iteration2 = 0; iteration2 < length/8; iteration2 ++)
		{
			returnValarray[iteration + 8*iteration2] = (double(iteration+1));
		}
	}
	return returnValarray;
}

enum FillType { zeros, ones, arrayPosition, entriesSymmetricBetweenZeroAndOne, entriesBetweenOneAndEight, bigAndSmallEntries };

#endif /* DATASAMPLETESTUTITLITIES_HPP_ */
