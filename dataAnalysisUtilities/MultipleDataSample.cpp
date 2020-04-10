/*
 *
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

#include "MultipleDataSample.hpp"

MultipleDataSample::MultipleDataSample(std::vector<DataSample> dataIn) : data(dataIn)
{
    int size = data[0].getNumberOfElements();
    for (auto sample : data) {
        if (sample.getNumberOfElements() != size)
            throw std::invalid_argument("MultipleDataSample constructed with not-equal-size samples!");
    }
}

MultipleDataSample::MultipleDataSample(std::string dataFilename, int column, int numberOfColumns, int offset)
{
    data.reserve(numberOfColumns);
    for (int i = 0; i < numberOfColumns; i++) {
        data.push_back(DataSample{dataFilename, column + i, offset});
    }
}

DataSample MultipleDataSample::getNthMomentPerDataPoint(unsigned int n)
{
    throw std::runtime_error("Calculation of moment number " + std::to_string(n) + " for MultipleDataSample not implemented yet!");
}
