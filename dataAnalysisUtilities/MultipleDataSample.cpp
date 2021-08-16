/*
 *
 *  Copyright (c) 2020-2021 Alessandro Sciarra
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

#include "../Quantities/Tools.hpp"

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

std::vector<realFloat> MultipleDataSample::getEstimatesPerTrajectory(const unsigned int trajectory)
{
    std::vector<realFloat> result(data.size());
    for (auto i = 0U; i < result.size(); i++)
        result[i] = data[i][trajectory];
    return result;
}

DataSample MultipleDataSample::getNthMomentPerDataPoint(unsigned int n)
{
    if (data.size() == 1)
        return data[0].getNthMomentPerDataPoint(n);
    else {
        DataSample result(data[0].getNumberOfElements());
        for (auto i = 0; i < result.getNumberOfElements(); i++)
            result[i] = getUnbiasEstimateOfNthMomentPerTrajectory(getEstimatesPerTrajectory(i), n);
        return result;
    }
}

DataSample MultipleDataSample::getNthCentralMomentPerDataPoint(unsigned int n)
{
    if (data.size() == 1)
        return data[0].getNthCentralMomentPerDataPoint(n);
    else
        throw std::logic_error("Calculation of central moment for MultipleDataSample shall not be used!");
    /*
     * NOTE: If multiple estimates are used, it is understood that an unbiased estimate of every moment
     *       per trajectory is desired and this implies that no central moment should be evaluated. Moreover
     *       it would be ill defined which x per trajectory to use in (x-<mu>)^n.
     */
}

DataSample& MultipleDataSample::operator[](size_t n)
{
    return data[n];
}

size_t MultipleDataSample::size()
{
    return data.size();
}
