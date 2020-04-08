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

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/DataSample.hpp"
#include "Constants.hpp"

Quantities::Quantities() : mean(), variance(), skewness(), kurtosis() {}

/*
 * NOTE: It it important to get the full parameters here and to extract the binning
 *       information for each quantity, since the number to make binning depends
 *       on the needed moment to calculate the quantity!
 */
Quantities::Quantities(const DataSample& dataSample, Parameters parameters)
{
    if (! parameters.doNotAnalyzeMean) {
        mean = Mean(dataSample, parameters.getBinningParametersForObservablesAnalysis(constants::observableName<Mean>),
                    parameters.isMeanKnownToBeZero);
    }
    if (! parameters.doNotAnalyzeVariance) {
        variance = Variance(dataSample, parameters.getBinningParametersForObservablesAnalysis(constants::observableName<Variance>),
                            parameters.isMeanKnownToBeZero);
    }
    if (! parameters.doNotAnalyzeSkewness) {
        skewness = Skewness(dataSample, parameters.getBinningParametersForObservablesAnalysis(constants::observableName<Skewness>),
                            parameters.isMeanKnownToBeZero);
    }
    if (! parameters.doNotAnalyzeKurtosis) {
        kurtosis = Kurtosis(dataSample, parameters.getBinningParametersForObservablesAnalysis(constants::observableName<Kurtosis>),
                            parameters.isMeanKnownToBeZero);
    }
}

std::string Quantities::getMetaInformation()
{
    std::string metaInfos = "";
    std::vector<std::string> observableNames = {constants::observableName<Mean>, constants::observableName<Variance>,
                                                constants::observableName<Skewness>, constants::observableName<Kurtosis>};
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

const QuantityAbstract& Quantities::operator[](std::string quantityLabel) const
{
    if (quantityLabel == constants::observableName<Mean>)
        return mean;
    else if (quantityLabel == constants::observableName<Variance>)
        return variance;
    else if (quantityLabel == constants::observableName<Skewness>)
        return skewness;
    else if (quantityLabel == constants::observableName<Kurtosis>)
        return kurtosis;
    else
        throw std::out_of_range("Quantities::operator[] accessed an invalid quantity!");
}

QuantityAbstract& Quantities::operator[](std::string quantityLabel)
{
    // Item 3 "Use const whenever possible," in Effective C++, 3d ed by Scott Meyers, ISBN-13: 9780321334879.
    return const_cast<QuantityAbstract&>(static_cast<const Quantities&>(*this)[quantityLabel]);
}
