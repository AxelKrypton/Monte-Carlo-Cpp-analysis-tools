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
			BinderAnalyzer(sample, parameters);
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
BinderAnalyzer::BinderAnalyzer(DataSample &sample, const Parameters parameters):
	AnalyzerWrapper("Binder", getFilenameForObservables(parameters))
{
	estimateAndError = calcBinderAndErrorOfDataSample(sample, parameters);
}

AutocorrelationAnalyzer::AutocorrelationAnalyzer(DataSample &sample, Parameters parameters):
	AnalyzerWrapperBasic("Autocorrelation", getFilenameForObservables(parameters))
{
	estimateAndError = calcAutocorrelationAndErrorOfDataSample(sample, parameters);
}

AnalyzerWrapperBasic::AnalyzerWrapperBasic(std::string name, std::string outputFilename):
	estimateName(name), outputFilename(outputFilename)
{
	std::cout << "# Analyse " << name << "..." << std::endl;
}

AnalyzerWrapper::AnalyzerWrapper(std::string name, std::string outputFilename):
	AnalyzerWrapperBasic(name, outputFilename) {}

AnalyzerWrapper::~AnalyzerWrapper()
{
	printEstimateAndError(estimateName, estimateAndError.estimate, estimateAndError.error);
	writeEstimateAndErrorToFile(estimateName, estimateAndError.estimate, estimateAndError.error, outputFilename);
}

