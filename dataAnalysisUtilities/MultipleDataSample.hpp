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

#include "DataSample.hpp"

#pragma once

class MultipleDataSample {
  public:
    MultipleDataSample() = delete;
    MultipleDataSample(std::string dataFilename, int column = 1, int numberOfColumns = 1, int offset = 0);
    MultipleDataSample(std::vector<DataSample> dataIn);

    DataSample getNthMomentPerDataPoint(unsigned int n);

  private:
    std::vector<DataSample> data;
};
