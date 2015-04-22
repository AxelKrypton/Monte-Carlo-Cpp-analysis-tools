#ifndef LQCDREWEIGHTINGPARAMETERS_HPP_
#define LQCDREWEIGHTINGPARAMETERS_HPP_

#include "iostream"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;


//TODO: Merge with other Parameters class
class LqcdReweightingParameters
{
public:
	LqcdReweightingParameters(int argc, const char ** argv);
	
	void printParameters();
	
	std::string getInputfile();
	std::string getOutputfilePrefix();
	uint getNumberOfNewBetaPoints();
	double getNewBetaRange_low();
	double getNewBetaRange_high();
	bool getDeactivateReweightingForMean();
	bool getDeactivateReweightingForVariance();
	bool getDeactivateReweightingForSkewness();
	bool getDeactivateReweightingForBinder();
    std::vector<unsigned int> getColumnsToBeReweightedUsingMultipleColumns();
	std::vector<unsigned int> getColumnsWhoseMeanIsKnownToBeZero();
	bool getUseJackknifeAsErrorMethod();
	bool getUseBootstrapAsErrorMethod();
	int getNumberOfBootstrapResample();

	struct parse_aborted {};
	
private:
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
	
	std::string inputfile;
	std::string outputfilePrefix;
	uint numberOfNewBetaPoints;
	double newBetaRange_low;
	double newBetaRange_high;
	bool deactivateReweightingForMean;
	bool deactivateReweightingForVariance;
	bool deactivateReweightingForSkewness;
	bool deactivateReweightingForBinder;
	bool useJackknifeAsErrorMethod;
	bool useBootstrapAsErrorMethod;
	int numberOfBootstrapResample;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    std::vector<unsigned int> columnsWhoseMeanIsKnownToBeZero;
};

#endif
