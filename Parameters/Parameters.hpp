#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "../types.hpp"

#include "iostream"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

class Parameters
{
public:
	Parameters(int argc, const char ** argv);

	struct parse_aborted {};
	std::string file;
	std::string analysisOutputFilePrefix;
	std::string analysisOutputFilePostfix;
	int binsize;
	std::vector<int> binsizeMoments;
	std::vector<int> binsizeCentralMoments;
	int numberOfBins;
	std::vector<int> numberOfBinsMoments;
	std::vector<int> numberOfBinsCentralMoments;
	int numberOfBinsForAutocorrelation;
	int timeMaxAutocorrelationFunction;
	int offset;
	int column;
	bool isMeanKnownToBeZero;
	bool doNotUseBinning;
	bool useNumberOfBinsForBinning;
	bool calcAutocorrelation;
	bool doNotAnalyzeMean;
	bool doNotAnalyzeVariance;
	bool doNotAnalyzeSkewness;
	bool doNotAnalyzeBinderCumulant;
	bool binningMustFitDataSampleSize;
	bool adjustDataSampleSizeToBinning;

private:
	void printParameters();
	void setBinningTypeParameter(bool valueIn);
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
	void parseBinningInformationForMoments(std::vector<int>& vectorWithBinningInformations, const int defaultValue);
};

#endif /* PARAMETERS_HPP_ */
