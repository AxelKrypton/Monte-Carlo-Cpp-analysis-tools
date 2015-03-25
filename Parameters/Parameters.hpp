#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

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
	int numberOfBins;
	int numberOfBinsForAutocorrelation;
	int timeMaxAutocorrelationFunction;
	int offset;
	int column;
	bool isMeanKnownToBeZero;
	bool useBinning;
	bool useNumberOfBinsForBinning;
	bool calcAutocorrelation;
	bool analyzeMean;
	bool analyzeVariance;
	bool analyzeSkewness;
	bool analyzeKurtosis;
	bool binningMustFitDataSampleSize;
	bool adjustDataSampleSizeToBinning;

private:
	void printParameters();
	void setBinningTypeParameter(bool valueIn);
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
};

#endif /* PARAMETERS_HPP_ */
