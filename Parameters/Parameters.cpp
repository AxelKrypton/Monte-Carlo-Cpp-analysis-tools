#include "Parameters.hpp"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

Parameters::Parameters(int argc, const char ** argv)
{
	std::string defaultFile = "";
	//todo: make this a real option
	useNumberOfBinsForBinning = true;

	po::options_description desc("   Options for data analysis.\nUsage: \"--<optionName>=<value>\" (or \"-<shortOptionName><value>\")\nNote that boolean options can be changed from their default value implicitly, ie without giving explicitly true or false in the command line.\nFor example, \"--useBinning\" equals \"--useBinning=false\" (as the default value is true)");
	po::variables_map vm;

	//todo: Maybe it would be nicer to put the observables into a vector
	/**
	 * Apparently, when using short options with int one has to do "-n99"
	 * because otherwise the empty space causes an error.
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
		("binsize,b", po::value<int>(&binsize), "Size of bin (default: 100)")
		("numberOfBins,n", po::value<int>(&numberOfBins), "Number of bins (default: 10)")
		("calcAutocorrelation,a", po::value<bool>(&calcAutocorrelation)->default_value(false)->implicit_value(true), "Estimate autocorrelation of data")
		;

	//option "file" can be given without option description
	po::positional_options_description positionalOptions;
	positionalOptions.add("file", 1);

	po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);

	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		throw Parameters::parse_aborted();
	}

	/**
	 * For the binning one has to know if it should be
	 * performed with numberOfBins or with binsize parameter.
	 * If a default value is assigned, the option counts as set.
	 * This makes judgement difficult.
	 * Hence, no default value is given in the definition of
	 * the options but it is assigned as done below.
	 * Propably this can be done better with the "notifier"
	 * functionality of boost.
	 */

	//check if numberOfBins or binsize have been set, otherwise set them.
	if(!vm.count("numberOfBins"))
	{
		numberOfBins = 10;
	}
	if(!vm.count("binsize"))
	{
		binsize = 100;
	}

	//todo: add tests
	//todo: add check on bool parameter, otherwise default value of binsize is useless!
	//check if numberOfBins or binsize shall be used for binning
	if(vm.count("binsize"))
	{
		if(vm.count("numberOfBins"))
		{
			throw std::invalid_argument("Not clear what binning parameter to use, both \"numberOfBins\" and \"binsize\" have been set. Aborting!");
		}
		useNumberOfBinsForBinning = false;
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
	std::cout << "###############################" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "###############################" << std::endl;
	std::cout << "Datafile:\t" << file << std::endl;
	std::cout << "Offset:\t" << offset << std::endl;
	//todo: add output of observables which are analyzed
	std::cout << "###############################" << std::endl;
	if (useBinning)
	{
		std::cout << "binsize:\t" << binsize << std::endl;
		std::cout << "number of bins:\t" << numberOfBins << std::endl;
	}
	else
		std::cout << "Do not use binning!" << std::endl;
	if (calcAutocorrelation)
		std::cout << "Calculate estimate on autocorrelation" << std::endl;
	std::cout << "###############################" << std::endl;
}
