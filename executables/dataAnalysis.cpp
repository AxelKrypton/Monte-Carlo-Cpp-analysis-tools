
#include "parameters.hpp"
//todo: this include should not be necessary in the end
#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"

//todo: perhaps make a common sample object that is passed as pointer to save memory allocation stuff

//todo: move this into own class
void calcAutocorrelation(std::string file)
{
	throw std::invalid_argument("Autocorrelation is not implemented yet. Aborting!");
}

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

class MeanAnalyzer : public AnalyzerWrapper
{
public:
	MeanAnalyzer(std::string & file, int & numberOfBins):
		AnalyzerWrapper("Mean")
	{
	    DataSample sample(file);
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(sample, numberOfBins);
	}
};

class VarianceAnalyzer : public AnalyzerWrapper
{
public:
	VarianceAnalyzer(std::string & file, int & numberOfBins):
		AnalyzerWrapper("Variance")
	{
	    DataSampleAnalyzer sample(file);
	    DataSampleAnalyzer varianceSample = (sample - sample.getNthMoment(1)) ^ 2;

	    //todo: replace with dedicated function
	    estimateAndError = calcMeanAndErrorOfDataSampleWithBinningFromNumberOfBins(varianceSample, numberOfBins);
	}
};

void calcSkewness(std::string & file, int & numberOfBins)
{
    /**
     * Skewness gamma_1 is defined as:
     *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
     */
    DataSampleAnalyzer sample(file);
    DataSampleAnalyzer thirdCentralMoment = (sample - sample.getNthMoment(1)) ^ 3;
    DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
    JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
    //this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
    JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
    double skewness = skewnessSample.getNthMoment(1);
    double error = skewnessSample.getJackknifeError();
    std::cout << "\t\tSkewness\t\tError" << std::endl;
    std::cout << "JackEstimate:\t" << scientific << skewness << "\t" << error << std::endl;
}

void calcKurtosis(std::string file, int numberOfBins)
{
    /**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */
    DataSampleAnalyzer sample(file);
    DataSampleAnalyzer fourthCentralMoment = (sample - sample.getNthMoment(1)) ^ 4;
    DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    DataSampleAnalyzer binnedSample1 = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
    JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
    JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
    //this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
    JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
    double skewness = kurtosisSample.getNthMoment(1);
    double error = kurtosisSample.getJackknifeError();
    std::cout << "\t\tBinder\t\tError" << std::endl;
    std::cout << "JackEstimate:\t" << scientific << skewness << "\t" << error << std::endl;
}

class dataAnalyzer
{
public:
	dataAnalyzer(parameters paramsIn):
		params(paramsIn)
	{
		if (params.calcAutocorrelation)
		{
			calcAutocorrelation(params.file);
		}
	    if(params.analyseMean)
	    {
	        MeanAnalyzer(params.file, params.numberOfBins);
	    }
	    if(params.analyseVariance)
	    {
	        VarianceAnalyzer(params.file, params.numberOfBins);
	    }
	    if(params.analyseSkewness)
	    {
	        calcSkewness(params.file, params.numberOfBins);
	    }
	    if(params.analyseKurtosis)
	    {
	        calcKurtosis(params.file, params.numberOfBins);
	    }
	};

private:
	parameters params;
};

int main(int argc, char ** argv)
{
	try
	{
		parameters params(argc, argv);
	    dataAnalyzer analyzer(params);
	}
	//todo: move catch block into own function?
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(parameters::parse_aborted)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
	}

    return 0;
}
