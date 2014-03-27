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

	desc.add_options()
		("help,h", "Produce this help message")
		("datafile,f", po::value<std::string>(&file)->default_value(""), "File containing data")
		("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
		("useBinning", po::value<bool>(&useBinning)->default_value(true), "Use binning on data")
		("binsize,bs", po::value<int>(&numberOfBins)->default_value(10), "Size of bin")
		("numberOfBins,nb", po::value<int>(&binsize)->default_value(10), "Number of bins")
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

	if (file == "")
		return 0;

	if (calcAutocorrelation)
	{
		cout << "Autocorrelation is not implemented yet. Aborting!" << endl;
		return 0;
	}

	//calc mean and error
	if(true)
	{
		DataSampleAnalyzer sample(file);
		DataSample binnedSample = sample.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		JackknifeEstimatorsFromBinnedDataSample jackSample(binnedSample);

		cout << "Mean\t\tError" << endl;
		cout << scientific << sample.getNthMoment(1) << "\t" << jackSample.getJackknifeError() << endl;
	}

	//calc var and error
	if(true)
	{
		DataSampleAnalyzer sample(file);

		DataSampleAnalyzer varianceSample = ( sample - sample.getNthMoment(1))^2;

		DataSample binnedSample = varianceSample.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		JackknifeEstimatorsFromBinnedDataSample jackSample(binnedSample);

		cout << "Variance\t\tError" << endl;
		cout << scientific << varianceSample.getNthMoment(1) << "\t" << jackSample.getJackknifeError() << endl;
	}

	//calc skewness and error
	if(true)
	{
		DataSampleAnalyzer sample(file);
		double x3 = sample.getNthCentralMoment(3);
		double x2 = sample.getNthCentralMoment(2);

		DataSampleAnalyzer thirdCentralMoment = (sample - sample.getNthMoment(1))^3;
		DataSampleAnalyzer secondCentralMoment = (sample - sample.getNthMoment(1))^2;

		DataSampleAnalyzer binnedSample1 = thirdCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);
		DataSampleAnalyzer binnedSample2 = secondCentralMoment.createBinnedDataSampleWithNumberOfBins(numberOfBins);

		double x3_2 = binnedSample1.getNthMoment(1);
		double x2_2 = binnedSample2.getNthMoment(1);

		JackknifeEstimatorsFromBinnedDataSample jackSample1(binnedSample1);
		JackknifeEstimatorsFromBinnedDataSample jackSample2(binnedSample2);

		JackknifeEstimators skewnessSample( jackSample1 / jackSample2 );

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

		//need sqrt for mean2!!!
		double skewness = mean1 / mean2;
		double error = sqrt( fabs(var1 / mean2 / mean2) + fabs(var2 * mean1 / mean2 / mean2 / mean2 / mean2) );
		double unbiasedError = error*(double(numberOfBins)/double(numberOfBins-1));

		double skewness2 = skewnessSample.getNthMoment(1);
		double error2 = skewnessSample.getJackknifeError();

		cout << "Skewness\t\tError" << endl;
		cout << scientific << skewness << "\t" << error << endl;
		cout << scientific << "\t\t" << unbiasedError << endl;
		cout << scientific << skewness2 << "\t" << error2 << endl;
	}

	//calc kurtosis and error
	if(true)
	{
		DataSampleAnalyzer sample(file);
		double x4 = sample.getNthCentralMoment(4);
		double x2 = sample.getNthCentralMoment(2);

		//todo: think about different definition (-3)
		double kurtosis = x4 / (x2 * x2);

		double error = -1.;

		cout << "Kurtosis\t\tError" << endl;
		cout << scientific << kurtosis << "\t" << error << endl;
	}


  return 0;
}
