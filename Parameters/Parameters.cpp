#include "Parameters.hpp"

Parameters::Parameters(int argc, const char ** argv)
{
	po::options_description desc("   Options for data analysis.\nUsage: \"--<optionName>=<value>\" (or \"-<shortOptionName><value>\")\nNote that boolean options can be changed from their default value implicitly, ie without giving explicitly true or false in the command line.\nFor example, \"--useBinning\" equals \"--useBinning=false\" (as the default value is true)");
	po::variables_map vm;
	po::positional_options_description positionalOptions;

	//todo: Maybe it would be nicer to put the observables into a vector
	/**
	 * Apparently, when using short options with int one has to do "-n99"
	 * because otherwise the empty space causes an error.
	 */
	desc.add_options()
		("help,h", "Produce this help message")
		("file,f", po::value<std::string>(&file), "File containing data")
		("analysisOutputFilePrefix", po::value<std::string>(&analysisOutputFilePrefix)->default_value(""), "Prefix for filename of analysis results")
		("analysisOutputFilePostfix", po::value<std::string>(&analysisOutputFilePostfix)->default_value("_stat"), "Postfix for filename of analysis results")
		("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
		("analyzeMean", po::value<bool>(&analyzeMean)->default_value(true)->implicit_value(false), "Analyse data for mean")
		("analyzeVariance", po::value<bool>(&analyzeVariance)->default_value(true)->implicit_value(false), "Analyse data for variance")
		("analyzeSkewness", po::value<bool>(&analyzeSkewness)->default_value(true)->implicit_value(false), "Analyse data for skewness")
		("analyzeKurtosis", po::value<bool>(&analyzeKurtosis)->default_value(true)->implicit_value(false), "Analyse data for kurtosis/binder-cumulant")
		("useBinning", po::value<bool>(&useBinning)->default_value(true)->implicit_value(false), "Use binning on data")
		("useNumberOfBinsForBinning", po::value<bool>(&useNumberOfBinsForBinning)->default_value(true)->implicit_value(false), "Perform binning based on \"numberOfBins\" parameter")
		("binningMustFitDataSampleSize", po::value<bool>(&binningMustFitDataSampleSize)->default_value(false)->implicit_value(true), "Require that no element of the data sample is discarded during binning")
		("adjustDataSampleSizeToBinning", po::value<bool>(&adjustDataSampleSizeToBinning)->default_value(true)->implicit_value(true), "Adjust number of elements of the data sample if elements are discarded during binning")
		("binsize,b", po::value<int>(&binsize), "Size of bin (default: 100)")
		("numberOfBins,n", po::value<int>(&numberOfBins), "Number of bins (default: 10)")
		("calcAutocorrelation,a", po::value<bool>(&calcAutocorrelation)->default_value(false)->implicit_value(true), "Estimate autocorrelation of data. In this case no other observable is evaluated!")
		("numberOfBinsForAutocorrelation", po::value<int>(&numberOfBinsForAutocorrelation), "Number of bins for the estimate of the autocorrelation time (default: 10)")
		("timeMaxAutocorrelationFunction", po::value<int>(&timeMaxAutocorrelationFunction), "Maximum data distance for the estimate of the autocorrelation function (needed parameter).")
		;

	//option "file" can be given without option description
	positionalOptions.add("file", 1);
	po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);
	po::notify(vm);

	checkParsedArguments(vm, desc);
	printParameters();
}

void Parameters::checkParsedArguments(po::variables_map & vm, po::options_description & desc)
{
	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		throw Parameters::parse_aborted();
	}

	if (! vm.count("file"))
	{
		throw std::invalid_argument("No datafile given. Aborting!");
	}

	if (calcAutocorrelation && (! vm.count("timeMaxAutocorrelationFunction")))
	{
		throw std::invalid_argument("If calcAutocorrelation==true then the option --timeMaxAutocorrelationFunction=... must be given. Aborting!");
	}

	/**
	 * For the binning one has to know if it should be
	 * performed with numberOfBins or with binsize parameter.
	 * This can be judged with the "useNumberOfBinsForBinning"
	 * parameter. However, it should be that this value
	 * is changed automatically if a binsize is given in the
	 * command line.
	 * If a default value is assigned, the option is regarded as set.
	 * This makes it difficult to see what was in the command line
	 * as "count" is always true.
	 * Hence, no default value is given in the definition of
	 * the options but it is assigned as done below.
	 * Probaply this can be done better with the "notifier"
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
	if(!vm.count("numberOfBinsForAutocorrelation"))
	{
		numberOfBinsForAutocorrelation = 10;
	}
	if(vm.count("binsize"))
	{
		if(vm.count("numberOfBins"))
		{
			throw std::invalid_argument("Not clear what binning parameter to use, both \"numberOfBins\" and \"binsize\" have been set. Aborting!");
		}
		useNumberOfBinsForBinning = false;
	}

}

void Parameters::printParameters()
{
	std::cout << "###############################" << std::endl;
	std::cout << "# Options:" << std::endl;
	std::cout << "###############################" << std::endl;
	std::cout << "# Datafile:\t" << file << std::endl;
	std::cout << "# Offset:\t" << offset << std::endl;
	//todo: add output of observables which are analyzed
	std::cout << "###############################" << std::endl;
	if (useBinning)
	{
		std::cout << "# Perform binning with:" << std::endl;
		if( useNumberOfBinsForBinning)
		{
			std::cout << "# Number of bins:\t" << numberOfBins << std::endl;
		}
		else
		{
			std::cout << "# Binsize:\t" << binsize << std::endl;
		}
		if( binningMustFitDataSampleSize )
		{
			std::cout << "# Require binsize/numberOfBins\n#   to be multiple of number of\n#   data points" << std::endl;
		}
		if( adjustDataSampleSizeToBinning )
		{
			std::cout << "# Resize raw data sample in case data points are discarded during binning" << std::endl;
		}
	}
	else
		std::cout << "# Do not perform binning!" << std::endl;
	if (calcAutocorrelation)
	{
		std::cout << "###############################" << std::endl;
		std::cout << "# Calculate estimate of autocorrelation time:" << std::endl;
		std::cout << "#  - for \"t\" up to " << timeMaxAutocorrelationFunction << "," << std::endl;
		std::cout << "#  - using " << numberOfBinsForAutocorrelation << " bins to bin the data before" << std::endl;
		std::cout << "#    applying Jackknife." << std::endl;
	}
	std::cout << "###############################" << std::endl;
}
