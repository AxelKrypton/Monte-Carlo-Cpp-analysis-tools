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

#include "DataSampleAnalyzer.hpp"
#include "../IO/io_utilities.hpp"

DataSampleAnalyzer::DataSampleAnalyzer(DataSample &sample, Parameters parametersIn):
parameters(parametersIn)
{
	/**
	 * If the autocorrelation should be estimated, do not analyze any other observable!
	 */
	if (parameters.calcAutocorrelation)
	{
		AutocorrelationAnalyzer(sample, parameters);
	}
	else
	{
		if(!parameters.doNotAnalyzeMean)
		{
			MeanAnalyzer(sample, parameters);
		}
		if(!parameters.doNotAnalyzeVariance)
		{
			VarianceAnalyzer(sample, parameters);
		}
		if(!parameters.doNotAnalyzeSkewness)
		{
			SkewnessAnalyzer(sample, parameters);
		}
		if(!parameters.doNotAnalyzeKurtosis)
		{
			KurtosisAnalyzer(sample, parameters);
		}
	}
}
	
MeanAnalyzer::MeanAnalyzer(DataSample &sample, const Parameters parameters):
	AnalyzerWrapper("Mean", getFilenameForObservables(parameters))
{
	estimateAndError = calcMeanAndErrorOfDataSample(sample, parameters);
}

VarianceAnalyzer::VarianceAnalyzer(DataSample &sample, const Parameters parameters):
	AnalyzerWrapper("Variance", getFilenameForObservables(parameters))
{
	estimateAndError = calcVarianceAndErrorOfDataSample(sample, parameters);
}

SkewnessAnalyzer::SkewnessAnalyzer(DataSample &sample, const Parameters parameters):
	AnalyzerWrapper("Skewness", getFilenameForObservables(parameters))
{
	estimateAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
}
KurtosisAnalyzer::KurtosisAnalyzer(DataSample &sample, const Parameters parameters):
	AnalyzerWrapper("Kurtosis", getFilenameForObservables(parameters))
{
	estimateAndError = calcKurtosisAndErrorOfDataSample(sample, parameters);
}

AutocorrelationAnalyzer::AutocorrelationAnalyzer(DataSample &sample, Parameters parameters):
	AnalyzerWrapperBasic("Autocorrelation", getFilenameForObservables(parameters))
{
	calcAutocorrelationAndErrorOfDataSample(sample, parameters);
}

AnalyzerWrapperBasic::AnalyzerWrapperBasic(std::string name, std::string outputFilename):
	estimateName(name), outputFilename(outputFilename)
{
    std::cout << "#------------------------------------------------------------------------------------" << std::endl;
	std::cout << "# Analyse " << name << "..." << std::endl;
}

AnalyzerWrapper::AnalyzerWrapper(std::string name, std::string outputFilename):
	AnalyzerWrapperBasic(name, outputFilename) {}

AnalyzerWrapper::~AnalyzerWrapper()
{
	printEstimateAndError(estimateName, estimateAndError.estimate, estimateAndError.error);
	writeEstimateAndErrorToFile(estimateName, estimateAndError.estimate, estimateAndError.error, outputFilename);
}

