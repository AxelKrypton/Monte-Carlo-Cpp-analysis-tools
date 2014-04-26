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
	bool calcAutocorrelation;
	bool analyseMean;
	bool analyseVariance;
	bool analyseSkewness;
	bool analyseKurtosis;

private:
	void printParameters();

	std::string defaultFile;
};

#endif /* PARAMETERS_HPP_ */
