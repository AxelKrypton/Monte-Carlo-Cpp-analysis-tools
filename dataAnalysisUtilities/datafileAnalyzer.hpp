/*
 * datafileAnalyser.hpp
 * Functionality to analyze a given datafile.
 * No direct tests are implemented as all classes rely
 * on functionality of other classes.
 */

#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "../Parameters/Parameters.hpp"
#include "dataAnalysisUtilities.hpp"

//todo: perhaps make a common sample object that is passed as pointer or call by reference to save memory allocation stuff
//todo: naming is bad, one should have: dataAnalyzer -> DataSampleAnalyzer, DataSampleAnalyzer -> DataSample, DataSample -> DataSampleBasic or so!

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
	MeanAnalyzer(DataSampleAnalyzer &sample, const Parameters parameters):
		AnalyzerWrapper("Mean")
	{
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, parameters.numberOfBins);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(DataSampleAnalyzer &sample, const Parameters parameters):
		AnalyzerWrapper("Variance")
	{
	    DataSampleAnalyzer varianceSample = (sample - sample.getNthMoment(1)) ^ 2;

	    //todo: replace with dedicated function
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(varianceSample, parameters.numberOfBins);
	}
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(DataSampleAnalyzer &sample, const Parameters parameters):
		AnalyzerWrapper("Skewness")
	{
		//todo: replace with dedicated function
	    estimateAndError = calcSkewness(sample, parameters.numberOfBins);
	}
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(DataSampleAnalyzer &sample, const Parameters parameters):
		AnalyzerWrapper("Kurtosis")
	{
	    //todo: replace with dedicated function
	    estimateAndError = calcKurtosis(sample, parameters.numberOfBins);
	}
};

class AutocorrelationAnalyzer : public AnalyzerWrapper
{
public:
	AutocorrelationAnalyzer(DataSampleAnalyzer &sample):
		AnalyzerWrapper("Autocorrelation")
	{
		throw std::invalid_argument("Autocorrelation is not implemented yet. Aborting!");
	}
};

//todo: rename to fileAnalyzer and create the dataSample object in here!
class dataAnalyzer
{
public:
	dataAnalyzer(DataSampleAnalyzer &sample, Parameters parametersIn):
		parameters(parametersIn)
	{
		if (parameters.calcAutocorrelation)
		{
			//I dont know why I need a name for the object here and not below
			//If I do not have that, I get a compiler error that AutocorrelationAnalyzer() is called!
			AutocorrelationAnalyzer tmp(sample);
		}
	    if(parameters.analyseMean)
	    {
	        MeanAnalyzer(sample, parameters);
	    }
	    if(parameters.analyseVariance)
	    {
	        VarianceAnalyzer(sample, parameters);
	    }
	    if(parameters.analyseSkewness)
	    {
	        SkewnessAnalyzer(sample, parameters);
	    }
	    if(parameters.analyseKurtosis)
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
		DataSampleAnalyzer dataSample(filename);
		dataAnalyzer analyzer(dataSample, parameters);
	}
};

#endif /* DATAFILEANALYZER_HPP_ */
