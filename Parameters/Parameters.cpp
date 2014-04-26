#include "Parameters.hpp"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

//todo: remove this again
using namespace std;

Parameters::Parameters(int argc, const char ** argv)
{
	std::string defaultFile = "";

	po::options_description desc("   Options for data analysis.\nNote that boolean options can be changed from their default value implicitly, ie without giving explicitly true or false in the command line.\nFor example, \"--useBinning\" equals \"--useBinning=false\" (as the default value is true)");
	po::variables_map vm;

	//todo: Maybe it would be nicer to put the observables into a vector!
	//todo: add positional operator for data file!
	/**
	 * Apparently, when using short options with int one has to do "-n99"
	 * because otherwise the empty space will be treated as a number which
	 * causes an error.
	 */
	desc.add_options()
		("help,h", "Produce this help message")
		("file,f", po::value<std::string>(&file)->default_value(defaultFile), "File containing data")
		("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
		("analyzeMean", po::value<bool>(&analyzeMean)->default_value(true)->implicit_value(false), "Analyse data for mean")
		("analyzeVariance", po::value<bool>(&analyzeVariance)->default_value(true)->implicit_value(false), "Analyse data for variance")
		("analyzeSkewness", po::value<bool>(&analyzeSkewness)->default_value(true)->implicit_value(false), "Analyse data for skewness")
		("analyzeKurtosis", po::value<bool>(&analyzeKurtosis)->default_value(true)->implicit_value(false), "Analyse data for kurtosis/binder-cumulant")
		("useBinning", po::value<bool>(&useBinning)->default_value(true)->implicit_value(false), "Use binning on data")
		("binsize,b", po::value<int>(&binsize)->default_value(100), "Size of bin")
		("numberOfBins,n", po::value<int>(&numberOfBins)->default_value(10), "Number of bins")
		("calcAutocorrelation,a", po::value<bool>(&calcAutocorrelation)->default_value(false)->implicit_value(true), "Estimate autocorrelation of data")
		;

	po::store(po::parse_command_line(argc, argv, desc), vm);

	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		throw Parameters::parse_aborted();
	}

	po::notify(vm);
	printParameters();

	if (file == defaultFile)
	{
		throw std::invalid_argument("No datafile given. Aborting!");
	}
}

void Parameters::printParameters()
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
