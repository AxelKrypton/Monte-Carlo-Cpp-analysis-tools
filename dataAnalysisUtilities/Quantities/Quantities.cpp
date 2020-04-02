/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018-2020 Alessandro Sciarra
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

#include "Quantities.hpp"

Quantities::Quantities() : mean(), variance(), skewness(), kurtosis() {}

std::string Quantities::getMetaInformation()
{
    std::string metaInfos = "";
    std::vector<std::string> observableNames
        = {Mean::observableName, Variance::observableName, Skewness::observableName, Kurtosis::observableName};
    for (unsigned int index = 0; index < observableNames.size(); index++) {
        metaInfos += observableNames[index] + "\t\t\terror\t\t\t";
    }
    return metaInfos;
}

std::string Quantities::getObservablesAsString()
{
    std::stringstream values;
    values.precision(12);
    values << std::scientific;
    values << mean.value.estimate << "\t" << mean.value.error << "\t";
    values << variance.value.estimate << "\t" << variance.value.error << "\t";
    values << skewness.value.estimate << "\t" << skewness.value.error << "\t";
    values << kurtosis.value.estimate << "\t" << kurtosis.value.error;
    return values.str();
}
