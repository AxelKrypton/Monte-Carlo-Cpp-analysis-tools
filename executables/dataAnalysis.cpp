#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

#include "../dataAnalysisUtilities/jackknifeEstimators.hpp"

//todo: remove this again
using namespace std;

//todo: make this a fct. of DataSample or put it in a namespace!
//todo: enlarge for binsize..
std::pair<double,double> calcMeanAndErrorOfDataSampleWithBinning(DataSampleAnalyzer sampleIn, int numberOfBins)
{
	DataSampleAnalyzer binnedSample = sampleIn.createBinnedDataSampleWithNumberOfBins(numberOfBins);
	double mean = binnedSample.getNthMoment(1);
	/**
	 * unbiased estimate of variance of the sample is
	 *   n/(n-1) * biasedEstimator(varianceOfSample)
	 * and the estimate of the variance of the mean of the sample is always:
	 *   varianceEstimator(sample) / n
	 * because of the central limit theorem.
	 * Note that for the mean the unbiased variance yields
	 * the same error as jackknifing.
	 */
	int n = binnedSample.getNumberOfElements();
	double error = sqrt(1. / double(n - 1) * binnedSample.getNthCentralMoment(2));
	return std::pair <double, double> (mean, error);
}

void calcMean(std::string & file, int & numberOfBins)
{
    cout << "Analyse mean..." << endl;
    DataSample sample(file);

    std::pair<double,double> meanAndError = calcMeanAndErrorOfDataSampleWithBinning(sample, numberOfBins);

    cout << "Mean\t\tError" << endl;
    cout << scientific << meanAndError.first << "\t" << meanAndError.second << endl;
}

void calcVariance(std::string & file, int & numberOfBins)
{
    cout << "Analyse variance..." << endl;
    DataSampleAnalyzer sample(file);
    DataSampleAnalyzer varianceSample = (sample - sample.getNthMoment(1)) ^ 2;

    std::pair<double,double> meanAndError = calcMeanAndErrorOfDataSampleWithBinning(varianceSample, numberOfBins);

    cout << "Variance\t\tError" << endl;
    cout << scientific << meanAndError.first << "\t" << meanAndError.second << endl;
}

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
    cout << "\t\tSkewness\t\tError" << endl;
    cout << "JackEstimate:\t" << scientific << skewness << "\t" << error << endl;
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
    //this calculates x_i / y_i, where x_i and y_i are the jackknife estimators of the third and second moment
    JackknifeEstimators kurtosisSample(jackSample1 / (jackSample2 ^ 2));
    double skewness = kurtosisSample.getNthMoment(1);
    double error = kurtosisSample.getJackknifeError();
    cout << "\t\tBinder\t\tError" << endl;
    cout << "JackEstimate:\t" << scientific << skewness << "\t" << error << endl;
}

int main(int argc, char ** argv)
{
	//todo: add try-catch block

	int binsize;
	int numberOfBins;
	bool useBinning;
	bool calcAutocorrelation;
	int offset;
	std::string file;
	bool analyseMean = true;
	bool analyseVariance = true;
	bool analyseSkewness = true;
	bool analyseKurtosis = true;
	std::string defaultFile = "";

	po::options_description desc("Generic options");
	po::variables_map vm;

	//todo: find out why short names gives parsing error!
	desc.add_options()
		("help,h", "Produce this help message")
		("datafile,f", po::value<std::string>(&file)->default_value(defaultFile), "File containing data")
		("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
		("analyseMean", po::value<bool>(&analyseMean)->default_value(true), "Analyse data for mean")
		("analyseVariance", po::value<bool>(&analyseVariance)->default_value(true), "Analyse data for variance")
		("analyseSkewness", po::value<bool>(&analyseSkewness)->default_value(true), "Analyse data for skewness")
		("analyseKurtosis", po::value<bool>(&analyseKurtosis)->default_value(true), "Analyse data for kurtosis/binder-cumulant")
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

	cout << "###############################" << endl;
	cout << "Options:" << endl;
	cout << "###############################" << endl;
	cout << "Datafile:\t" << file << endl;
	cout << "Offset:\t" << offset << endl;
	//todo: add output of observables which are analyzed
	cout << "###############################" << endl;
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

	if (file == defaultFile)
		return 0;

	if (calcAutocorrelation)
	{
		cout << "Autocorrelation is not implemented yet. Aborting!" << endl;
		return 0;
	}

	if(analyseMean)
	{
	    calcMean(file, numberOfBins);
	}

	if(analyseVariance)
	{
		calcVariance(file, numberOfBins);
	}

	if(analyseSkewness)
	{
	    calcSkewness(file, numberOfBins);
	}

	if(analyseKurtosis)
	{
	    calcKurtosis(file, numberOfBins);
	}

  return 0;
}
