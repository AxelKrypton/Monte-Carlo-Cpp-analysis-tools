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

#ifndef DATASAMPLEANALYZER_HPP_
#define DATASAMPLEANALYZER_HPP_

#include "../Parameters/Parameters.hpp"
#include "DataSample.hpp"
#include "dataAnalysisUtilities.hpp"

class AnalyzerWrapperBasic
{
protected:
	AnalyzerWrapperBasic(std::string name, std::string outputFilename);

	std::string estimateName;
	std::string outputFilename;
	EstimateAndError estimateAndError;
};

class AnalyzerWrapper : public AnalyzerWrapperBasic
{
protected:
	AnalyzerWrapper(std::string name, std::string outputFilename);
	~AnalyzerWrapper();
};

class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(DataSample &sample, const Parameters parameters);
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(DataSample &sample, const Parameters parameters);
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(DataSample &sample, const Parameters parameters);
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(DataSample &sample, const Parameters parameters);
};

class AutocorrelationAnalyzer : public AnalyzerWrapperBasic
{
public:
	AutocorrelationAnalyzer(DataSample &sample, Parameters parameters);
};

class DataSampleAnalyzer
{
public:
	DataSampleAnalyzer(DataSample &sample, Parameters parametersIn);

private:
	Parameters parameters;
};

#endif