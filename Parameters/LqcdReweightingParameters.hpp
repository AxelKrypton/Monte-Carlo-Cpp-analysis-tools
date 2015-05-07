#ifndef LQCDREWEIGHTINGPARAMETERS_HPP_
#define LQCDREWEIGHTINGPARAMETERS_HPP_

#include "iostream"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

//TODO: Merge with other Parameters class

class ReweightingParameters{
	/*
	 * TODO: implement! Here the methods for a general reweighting in N par should be put.
	 *       Then the LqcdReweightingParameters should be a particular case of this class.
	 *       One idea is to make some getters here be overwritten by those of the child class.
	 *       For example here getNewBetaRange_low should return a std::vector<double> and
	 *       in LqcdReweightingParameters just one double. Actually I do not know if this can
	 *       be done overwriting methods, maybe a class template would be better. For the moment
	 *       leave like that since we can use polymorphism in the Reweighter class constructor!
	 */
};


class LqcdReweightingParameters : ReweightingParameters
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
	bool getUseJackknifeAsErrorMethod();
	bool getUseBootstrapAsErrorMethod();
	int getNumberOfBootstrapResample();
	double getWeightPrecision();
	bool getIsMeanKnownToBeZero();

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
	double weightPrecision;
	int numberOfBootstrapResample;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    bool isMeanKnownToBeZero;
};

#endif
