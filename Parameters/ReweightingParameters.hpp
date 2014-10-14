#ifndef REWEIGHTINGPARAMETERS_HPP_
#define REWEIGHTINGPARAMETERS_HPP_

#include "iostream"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;


//TODO: Merge with other Parameters class
class ReweightingParameters
{
public:
	ReweightingParameters(int argc, const char ** argv);
	
	std::string getInputfile();
	uint getNumberOfNewPoints();
	double getNewRange_low();
	double getNewRange_high();
	
	struct parse_aborted {};
	
private:
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
	
	std::string inputfile;
	uint numberOfNewPoints;
	double newRange_low;
	double newRange_high;
};

#endif