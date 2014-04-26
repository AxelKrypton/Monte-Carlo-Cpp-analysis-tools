#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "iostream"

class Parameters
{
public:
	Parameters(int argc, const char ** argv);

	struct parse_aborted {};
	std::string file;
	int binsize;
	int numberOfBins;
	int offset;
	bool useBinning;
	bool useNumberOfBinsForBinning;
	bool calcAutocorrelation;
	bool analyzeMean;
	bool analyzeVariance;
	bool analyzeSkewness;
	bool analyzeKurtosis;

private:
	void printParameters();
	void setBinningTypeParameter(bool valueIn);
};

#endif /* PARAMETERS_HPP_ */
