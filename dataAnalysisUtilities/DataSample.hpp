/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2014-2015,2020 Alessandro Sciarra
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

#ifndef DATASAMPLE_HPP_
#define DATASAMPLE_HPP_

#include "DataSampleBasic.hpp"

//TODO: Think whether Moment and MomentPerDataPoint should be inside the class DataSample
/*
 * Here Moment is a generic name for central and non central moment: <x> and <x-mu>, <x^2> and <(x-mu)^2>, etc.
 */
class Moment
{
public:
	Moment()
	{
		calculated = false;
		value = 0.;
	}

	void set(realFloat valueIn)
	{
		value = valueIn;
		calculated = true;
	};

	bool calculated;
	realFloat value;
};

/*
 * MomentPerDataPoint is not an official name. It is referred to the content of the <...> in the Moment above.
 * For example: for the second MomentPerDataPoint the original data set will be considered and each data will
 * be shifted by the mean (if central) and squared, BUT the average will be not evaluated.
 */
class MomentPerDataPoint
{
public:
	MomentPerDataPoint()
	{
		calculated = false;
		value = DataSampleBasic();
	}

	void set(DataSampleBasic valueIn)
	{
		value = valueIn;
		calculated = true;
	};

	bool calculated;
	DataSampleBasic value;
};

class DataSample: public DataSampleBasic
{
public:
	DataSample(DataSampleBasic sampleIn):
		DataSampleBasic(sampleIn)
	{
		initMoments();
	}

	DataSample(int length = defaultSizeOfDataSample):
		DataSampleBasic(length)
	{
		initMoments();
	}

	DataSample(std::valarray<realFloat> valuesIn):
		DataSampleBasic(valuesIn)
	{
		initMoments();
	}

	DataSample(std::string dataFilename, int column = 1, int offset = 0):
		DataSampleBasic(dataFilename, column, offset)
	{
		initMoments();
	}

	realFloat getNthCentralMoment(int n);
	realFloat getNthMoment(int n);
	DataSampleBasic getNthCentralMomentPerDataPoint(int n);
	DataSampleBasic getNthMomentPerDataPoint(int n);
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();

protected:
	int getNumberOfMoments();
	void initMoments();
	realFloat calcNthMoment(int n);
	realFloat calcNthCentralMoment(int n);
	DataSampleBasic calcNthMomentPerDataPoint(int n);
	DataSampleBasic calcNthCentralMomentPerDataPoint(int n);

	std::vector<Moment> moments;
	std::vector<Moment> centralMoments;
	std::vector<MomentPerDataPoint> momentsPerDataPoint;
	std::vector<MomentPerDataPoint> centralMomentsPerDataPoint;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
};

DataSample removeNElementsFromDataSample(DataSample sampleIn, int n);

#endif /* DATASAMPLEANALYZER_HPP_ */
