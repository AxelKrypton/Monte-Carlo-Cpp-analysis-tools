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

class BinderAnalyzer : public AnalyzerWrapper
{
public:
	BinderAnalyzer(DataSample &sample, const Parameters parameters);
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