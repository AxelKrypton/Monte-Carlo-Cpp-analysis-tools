/*
 * datafileAnalyser.hpp
 * No direct tests are implemented as all classes rely
 * on functionality of other classes.
 */

#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "../Parameters/Parameters.hpp"
#include "dataAnalysisUtilities.hpp"
#include "../IO/io_utilities.hpp"

class AnalyzerWrapper
{
protected:
	AnalyzerWrapper(std::string name, std::string outputFilename):
		estimateName(name), outputFilename(outputFilename)
	{
		std::cout << "# Analyse " << name << "..." << std::endl;
	}
	~AnalyzerWrapper()
	{
		printEstimateAndError(estimateName, estimateAndError.estimate, estimateAndError.error);
		writeEstimateAndErrorToFile(estimateName, estimateAndError.estimate, estimateAndError.error, outputFilename);
	}

	std::string estimateName;
	std::string outputFilename;
	EstimateAndError estimateAndError;
};

class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Mean", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcMeanAndErrorOfDataSample(sample);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Variance", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcVarianceAndErrorOfDataSample(sample);
	}
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Skewness", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcSkewnessAndErrorOfDataSample(sample, parameters);
	}
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Kurtosis", getFilenameForObservables(parameters))
	{
	    estimateAndError = calcKurtosisAndErrorOfDataSample(sample, parameters);
	}
};

class AutocorrelationAnalyzer : public AnalyzerWrapper
{
public:
	AutocorrelationAnalyzer(DataSample &sample, Parameters parameters):
		AnalyzerWrapper("Autocorrelation", getFilenameForObservables(parameters))
	{
		estimateAndError = calcAutocorrelationAndErrorOfDataSample(sample, parameters);
	}
};

class DataSampleAnalyzer
{
public:
	DataSampleAnalyzer(DataSample &sample, Parameters parametersIn):
		parameters(parametersIn)
	{
		if (parameters.calcAutocorrelation)
		{
			AutocorrelationAnalyzer(sample, parameters);
		}
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
	};

private:
	Parameters parameters;
};

class DatafileAnalyzer
{
public:
	DatafileAnalyzer(std::string filename, Parameters parameters)
	{
		DataSample dataSample = createDataSampleFromDatafile(filename, parameters);
		DataSampleAnalyzer analyzer(dataSample, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
