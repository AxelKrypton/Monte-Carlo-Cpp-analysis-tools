/*
 * dataAnalyzer.hpp
 *
 *  Created on: Apr 26, 2014
 *      Author: christopher
 */

#ifndef DATAANALYZER_HPP_
#define DATAANALYZER_HPP_

//todo: move to own directory
#include "../executables/parameters.hpp"
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
	    std::cout << scientific << estimateAndError.estimate << "\t" << estimateAndError.error << std::endl;
	}

	std::string estimateName;
	EstimateAndError estimateAndError;
};

//todo: in all classes here: implement usage of binsize or no binning
class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(DataSampleAnalyzer &sample, const parameters params):
		AnalyzerWrapper("Mean")
	{
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, params.numberOfBins);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(DataSampleAnalyzer &sample, const parameters params):
		AnalyzerWrapper("Variance")
	{
	    DataSampleAnalyzer varianceSample = (sample - sample.getNthMoment(1)) ^ 2;

	    //todo: replace with dedicated function
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(varianceSample, params.numberOfBins);
	}
};

class SkewnessAnalyzer : public AnalyzerWrapper
{
public:
	SkewnessAnalyzer(DataSampleAnalyzer &sample, const parameters params):
		AnalyzerWrapper("Skewness")
	{
		//todo: replace with dedicated function
	    estimateAndError = calcSkewness(sample, params.numberOfBins);
	}
};

class KurtosisAnalyzer : public AnalyzerWrapper
{
public:
	KurtosisAnalyzer(DataSampleAnalyzer &sample, const parameters params):
		AnalyzerWrapper("Kurtosis")
	{
	    //todo: replace with dedicated function
	    estimateAndError = calcKurtosis(sample, params.numberOfBins);
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

class dataAnalyzer
{
public:
	dataAnalyzer(DataSampleAnalyzer &sample, parameters paramsIn):
		params(paramsIn)
	{
		if (params.calcAutocorrelation)
		{
			//I dont know why I need a name for the object here and not below
			//If I do not have that, I get a compiler error that AutocorrelationAnalyzer() is called!
			AutocorrelationAnalyzer tmp(sample);
		}
	    if(params.analyseMean)
	    {
	        MeanAnalyzer(sample, paramsIn);
	    }
	    if(params.analyseVariance)
	    {
	        VarianceAnalyzer(sample, paramsIn);
	    }
	    if(params.analyseSkewness)
	    {
	        SkewnessAnalyzer(sample, paramsIn);
	    }
	    if(params.analyseKurtosis)
	    {
	        KurtosisAnalyzer(sample, paramsIn);
	    }
	};

private:
	parameters params;
};



#endif /* DATAANALYZER_HPP_ */
