#include "ReweightingParameters.hpp"

ReweightingParameters::ReweightingParameters(int argc, const char ** argv) 
{
	po::options_description desc("   Options for reweighting of data.");
	po::variables_map vm;
	po::positional_options_description positionalOptions;

	//todo: Maybe it would be nicer to put the observables into a vector
	/**
	 * Apparently, when using short options with int one has to do "-n99"
	 * because otherwise the empty space causes an error.
	 */
	desc.add_options()
		("help,h", "Produce this help message")
		("file,f", po::value<std::string>(&inputfile), "Inputfile containing metainformation for reweighting procedure.")
		("numberOfNewPoints", po::value<uint>(&numberOfNewPoints)->default_value(2), "Number of new points to produce with reweighting.")
		("newRange_high", po::value<double>(&newRange_high)->default_value(2), "Upper limit of new range of to cover with reweighting.")
		("newRange_low", po::value<double>(&newRange_low)->default_value(1), "Lower limit of new range of to cover with reweighting.");

	//option "file" can be given without option description
	positionalOptions.add("file", 1);
	po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);
	po::notify(vm);
	
	checkParsedArguments(vm, desc);
}

void ReweightingParameters::checkParsedArguments(po::variables_map & vm, po::options_description & desc)
{
	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		throw ReweightingParameters::parse_aborted();
	}

	if (! vm.count("file"))
	{
		throw std::invalid_argument("No datafile given. Aborting!");
	}
}

uint ReweightingParameters::getNumberOfNewPoints()
{
	return numberOfNewPoints;
}

std::string ReweightingParameters::getInputfile()
{
	return inputfile;
}

double ReweightingParameters::getNewRange_low()
{
	return newRange_low;
}

double ReweightingParameters::getNewRange_high()
{
	return newRange_high;
}


