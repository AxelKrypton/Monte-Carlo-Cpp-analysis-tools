#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

//todo: remove this again
using namespace std;

//todo: rename to Parameters
class parameters
{
public:
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
	std::string defaultFile;

	parameters(int argc, char ** argv);

private:
	void printParameters();
};

#endif /* PARAMETERS_HPP_ */
