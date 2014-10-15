#include "LqcdReweightingParameters.hpp"

LqcdReweightingParameters::LqcdReweightingParameters(int argc, const char ** argv) 
{
	po::options_description desc("   Options for reweighting of data.");
	po::variables_map vm;
	po::positional_options_description positionalOptions;

	desc.add_options()
		("help,h", "Produce this help message")
		("file,f", po::value<std::string>(&inputfile), "Inputfile containing metainformation for reweighting procedure.")
		("numberOfNewPoints", po::value<uint>(&numberOfNewPoints)->default_value(2), "Number of new points to produce with reweighting.")
		("newRange_high", po::value<double>(&newRange_high)->default_value(2), "Upper limit of new range of to cover with reweighting.")
		("newRange_low", po::value<double>(&newRange_low)->default_value(1), "Lower limit of new range of to cover with reweighting.")
		("deactivateReweightingForMean", po::value<bool>(&deactivateReweightingForMean)->default_value(false)->implicit_value(true), "Do not perform reweighting for the mean of the data.")
		("deactivateReweightingForVariance", po::value<bool>(&deactivateReweightingForVariance)->default_value(false)->implicit_value(true), "Do not perform reweighting for the variance of the data.")
		("deactivateReweightingForSkewness", po::value<bool>(&deactivateReweightingForSkewness)->default_value(false)->implicit_value(true), "Do not perform reweighting for the skewness of the data.")
		("deactivateReweightingForBinder", po::value<bool>(&deactivateReweightingForBinder)->default_value(false)->implicit_value(true), "Do not perform reweighting for the binder cumulant of the data.");
		
	//option "file" can be given without option description
	positionalOptions.add("file", 1);
	po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);
	po::notify(vm);
	
	checkParsedArguments(vm, desc);
	printParameters();
}

void LqcdReweightingParameters::checkParsedArguments(po::variables_map & vm, po::options_description & desc)
{
	if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
		std::cout << desc << '\n';
		throw LqcdReweightingParameters::parse_aborted();
	}

	if (! vm.count("file"))
	{
		throw std::invalid_argument("No datafile given. Aborting!");
	}
}

void LqcdReweightingParameters::printParameters()
{
	std::string separator = "###################################################";
	
	std::cout << separator << std::endl;
	std::cout << "# Options:" << std::endl;
	std::cout << separator << std::endl;
	std::cout << "# Inputfile:\t" << inputfile << std::endl;
	std::cout << separator << std::endl;
	std::cout << "# Reweighting parameters:" << std::endl;
	std::cout << "# New beta range:\t[" << newRange_low << ":" << newRange_high << "]" << std::endl;
	std::cout << "# New points:\t\t" << numberOfNewPoints << std::endl;
	std::cout << separator << std::endl;
	std::cout << "# Observables:" << std::endl;
	if ( deactivateReweightingForMean )
	{
		std::cout << "#\tDo NOT reweight mean of data" << std::endl;
	}
	else
	{
		std::cout << "#\tMean of data" << std::endl;
	}
	if ( deactivateReweightingForVariance )
	{
		std::cout << "#\tDo NOT reweight variance of data" << std::endl;
	}
	else
	{
		std::cout << "#\tVariance of data" << std::endl;
	}
	if ( deactivateReweightingForSkewness )
	{
		std::cout << "#\tDo NOT reweight skewness of data" << std::endl;
	}
	else
	{
		std::cout << "#\tSkewness of data" << std::endl;
	}
	if ( deactivateReweightingForBinder )
	{
		std::cout << "#\tDo NOT reweight binder cumulant of data" << std::endl;
	}
	else
	{
		std::cout << "#\tBinder cumulant of data" << std::endl;
	}
	std::cout << separator << std::endl;
}

uint LqcdReweightingParameters::getNumberOfNewPoints()
{
	return numberOfNewPoints;
}

std::string LqcdReweightingParameters::getInputfile()
{
	return inputfile;
}

double LqcdReweightingParameters::getNewRange_low()
{
	return newRange_low;
}

double LqcdReweightingParameters::getNewRange_high()
{
	return newRange_high;
}

bool LqcdReweightingParameters::getDeactivateReweightingForMean()
{
	return deactivateReweightingForMean;
}

bool LqcdReweightingParameters::getDeactivateReweightingForVariance()
{
	return deactivateReweightingForVariance;
}

bool LqcdReweightingParameters::getDeactivateReweightingForSkewness()
{
	return deactivateReweightingForSkewness;
}

bool LqcdReweightingParameters::getDeactivateReweightingForBinder()
{
	return deactivateReweightingForBinder;
}

