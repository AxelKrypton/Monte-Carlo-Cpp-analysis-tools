#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

//todo: make own .cpp file plus tests

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

//todo: remove this again
using namespace std;

//todo: rename to Parameters
class parameters
{
public:
	struct parse_aborted {};
	int binsize;
	int numberOfBins;
	bool useBinning;
	bool calcAutocorrelation;
	int offset;
	std::string file;
	bool analyseMean;
	bool analyseVariance;
	bool analyseSkewness;
	bool analyseKurtosis;
	std::string defaultFile;

	parameters(int argc, char ** argv)
	{
		defaultFile = "";

		po::options_description desc("Generic options");
		po::variables_map vm;

		//todo: find out why short names gives parsing error!
		//todo: add positional operator for data file!
		//todo: bool should not have to be set!
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
			("calcAutocorrelation,a", po::value<bool>(&calcAutocorrelation)->default_value(false), "Estimate autocorrelation of data")
		;

		po::store(po::parse_command_line(argc, argv, desc), vm);

		if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
			std::cout << desc << '\n';
			throw parameters::parse_aborted();
		}

		po::notify(vm);
		printParameters();

		if (file == defaultFile)
		{
			throw std::invalid_argument("No datafile given. Aborting!");
		}
	}

private:
	void printParameters()
	{
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
	}
};

#endif /* PARAMETERS_HPP_ */
