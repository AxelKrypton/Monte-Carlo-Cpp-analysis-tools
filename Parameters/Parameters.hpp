#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "iostream"

class Parameters
{
public:
	Parameters(int argc, char ** argv);

	struct parse_aborted {};
	int binsize;
	int numberOfBins;
	bool useBinning;
	bool calcAutocorrelation;
	int offset;
	std::string file;
	bool analyseMean;
	bool analyseVariance;
	bool analyseSkewness;
	bool analyseKurtosis;

private:
	void printParameters();

	std::string defaultFile;
};

#endif /* PARAMETERS_HPP_ */
