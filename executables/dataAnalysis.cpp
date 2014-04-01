#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

int main(int argc, char ** argv)
{
	//todo: add try-catch block

	int binsize;
	int numberOfBins;
	bool useBinning;
	bool calcAutocorrelation;
	int offset;
	std::string file;

	po::options_description desc("Generic options");
	po::variables_map vm;

	//todo: find out why short names gives parsing error!
	desc.add_options()
		("help,h", "Produce this help message")
		("datafile,f", po::value<std::string>(&file)->default_value(""), "File containing data")
		("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
		("useBinning", po::value<bool>(&useBinning)->default_value(true), "Use binning on data")
		("numberOfBins,nb", po::value<int>(&numberOfBins)->default_value(10), "Number of bins")
		("binsize,bs", po::value<int>(&binsize)->default_value(100), "Size of bin")
		("calcAutocorrelation,ac", po::value<bool>(&calcAutocorrelation)->default_value(false), "Estimate autocorrelation of data data")
	;

	po::store(po::parse_command_line(argc, argv, desc), vm);

	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		return 0;
	}

	po::notify(vm);

	using namespace std;
	cout << "###############################" << endl;
	cout << "Options:" << endl;
	cout << "###############################" << endl;
	cout << "Datafile:\t" << file << endl;
	cout << "Offset:\t" << offset << endl;
	if (useBinning)
	{
		cout << "binsize:\t" << binsize << endl;
		cout << "number of bins:\t" << numberOfBins << endl;
	}
	else
		cout << "Do not use binning!" << endl;
	if (calcAutocorrelation)
		cout << "Calculate estimate on autocorrelation" << endl;
	cout << "###############################" << endl;

	//todo: make "" a (constant) string object
	if (file == "")
		return 0;

	if (calcAutocorrelation)
	{
		cout << "Autocorrelation is not implemented yet. Aborting!" << endl;
		return 0;
	}

	bool analyseMean = false;
	if(analyseMean)
	{
		cout << "Analyse mean..." << endl;

		DataSampleAnalyzer sample(file);
		DataSampleAnalyzer binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double mean = binnedSample.getNthMoment(1);

		//todo: make this a fct. of DataSample!
		/**
		 * unbiased estimate of variance of the sample is
		 *   n/(n-1) * biasedEstimator(varianceOfSample)
		 * and the estimate of the variance of the mean of the sample is always:
		 *   varianceEstimator(sample) / n
		 * because of the central limit theorem.
		 */
		int n = binnedSample.getNumberOfElements();
		double error =  sqrt( 1. / double(n-1) * binnedSample.getNthCentralMoment(2) );

		cout << "Mean\t\tError" << endl;
		cout << scientific << mean << "\t" << error << endl;
	}

	bool analyseVariance = false;
	if(analyseVariance)
	{
		cout << "Analyse variance..." << endl;

		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer varianceSample = ( sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample = varianceSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double variance = binnedSample.getNthMoment(1);
		//todo: make this a fct. of DataSample!
		/**
		 * unbiased estimate of variance of the sample is
		 *   n/(n-1) * biasedEstimator(varianceOfSample)
		 * and the estimate of the variance of the mean of the sample is always:
		 *   varianceEstimator(sample) / n
		 * because of the central limit theorem.
		 */
		//todo: this equals numberOfBins!
		int n = binnedSample.getNumberOfElements();
		double error = sqrt( 1. / double(n-1) * binnedSample.getNthCentralMoment(2) );
		cout << "Variance\t\tError" << endl;
		cout << scientific << variance << "\t" << error << endl;
	}

	bool analyseSkewness = false;

	//naive version:
	if(analyseSkewness)
	{
		/**
		 * Skewness gamma_1 is defined as:
		 *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer thirdCentralMoment  = (sample - sample.getNthMoment(1))^3;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double x3 = thirdCentralMoment.getNthMoment(1);
		double x2 = secondCentralMoment.getNthMoment(1);

		double skewness = x3/pow(x2, 3./2.);

		//naive error propagation
		double varianceX3 = binnedSample1.getNthCentralMoment(2);
		double varianceX2 = binnedSample2.getNthCentralMoment(2);
		double derivativeX3 = fabs( 1./pow(x2, 3./2.) );
		double derivativeX2 = fabs( -3.*x3/2.*pow(x2, -5./2.) );
		double biasedEstimateOfSampleVarianceSquared =  varianceX2 * derivativeX2 + varianceX3 * derivativeX3 ;

		double errorOfMean = sqrt( 1. / double(numberOfBins-1) * biasedEstimateOfSampleVarianceSquared );

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "NaiveEstimate:\t" << scientific << skewness << "\t" << errorOfMean <<  endl;
	}

	//bit better version:
	if(analyseSkewness)
	{
		/**
		 * Skewness gamma_1 is defined as:
		 *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer thirdCentralMoment  = (sample - sample.getNthMoment(1))^3;

		double x2 = sample.getNthCentralMoment(2);
		double sigmaThree = pow(x2, 3./2.);
		thirdCentralMoment /= sigmaThree;

		DataSampleAnalyzer binnedSample = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double skewness = binnedSample.getNthMoment(1);

		//todo: make this a fct. of DataSample!
		/**
		 * unbiased estimate of variance of the sample is
		 *   n/(n-1) * biasedEstimator(varianceOfSample)
		 * and the estimate of the variance of the mean of the sample is always:
		 *   varianceEstimator(sample) / n
		 * because of the central limit theorem.
		 */
		//todo: this equals numberOfBins!
		int n = binnedSample.getNumberOfElements();
		double error = sqrt( 1. / double(n-1) * binnedSample.getNthCentralMoment(2) );

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "Estimate+:\t" << scientific << skewness << "\t" << error <<  endl;
	}

	//better version:
	if(analyseSkewness)
	{
		/**
		 * Skewness gamma_1 is defined as:
		 *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer thirdCentralMoment  = (sample - sample.getNthMoment(1))^3;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);

		//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
		JackknifeEstimators skewnessSample( jackSample1 / ( jackSample2^(3./2) ) );

		double skewness = skewnessSample.getNthMoment(1);
		double error = skewnessSample.getJackknifeError();

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "JackEstimate:\t" << scientific << skewness << "\t" << error <<  endl;
	}

	//first version including multiple methods
	if(false)
	{
		/**
		 * Skewness gamma_1 is defined as:
		 *   gamma_1 = <(x-mu)^3> / <(x-mu)^2>^(3/2)
		 */
		DataSampleAnalyzer sample(file);
		double x3 = sample.getNthCentralMoment(3);
		double x2 = sample.getNthCentralMoment(2);

		double skewnessBasic = x3/pow(x2, 3./2.);

		DataSampleAnalyzer thirdCentralMoment  = (sample - sample.getNthMoment(1))^3;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double x3_2 = binnedSample1.getNthMoment(1);
		double x2_2 = binnedSample2.getNthMoment(1);

		JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);

		std::cout << "central moments of original sample:" << std::endl;
		std::cout << x3 << " " << x2 << std::endl;

		std::cout << "means of binned sample:" << std::endl;
		std::cout << x3_2 << " " << x2_2 << std::endl;

		double mean1 = jackSample1.getNthMoment(1);
		double mean2 = jackSample2.getNthMoment(1);
		double var1 = jackSample1.getNthCentralMoment(2);
		double var2 = jackSample2.getNthCentralMoment(2);

		std::cout << "means of jackknife estimators" << std::endl;
		std::cout << mean1 << " " << mean2 << std::endl;
		std::cout << var1 << " " << var2 << std::endl;

		double skewness = mean1 / pow(mean2, 3./2.);
		double firstDerivative = fabs( 1./pow(mean2, 3./2.) );
		double secondDerivative = fabs( -3.*mean1/2.*pow(mean2, -5./2.) );
		double error = sqrt( var1 * firstDerivative + var2 * secondDerivative );
		double unbiasedError = error*(double(numberOfBins)/double(numberOfBins-1));

		//		JackknifeEstimators skewnessSample( jackSample1 / jackSample2 );
				JackknifeEstimators skewnessSample( jackSample1 / pow(mean2, 3./2.) );

		double skewness2 = skewnessSample.getNthMoment(1);
		double error2 = skewnessSample.getJackknifeError();

		JackknifeEstimatorsFromBinnedDataSample skewnessSample2( skewnessSample );

		double skewness3 = skewnessSample2.getNthMoment(1);
		double error3 = skewnessSample2.getJackknifeError();

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "NaiveEstimate:\t" << scientific << skewnessBasic << "\t" << unbiasedError <<  endl;
		cout << "jackRatio:\t" << scientific << skewness << "\t" << error << endl;
		cout << "jackMean:\t" << scientific << skewness2 << "\t" << error2 << endl;
		cout << "jackMean2:\t" << scientific << skewness3 << "\t" << error3 << endl;
	}

	//calc skewness and error, v2
	if(false)
	{
		cout << "Skewness v2:" << endl;

		DataSampleAnalyzer sample(file);
		double x2 = sample.getNthCentralMoment(2);
		int numberOfElements = sample.getNumberOfElements();
		double unbiasedSampleVariance = sqrt( double(numberOfElements)/double(numberOfElements-1)*x2 );
		double unbiasedSampleMean = sample.getNthMoment(1);
		double sigmaThree = pow(unbiasedSampleVariance, 3./2.);

		DataSampleAnalyzer binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackSample(binnedSample);

		cout << jackSample.getNthMoment(1) << endl;
		cout << unbiasedSampleMean << endl;
		cout << sigmaThree << endl;

		JackknifeEstimators skewnessSample ( ( (jackSample - unbiasedSampleMean)^3 ) / sigmaThree ) ;

		cout << skewnessSample.getNthMoment(1) << endl;

		for(int i = 0; i<numberOfBins; i++)
		{
			cout << skewnessSample[i] << endl;
		}

		double skewness = skewnessSample.getNthMoment(1);
		double skewnessError = skewnessSample.getJackknifeError();

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "jack:\t\t" << scientific << skewness << "\t" << skewnessError << endl;
	}

	//calc skewness and error, v3
	if(false)
	{
		cout << "Skewness v3:" << endl;

		DataSampleAnalyzer sample(file);
		double x2 = sample.getNthCentralMoment(2);
		int numberOfElements = sample.getNumberOfElements();
		double unbiasedSampleVariance = sqrt( double(numberOfElements)/double(numberOfElements-1)*x2 );
		double unbiasedSampleMean = sample.getNthMoment(1);
		double sigmaThree = pow(unbiasedSampleVariance, 3./2.);

		DataSampleAnalyzer skewnessSample = ( sample -  unbiasedSampleMean );

		DataSampleAnalyzer binnedSample = skewnessSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackSample(binnedSample);

		JackknifeEstimators skewnessSample2 ( ( (jackSample)^3 ) / sigmaThree );

		double skewness = skewnessSample2.getNthMoment(1);
		double skewnessError = skewnessSample2.getJackknifeError();

		cout << "\t\tSkewness\t\tError" << endl;
		cout << "jack:\t\t" << scientific << skewness << "\t" << skewnessError << endl;
	}

	bool analyseKurtosis = true;

	//naive version:
	if(analyseKurtosis)
	{
		/**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer fourthCentralMoment  = (sample - sample.getNthMoment(1))^4;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double x4 = fourthCentralMoment.getNthMoment(1);
		double x2 = secondCentralMoment.getNthMoment(1);

		double kurtosis = x4/pow(x2, 2.);

		//naive error propagation
		double varianceX4 = binnedSample1.getNthCentralMoment(2);
		double varianceX2 = binnedSample2.getNthCentralMoment(2);
		double derivativeX4 = fabs( 1./pow(x2, 2.) );
		double derivativeX2 = fabs( -2.*x4*pow(x2, -3.) );
		double biasedEstimateOfSampleVarianceSquared =  varianceX2 * derivativeX2 + varianceX4 * derivativeX4 ;

		double errorOfMean = sqrt( 1. / double(numberOfBins-1) * biasedEstimateOfSampleVarianceSquared );

		cout << "\t\Binder\t\tError" << endl;
		cout << "NaiveEstimate:\t" << scientific << kurtosis << "\t" << errorOfMean <<  endl;
	}

	//bit better version:
	if(analyseKurtosis)
	{
		/**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer fourthCentralMoment  = (sample - sample.getNthMoment(1))^4;

		double x2 = sample.getNthCentralMoment(2);
		double sigmaFour = x2 * x2;
		fourthCentralMoment /= sigmaFour;

		DataSampleAnalyzer binnedSample = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double kurtosis = binnedSample.getNthMoment(1);

		//todo: make this a fct. of DataSample!
		/**
		 * unbiased estimate of variance of the sample is
		 *   n/(n-1) * biasedEstimator(varianceOfSample)
		 * and the estimate of the variance of the mean of the sample is always:
		 *   varianceEstimator(sample) / n
		 * because of the central limit theorem.
		 */
		//todo: this equals numberOfBins!
		int n = binnedSample.getNumberOfElements();
		double error = sqrt( 1. / double(n-1) * binnedSample.getNthCentralMoment(2) );

		cout << "\t\Binder\t\tError" << endl;
		cout << "Estimate+:\t" << scientific << kurtosis << "\t" << error <<  endl;
	}

	//jackknife version:
	if(analyseKurtosis)
	{
		/**
		 * The Fourth Std. Moment beta_2 is defined as:
		 *   beta_2 = <(x-mu)^4> / <(x-mu)^2>^2
		 * This is also referred to as "Binder-cumulant"
		 * The Kurtosis gamma_2 is defined as:
		 *   gamma_2 = beta_2 - 3
		 */
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer fourthCentralMoment  = (sample - sample.getNthMoment(1))^4;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);

		//this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
		JackknifeEstimators kurtosisSample( jackSample1 / ( jackSample2^2 ) );

		double skewness = kurtosisSample.getNthMoment(1);
		double error = kurtosisSample.getJackknifeError();

		cout << "\t\tBinder\t\tError" << endl;
		cout << "JackEstimate:\t" << scientific << skewness << "\t" << error <<  endl;
	}

	//calc kurtosis and error
	if(false)
	{
		/**
		 * Following Wikipedia:
		 * 4. Standardized Moment: beta2 = < (x - mu)^4 > / <(x-mu)^2>^2
		 * Kurtosis = beta2 - 3
		 */

		DataSampleAnalyzer sample(file);
		double x4 = sample.getNthCentralMoment(4);
		double x2 = sample.getNthCentralMoment(2);

		double fourthStandardizedMomentBasic = x4/pow(x2, 2.);
		//todo: think about different definition (-3)
		double kurtosisBasic = fourthStandardizedMomentBasic - 3.;

		DataSampleAnalyzer fourthCentralMoment  = (sample - sample.getNthMoment(1))^4;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = fourthCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);

		double mean1 = jackSample1.getNthMoment(1);
		double mean2 = jackSample2.getNthMoment(1);
		double var1 = jackSample1.getNthCentralMoment(2);
		double var2 = jackSample2.getNthCentralMoment(2);

		std::cout << "means of jackknife estimators" << std::endl;
		std::cout << mean1 << " " << mean2 << std::endl;
		std::cout << var1 << " " << var2 << std::endl;

		double fourthStandardizedMoment = mean1 / pow(mean2, 2.);
		double firstDerivative = fabs( 1./pow(mean2, 3./2.) );
		double secondDerivative = fabs( -2.*mean1*pow(mean2, -3.) );
		double error = sqrt( var1 * firstDerivative + var2 * secondDerivative );
		double unbiasedError = error*(double(numberOfBins)/double(numberOfBins-1));

		JackknifeEstimators binderSample( jackSample1 / pow(mean2, 2.) );

		double binder = binderSample.getNthMoment(1);
		double binderError = binderSample.getJackknifeError();

		JackknifeEstimatorsFromBinnedDataSample binderSample2( binderSample );

		double binder2 = binderSample2.getNthMoment(1);
		double binderError2 = binderSample2.getJackknifeError();

		cout << "\t\t\"Binder\"\t\tError" << endl;
		cout << scientific << "Naive:\t\t" << fourthStandardizedMomentBasic << "\t" << error << endl;
		cout << scientific << "JackMean:\t" << binder << "\t" << binderError << endl;
		cout << scientific << "JackMean2:\t" << binder2 << "\t" << binderError2 << endl;

		cout << "Kurtosis\t\tError" << endl;
		cout << scientific << kurtosisBasic << "\t" << error << endl;
	}


  return 0;
}
