#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "Reweighter.hpp"

static LqcdReweightingParameters createLqcdParameters(std::string filename)
{
	int numberOfArguments = 4;
	filename = std::string("-f" + filename);
	const char * arguments[] = {"foo", filename.c_str(), "--useBootstrapAsErrorMethod", "--deactivateReweightingForSkewness"};
	return LqcdReweightingParameters(numberOfArguments, arguments);
}

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		LqcdReweightingParameters parameters = createLqcdParameters("./RealTestData/configfile_1");
		Reweighter reweighter(parameters);
	}

//	BOOST_AUTO_TEST_CASE(build4)
//	{
//		std::string fileThatDoesExist = "RealTestData/configfile_3";
//		ReweighterIOTester reweighterIOTester(fileThatDoesExist, std::vector<unsigned int>(), "jack");
//		const int referenceNumberOfBins = 16;
//		BOOST_REQUIRE_EQUAL(reweighterIOTester.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins);
//	}



BOOST_AUTO_TEST_SUITE_END()
