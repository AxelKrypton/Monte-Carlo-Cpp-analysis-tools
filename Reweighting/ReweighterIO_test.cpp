#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "ReweighterIO.hpp"

static LqcdReweightingParameters createLqcdParameters(std::string filename)
{
	int numberOfArguments = 4;
	filename = std::string("-f" + filename);
	const char * arguments[] = {"foo", filename.c_str(), "--useBootstrapAsErrorMethod", "--deactivateReweightingForSkewness"};
	return LqcdReweightingParameters(numberOfArguments, arguments);
}
