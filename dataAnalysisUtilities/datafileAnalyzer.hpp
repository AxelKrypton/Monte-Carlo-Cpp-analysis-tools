/*
 * datafileAnalyser.hpp
 * No direct tests are implemented as all classes rely
 * on functionality of other classes.
 */

#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "../Parameters/Parameters.hpp"
#include "dataAnalysisUtilities.hpp"

class AnalyzerWrapper
{
protected:
	AnalyzerWrapper(std::string name):
		estimateName(name)
	{
		std::cout << "# Analyse " << name << "..." << std::endl;
	}
	~AnalyzerWrapper()
	{
	    std::cout << "# " << estimateName << "\t\tError" << std::endl;
	    std::cout << std::scientific << estimateAndError.estimate << "\t" << estimateAndError.error << std::endl;
	}

	std::string estimateName;
	EstimateAndError estimateAndError;
};

//todo: in all classes here: implement usage of binsize or no binning
class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Mean")
	{
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, parameters.numberOfBins);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Variance")
	{
	    DataSample varianceSample = (sample - sample.getNthMoment(1)) ^ 2;

	    //todo: replace with dedicated function
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(varianceSample, parameters.numberOfBins);
	}
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Skewness")
	{
		//todo: replace with dedicated function
	    estimateAndError = calcSkewness(sample, parameters.numberOfBins);
	}
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(DataSample &sample, const Parameters parameters):
		AnalyzerWrapper("Kurtosis")
	{
	    //todo: replace with dedicated function
	    estimateAndError = calcKurtosis(sample, parameters.numberOfBins);
	}
};

class AutocorrelationAnalyzer : public AnalyzerWrapper
{
public:
	AutocorrelationAnalyzer(DataSample &sample, Parameters parameters):
		AnalyzerWrapper("Autocorrelation")
	{
		estimateAndError = calcAutocorrelation(sample, parameters);
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
		//todo: move binning here
		DataSample dataSample(filename);
		DataSampleAnalyzer analyzer(dataSample, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
