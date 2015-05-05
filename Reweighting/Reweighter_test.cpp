#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "Reweighter.hpp"

static void compareReweightingProcedures(ReweightingProcedure& reference, ReweightingProcedure& calculated){
	std::sort(reference.binsizesToBeUsed.begin(), reference.binsizesToBeUsed.end());
	std::sort(calculated.binsizesToBeUsed.begin(), calculated.binsizesToBeUsed.end());
	std::sort(reference.momentsToBeReweighted.begin(), reference.momentsToBeReweighted.end());
	std::sort(calculated.momentsToBeReweighted.begin(), calculated.momentsToBeReweighted.end());
	std::sort(reference.quantitiesConsidered.begin(), reference.quantitiesConsidered.end());
	std::sort(calculated.quantitiesConsidered.begin(), calculated.quantitiesConsidered.end());
	for(size_t i=0; i<reference.binsizesToBeUsed.size(); i++)
		BOOST_REQUIRE_EQUAL(reference.binsizesToBeUsed.at(i), calculated.binsizesToBeUsed.at(i));
	for(size_t i=0; i<reference.momentsToBeReweighted.size(); i++)
		BOOST_REQUIRE_EQUAL(reference.momentsToBeReweighted.at(i), calculated.momentsToBeReweighted.at(i));
	for(size_t i=0; i<reference.quantitiesConsidered.size(); i++)
		BOOST_CHECK(reference.quantitiesConsidered.at(i) == calculated.quantitiesConsidered.at(i));
}

class ReweighterTester{
public:
	ReweighterTester(std::initializer_list<std::string> options) : reweighter(options){};
	std::vector<ReweightingProcedure> getReweightingProceduresToBePerformed(){ return reweighter.getReweightingProceduresToBePerformed(); };
private:
	Reweighter reweighter;
};



BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_1", "--useBootstrapAsErrorMethod", "--deactivateReweightingForSkewness"};
		BOOST_REQUIRE_NO_THROW(ReweighterTester reweighterTester(options));
	}

//	BOOST_AUTO_TEST_CASE(build2)
//	{
//		std::string fileThatDoesExist = "RealTestData/configfile_3";
//		ReweighterIOTester reweighterIOTester(fileThatDoesExist, std::vector<unsigned int>(), "jack");
//		const int referenceNumberOfBins = 16;
//		BOOST_REQUIRE_EQUAL(reweighterIOTester.getNumberOfBinsToBeUsed()[0], referenceNumberOfBins);
//	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(privateFunctions)

	BOOST_AUTO_TEST_CASE(getRewProcedures1)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures2)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForVariance",
				                                      "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1}, {150, 100, 100}, {Mean::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures3)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
				                                      "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1, 2}, {150, 200, 100}, {Variance::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures4)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForBinder"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1, 2, 3}, {280, 200, 175}, {Skewness::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures5)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1, 2, 3, 4}, {280, 200, 175}, {BinderCumulant::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures6)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1, 2, 3, 4}, {280, 200, 175}, {Skewness::observableName, BinderCumulant::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures7)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		//Here the order is alphabetic in observables, since we use std::map
		reference.push_back({{1, 2, 3, 4}, {280, 200, 175}, {Skewness::observableName, BinderCumulant::observableName}});
		reference.push_back({{1, 2}, {150, 200, 100}, {Variance::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures8)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		//Here the order is alphabetic in observables, since we use std::map
		reference.push_back({{1, 2, 3, 4}, {280, 200, 175}, {Skewness::observableName, BinderCumulant::observableName}});
		reference.push_back({{1}, {150, 100, 100}, {Mean::observableName}});
		reference.push_back({{1, 2}, {150, 200, 100}, {Variance::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures9)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_6", "--useBootstrapAsErrorMethod"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{1, 2, 3, 4}, {100, 100, 100}, {Mean::observableName, Variance::observableName, Skewness::observableName, BinderCumulant::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures10)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures11)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{2}, {100, 200, 100}, {Variance::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures12)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForBinder", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{2, 3}, {280, 200, 175}, {Skewness::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures13)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{2, 4}, {100, 200, 160}, {BinderCumulant::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures14)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_7", "--useBootstrapAsErrorMethod", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{2, 4}, {100, 200, 160}, {BinderCumulant::observableName}});
		reference.push_back({{2, 3}, {280, 200, 175}, {Skewness::observableName}});
		reference.push_back({{2}, {100, 200, 100}, {Variance::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

	BOOST_AUTO_TEST_CASE(getRewProcedures15)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_6", "--useBootstrapAsErrorMethod", "--isMeanKnownToBeZero"};
		ReweighterTester reweighterTester(options);
		std::vector<ReweightingProcedure> reference;
		reference.push_back({{2, 3, 4}, {100, 100, 100}, {Variance::observableName, Skewness::observableName, BinderCumulant::observableName}});
		std::vector<ReweightingProcedure> gotten = reweighterTester.getReweightingProceduresToBePerformed();
		BOOST_REQUIRE_EQUAL(reference.size(), gotten.size());
		for(size_t i=0; i<reference.size(); i++)
			compareReweightingProcedures(reference[i], gotten[i]);
	}

BOOST_AUTO_TEST_SUITE_END()





