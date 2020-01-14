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

/**
 * Definition of the dataSample class
 */

#ifndef DATASAMPLEBASIC_HPP_
#define DATASAMPLEBASIC_HPP_

#include "../types.hpp"
#include<valarray>
#include<vector>
#include<stdexcept>
#include<iostream>

const static int roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid = 1e4;

//TODO: move this to better place
realFloat defaultFunction(realFloat in);

class DataSampleBasic
{
public:
	DataSampleBasic(int length = defaultSizeOfDataSample);
	DataSampleBasic(std::valarray<realFloat> valuesIn);
	//todo: think about creating two or three distinct constructors
	DataSampleBasic(std::string dataFilename, int column = 1, int offset = 0, bool* isAnyEntryBad = NULL);
	//todo: make the args and fcts. constant where possible
	int getNumberOfElements();
	DataSampleBasic applyFunction(realFloat (*function)(realFloat) = defaultFunction);
	DataSampleBasic& operator+=(realFloat factor);
	DataSampleBasic& operator+=(DataSampleBasic sampleIn);
	DataSampleBasic& operator-=(realFloat factor);
	DataSampleBasic& operator-=(DataSampleBasic sampleIn);
	DataSampleBasic& operator*=(int factor);
	DataSampleBasic& operator*=(realFloat factor);
	DataSampleBasic& operator*=(DataSampleBasic sampleIn);
	DataSampleBasic& operator/=(realFloat factor);
	DataSampleBasic& operator/=(DataSampleBasic sampleIn);
	DataSampleBasic& operator^=(int n);
	DataSampleBasic& operator^=(realFloat n);
	DataSampleBasic sampleSlice(int start, int size, int stride);
	realFloat sum();
    realFloat min();
    realFloat max();
	realFloat& operator[](size_t index);
	DataSampleBasic removeIthElement(int i);
	DataSampleBasic removeLastNElements(int n);

protected:
	void setValues(std::valarray<realFloat> valuesIn);
	void setValues(DataSampleBasic sampleIn);
	void checkIfNumberOfElementsIsValid(int length);
	void checkSliceParameters(int start, int size, int stride);
	DataSampleBasic readDataFromFile(std::string filename, int column, int offset, bool* isAnyEntryBad = NULL);

	std::valarray<realFloat> values;
	int numberOfElements;
	const static int defaultSizeOfDataSample = 1;
};

DataSampleBasic operator+(DataSampleBasic sampleIn, realFloat factor);
DataSampleBasic operator+(realFloat factor, DataSampleBasic sampleIn);
DataSampleBasic operator+(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator-(DataSampleBasic sampleIn, realFloat factor);
DataSampleBasic operator-(realFloat factor, DataSampleBasic sampleIn);
DataSampleBasic operator-(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator*(DataSampleBasic sampleIn, int factor);
DataSampleBasic operator*(int factor, DataSampleBasic sampleIn);
DataSampleBasic operator*(DataSampleBasic sampleIn, realFloat factor);
DataSampleBasic operator*(realFloat factor, DataSampleBasic sampleIn);
DataSampleBasic operator*(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator/(DataSampleBasic sampleIn, realFloat factor);
DataSampleBasic operator/(realFloat factor, DataSampleBasic sampleIn);
DataSampleBasic operator/(DataSampleBasic lhs, DataSampleBasic rhs);
DataSampleBasic operator^(DataSampleBasic sampleIn, int n);
DataSampleBasic operator^(DataSampleBasic sampleIn, realFloat n);
bool operator==(DataSampleBasic lhs, DataSampleBasic rhs);
bool operator!=(DataSampleBasic lhs, DataSampleBasic rhs);

#endif /* DATASAMPLEBASIC_HPP_ */
