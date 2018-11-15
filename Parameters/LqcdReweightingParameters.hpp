#ifndef LQCDREWEIGHTINGPARAMETERS_HPP_
#define LQCDREWEIGHTINGPARAMETERS_HPP_

#include "../types.hpp"
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
	 *       For example here getNewBetaRange_low should return a std::vector<realFloat> and
	 *       in LqcdReweightingParameters just one realFloat. Actually I do not know if this can
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
	unsigned int getNumberOfNewBetaPoints();
	realFloat getNewBetaRange_low();
	realFloat getNewBetaRange_high();
	bool getDeactivateReweightingForMean();
	bool getDeactivateReweightingForVariance();
	bool getDeactivateReweightingForSkewness();
	bool getDeactivateReweightingForKurtosis();
    std::vector<unsigned int> getColumnsToBeReweightedUsingMultipleColumns();
	bool getUseJackknifeAsErrorMethod();
	bool getUseBootstrapAsErrorMethod();
	bool getUseSimulatedPointsAsNewPoints();
	bool getPrintEstimatorsToFile();
	int getNumberOfBootstrapResample();
	realFloat getWeightPrecision();
	bool getIsMeanKnownToBeZero();
	unsigned int getNumberOfMultipleColumnsForSingleObservable();

	struct parse_aborted {};
	
private:
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
	
	std::string inputfile;
	std::string outputfilePrefix;
	unsigned int numberOfNewBetaPoints;
	realFloat newBetaRange_low;
	realFloat newBetaRange_high;
	bool deactivateReweightingForMean;
	bool deactivateReweightingForVariance;
	bool deactivateReweightingForSkewness;
	bool deactivateReweightingForKurtosis;
	bool useJackknifeAsErrorMethod;
	bool useBootstrapAsErrorMethod;
	bool useSimulatedPointsAsNewPoints;
	bool printEstimatorsToFile;
	realFloat weightPrecision;
	int numberOfBootstrapResample;
    std::vector<unsigned int> columnsToBeReweightedUsingMultipleColumns;
    bool isMeanKnownToBeZero;
    unsigned int numberOfMultipleColumnsForSingleObservable;
};

#endif
