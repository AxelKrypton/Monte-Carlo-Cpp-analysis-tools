# include "dataAnalysisUtilities.hpp"

static double meanOfDataSample(DataSample & sampleIn)
{
	return sampleIn.getNthMoment(1);
}

/**
 * A biased estimate of the sample variance is the naive
 *   definition, the second central moment.
 * An unbiased estimate of variance of the sample is
 *   n/(n-1) * biasedEstimator(varianceOfSample)
 * and the estimate of the variance of the mean of the sample is always:
 *   varianceEstimator(sample) / n
 * because of the central limit theorem.
 * See B.A. Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis"
 * Note that for the mean the unbiased variance yields
 * the same error as jackknifing.
 */
static double unbiasedVarianceOfDataSample(DataSample & sampleIn)
{
	return double(sampleIn.getNumberOfElements()) / double(sampleIn.getNumberOfElements() - 1.) * sampleIn.getNthCentralMoment(2);
}

static double unbiasedVarianceOfMean(DataSample & sampleIn)
{
	return 1. / double(sampleIn.getNumberOfElements() - 1) * sampleIn.getNthCentralMoment(2);
}

static double unbiasedErrorOfVariance(DataSample & sampleIn)
{
	/**
	 * A Jackknife analysis of the (naive) sample variance
	 *   1/N Sum ( sample[i] - mean )^2
	 * yields that the pseudovalues are
	 *   N/(N-1) Sum ( sample[i] - mean )^2
	 * (Example 3 in "Jackknife.pdf").
	 * This means that the jackknife error estimate is
	 *   sqrt(1/(N-1) * (Variance of the pseudo-values) )
	 * Hence, one can generate a new sample with each entry x_j
	 * the "variance" of entry x_j ( (x_j - mean)^2 ) and treat
	 * it the same way as an error on a mean.
	 */
	DataSample varianceSample = (sampleIn - sampleIn.getNthMoment(1)) ^ 2;
	return sqrt( unbiasedVarianceOfMean(varianceSample) );
}

EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn)
{
	double mean;
	double error;

	mean = meanOfDataSample(sampleIn);
	error = sqrt( unbiasedVarianceOfMean(sampleIn) );

	return EstimateAndError(mean, error);
}

EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn)
{
	double variance;
	double error;

	variance = unbiasedVarianceOfDataSample(sampleIn);
	error = unbiasedErrorOfVariance(sampleIn);

	return EstimateAndError(variance, error);
}

//todo: this include should not be necessary in the end
#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

//todo: work over this
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
    /**
     * Skewness gamma_1 is defined as:
     *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
     */

	//todo: this is a workaround, the binned dataSample as argument has to be implemented.
	DataSample sample(parameters.file);

    DataSample thirdCentralMoment = (sample - sample.getNthMoment(1)) ^ 3;
    DataSample secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    if ( parameters.useNumberOfBinsForBinning)
      {
	BinnedDataSampleFromNumberOfBins binnedSample1(thirdCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromNumberOfBins binnedSample2(secondCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);

	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
	JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
	double skewness = skewnessSample.getNthMoment(1);
	double error = skewnessSample.getJackknifeError();
    
	return EstimateAndError(skewness, error);
      }
    else
      {
	BinnedDataSampleFromBinsize binnedSample1(thirdCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromBinsize binnedSample2(secondCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);

	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
	JackknifeEstimators skewnessSample(jackSample1 / (jackSample2 ^ (3. / 2)));
	double skewness = skewnessSample.getNthMoment(1);
	double error = skewnessSample.getJackknifeError();
	
	return EstimateAndError(skewness, error);
      }
}

//todo: work over this
//todo: repair: this is acutally binder, rename? print also kurtosis?
EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters)
{
    /**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */

	//todo: this is a workaround, the binned dataSample as argument has to be implemented.
	DataSample sample(parameters.file);

    DataSample fourthCentralMoment = (sample - sample.getNthMoment(1)) ^ 4;
    DataSample secondCentralMoment = (sample - sample.getNthMoment(1)) ^ 2;
    if ( parameters.useNumberOfBinsForBinning)
      {
	BinnedDataSampleFromNumberOfBins binnedSample1 (fourthCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromNumberOfBins binnedSample2 (secondCentralMoment, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
	JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
	double skewness = kurtosisSample.getNthMoment(1);
	double error = kurtosisSample.getJackknifeError();

	return EstimateAndError(skewness, error);
      }
    else
      {
	BinnedDataSampleFromBinsize binnedSample1(fourthCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	BinnedDataSampleFromBinsize binnedSample2(secondCentralMoment, parameters.binsize, parameters.binningMustFitDataSampleSize);
	JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
	JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);
	//this calculates x_i / y_i^2, where x_i and y_i are the jackknife estimators of the fourth and second moment
	JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
	double skewness = kurtosisSample.getNthMoment(1);
	double error = kurtosisSample.getJackknifeError();

	return EstimateAndError(skewness, error);
      }

}

/*
 * Here, following the Berg book (pages 201-202), a method to estimate the integrated_autocorrelation_time
 * with an error is implemented. It is based on the Jackknife method. The idea is to build a set of
 * jackknife estimators for the autocorrelation_function C(t) of eq. (4.3) at some FIXED time.
 *
 * Once this set X of estimators is ready one can
 *   - either use the jackknife function above with a trivial f in order to get a value with
 *     error for C(t)
 *      --->  jackknife(X, [] (double val) -> double {return val;})
 *   - or build other sets of estimators like X at different times and use them to build a set Y
 *     of estimators for the integrated_autocorrelation_time at time t (see eq. (4.14)). Again
 *     use the jackknife function above with a trivial f in order to get a value with error for tau_int(t)
 *      --->  jackknife(Y, [] (double val) -> double {return val;})
 *  In both cases, one can produce plots similar to those of Figure 4.1-4.2.
 *
 *  Plotting the resulting data with errors, from the FIRST plateau, one can make the final estimate
 *  of the integrated_autocorrelation_time (read around equation (4.13) to understand why to look at
 *  the first plateau is important).
 *
 *  NOTE: Probably the jackknife call could in principle done with a more complicated function
 *        so that the sets X and Y can be built directly inside the jackknife. This is not however
 *        wise because to apply the jackknife one must have NOT correlated data and here this is
 *        achieved with a binning on the estimators BEFORE calling the jackknife.
 *
 *  NOTE: In principle, if one is interested only to a rough idea of the integrated_autocorrelation_time
 *        he can implement the equation (4.14) and look for a plateau plotting the output data.
 *        Of course this is not so rigorous because no error is estimated.
 *
 *  NOTE: The number of bins in the following function is that used to make binning on the estimators before
 *        the jackknife and it is contained in parameters (numberOfBinsForAutocorrelation).
 *        On page 201 of Berg's book there is written that it has to be much smaller of the total amount of data.
 *        This is up to the user, but if it is not given, then it is set to 10 (in principle fine for a data sample
 *        with more than 1000 data).
 */
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample & sample, int time);
static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample & sample, Parameters parameters);

std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEsitmatesOfDataSample(DataSample & sample, Parameters parameters)
{

	std::vector<BinnedDataSampleFromNumberOfBins>
	autocorrelationFunctionValuesBinnedSets = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

	std::vector<EstimateAndError> result;
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		JackknifeEstimatorsFromBinnedDataSample jackSample(autocorrelationFunctionValuesBinnedSets[time]);
		result.push_back(EstimateAndError(autocorrelationFunctionValuesBinnedSets[time].getNthMoment(1), jackSample.getJackknifeError()));
	}

	return result;
}

std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEsitmatesOfDataSample(DataSample & sample, Parameters parameters)
{

	std::vector<BinnedDataSampleFromNumberOfBins>
	autocorrelationFunctionValuesBinnedSets = calcAutocorrelationFunctionValuesBinnedSets(sample, parameters);

	std::vector<DataSample> integratedTimeBinnedSets;
	//The first array of integratedTimeBinnedSets must be an array of ones
	integratedTimeBinnedSets.push_back(DataSample(std::valarray<double>(1.0, autocorrelationFunctionValuesBinnedSets[0].getNumberOfElements())));

	for(int time=1; time<parameters.timeMaxAutocorrelationFunction; time++){
		integratedTimeBinnedSets.push_back((integratedTimeBinnedSets[time-1]
		                                    + 2.0 * autocorrelationFunctionValuesBinnedSets[time] / autocorrelationFunctionValuesBinnedSets[0]));
	}

	std::vector<EstimateAndError> result;
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		JackknifeEstimatorsFromBinnedDataSample jackSample(integratedTimeBinnedSets[time]);
		result.push_back(EstimateAndError(integratedTimeBinnedSets[time].getNthMoment(1), jackSample.getJackknifeError()));
	}

	return result;
}

//todo: move this to IO
static std::string getFilenameForAutocorrelation(Parameters parameters)
{
	return parameters.file + "_auto";
}

EstimateAndError calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters)
{
	std::vector<EstimateAndError> result = calcArrayOfAutocorrelationTimesAndErrorEsitmatesOfDataSample(sample, parameters);

	std::vector<double> estimates;
	std::vector<double> errors;
	for (int i = 0; i< int(result.size()); i++ )
	{
		estimates.push_back(result[i].estimate);
		errors.push_back(result[i].error);
	}

	std::string filename = getFilenameForAutocorrelation(parameters);

	writeEstimateAndErrorArraysToFile("auto", estimates, errors, filename);
}

DataSample createDataSampleFromDatafile(std::string filename, Parameters parameters)
{
	DataSample dataSample(filename);
	if ( parameters.useBinning)
	{
		if (parameters.calcAutocorrelation)
		{
			std::cout << "Do not perform binning as the autocorrelation should be estimated!" << std::endl;
		}
		else
		{
			std::cout << "Perform binning on data sample..." << std::endl;
			if ( parameters.useNumberOfBinsForBinning)
			{
				return BinnedDataSampleFromNumberOfBins(dataSample, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
			}
			else
			{
				return BinnedDataSampleFromBinsize(dataSample, parameters.binsize, parameters.binningMustFitDataSampleSize);
			}
		}
	}
	return dataSample;
}



/*****************************************************************************************/
static DataSampleBasic autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(DataSample & sample, int time)
{
	DataSampleBasic x_first  = sample.sampleSlice(0, sample.getNumberOfElements() - time, 1);
	DataSampleBasic x_second = sample.sampleSlice(time, sample.getNumberOfElements() - time, 1);
	double data_mean = sample.getNthMoment(1);
	/*
	 * Since we do not know in general if the "true" mean value of our sample is zero, we will
	 * always substitute it by its estimator (the mean of the sample itself), but then we introduce
	 * a bias that we correct here. Read paragraph between (4.19) and (4.20) on Berg's book for
	 * additional informations.
	 */
	DataSampleBasic result = ((x_first - data_mean) * (x_second - data_mean));
	result = result * sample.getNumberOfElements() / (sample.getNumberOfElements() - 1);
	return result;
}

static std::vector<BinnedDataSampleFromNumberOfBins> calcAutocorrelationFunctionValuesBinnedSets(DataSample & sample, Parameters parameters)
{
	std::vector<BinnedDataSampleFromNumberOfBins> autocorrelationFunctionValuesBinnedSets;
	for(int time=0; time<parameters.timeMaxAutocorrelationFunction; time++){
		autocorrelationFunctionValuesBinnedSets.push_back(BinnedDataSampleFromNumberOfBins(
				                                          autocorrelationFunctionValuesAtCertainTimeNotAveragedOut(sample, time),
				                                          parameters.numberOfBinsForAutocorrelation, false, false));
	}
	return autocorrelationFunctionValuesBinnedSets;
}



