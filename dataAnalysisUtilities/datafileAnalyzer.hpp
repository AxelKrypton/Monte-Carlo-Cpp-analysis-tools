/*
 * datafileAnalyser.hpp
 */

#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "../Parameters/Parameters.hpp"
#include "dataAnalysisUtilities.hpp"
#include "../IO/io_utilities.hpp"

class AnalyzerWrapperBasic
{
protected:
	AnalyzerWrapperBasic(std::string name, std::string outputFilename):
		estimateName(name), outputFilename(outputFilename)
	{
		std::cout << "# Analyse " << name << "..." << std::endl;
	}
	~AnalyzerWrapperBasic(){}

	std::string estimateName;
	std::string outputFilename;
	EstimateAndError estimateAndError;
};

class AnalyzerWrapper : public AnalyzerWrapperBasic
{
protected:
	AnalyzerWrapper(std::string name, std::string outputFilename):
		AnalyzerWrapperBasic(name, outputFilename) {}
	~AnalyzerWrapper()
	{
		printEstimateAndError(estimateName, estimateAndError.estimate, estimateAndError.error);
		writeEstimateAndErrorToFile(estimateName, estimateAndError.estimate, estimateAndError.error, outputFilename);
	}
};

class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(RawAndBinnedDataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Mean", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcMeanAndErrorOfDataSample(sample);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(RawAndBinnedDataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Variance", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcVarianceAndErrorOfDataSample(sample);
	}
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(RawAndBinnedDataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Skewness", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
	}
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(RawAndBinnedDataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Kurtosis", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcKurtosisAndErrorOfDataSample(sample, parameters);
	}
};

class AutocorrelationAnalyzer : public AnalyzerWrapperBasic
{
public:
	AutocorrelationAnalyzer(DataSample &sample, Parameters parameters):
		AnalyzerWrapperBasic("Autocorrelation", getFilenameForObservables(parameters))
	{
		estimateAndError = calcAutocorrelationAndErrorOfDataSample(sample, parameters);
	}
};

class DataSampleAnalyzer
{
public:
	DataSampleAnalyzer(RawAndBinnedDataSample &sample, Parameters parametersIn):
		parameters(parametersIn)
	{
		/**
		 * If the autocorrelation should be estimated, do not analyze any other observable!
		 */
		if (parameters.calcAutocorrelation)
		{
			AutocorrelationAnalyzer(sample.getBinnedData(), parameters);
		}
		else
		{
			if(parameters.analyzeMean)
			{
				MeanAnalyzer(sample, parameters);
			}
			if(parameters.analyzeVariance)
			{
				VarianceAnalyzer(sample, parameters);
			}
			if(parameters.analyzeSkewness)
			{
				SkewnessAnalyzer(sample, parameters);
			}
			if(parameters.analyzeKurtosis)
			{
				KurtosisAnalyzer(sample, parameters);
			}
		}
	};

private:
	Parameters parameters;
};

class DatafileAnalyzer
{
public:
	DatafileAnalyzer(std::string filename, Parameters parameters)
	{
		RawAndBinnedDataSample data(filename, parameters);
		DataSampleAnalyzer analyzer(data, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
