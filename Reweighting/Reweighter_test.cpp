#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "Reweighter.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

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


BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		std::initializer_list<std::string> options = {"-f./RealTestData/configfile_1", "--useBootstrapAsErrorMethod", "--deactivateReweightingForSkewness"};
		BOOST_REQUIRE_NO_THROW(ReweighterTester reweighterTester(options));
	}

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


BOOST_AUTO_TEST_SUITE(meanReweighting)

    BOOST_AUTO_TEST_CASE(meanReweighting1)
    {
        /*
         * In this test we start to test the function calculateAndGetReweightedObservables.
         * The first trivial test is to compare the value of the mean regardless to the
         * error. This has basically already been tested in "observables2" but here we call
         * a different function in which the preparation and the restoring of the observables is done.
         * Again we have to calculate the logZ manually since the Tester class doesn't do that in the
         * constructor/setters.
         */
        std::string fileThatDoesExist = "RealTestData/configfile_4";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
        ReweighterTester reweighter(options, true);
        double referenceValuesObs1NewPoints[] = {0.513201682060, 0.513258510947, 0.513316165254, 0.513374651344, 0.513433975214, 0.513494142470,
        										 0.513555158307, 0.513617027480, 0.513679754278, 0.513743342503, 0.513807795437, 0.513873115823,
        										 0.513939305834, 0.514006367050, 0.514074300428, 0.514143106283, 0.514212784255, 0.514283333291,
        										 0.514354751616, 0.514427036709, 0.514500185286, 0.514574193271, 0.514649055778, 0.514724767094,
        										 0.514801320653, 0.514878709028, 0.514956923909, 0.515035956091, 0.515115795462, 0.515196430993};

        double referenceValuesObs2NewPoints[] = {0.512565539027, 0.512620638254, 0.512676556317, 0.512733299285, 0.512790872858, 0.512849282342,
        										 0.512908532626, 0.512968628155, 0.513029572905, 0.513091370361, 0.513154023485, 0.513217534696,
        										 0.513281905844, 0.513347138179, 0.513413232332, 0.513480188285, 0.513548005351, 0.513616682143,
        										 0.513686216557, 0.513756605745, 0.513827846094, 0.513899933203, 0.513972861865, 0.514046626048,
        										 0.514121218873, 0.514196632603, 0.514272858625, 0.514349887439, 0.514427708644, 0.514506310930};
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
        for(size_t i=0; i < valuesObsNewPoints.size(); i++){
            BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 1.e-8);
            BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].mean.estimate, 1.e-8);
        }
    }

    BOOST_AUTO_TEST_CASE(meanReweighting2)
    {
        /*
         * Here we compare our estimate of the observables with error with the same of the Reference Code.
         * Since in the Ref. Code they are estimated using bootstrap, we cannot really compare the error.
         * Infact, if you use binsize=1 in the bootstrap as it is done here, then you get zero as error
         * everywhere (or something in 1.e-7 or 1.e-8) because the bootstrap estimators are always the same!
         */
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
        ReweighterTester reweighter(options, true);
        double referenceValuesObsNewPoints[] = {0.511622217173, 0.511770790824, 0.511922922294, 0.512078816370, 0.512238695862, 0.512402801577,
        										0.512571391956, 0.512744742291, 0.512923143444, 0.513106900003, 0.513296327766, 0.513491750494,
        										0.513693495837, 0.513901890370, 0.514117253685, 0.514339891515, 0.514570087889, 0.514808096378,
        										0.515054130536, 0.515308353702, 0.515570868411, 0.515841705718, 0.516120814849, 0.516408053606,
        										0.516703180054, 0.517005846018, 0.517315592906, 0.517631850357, 0.517953938106, 0.518281071314,
        										0.518612369493, 0.518946868901, 0.519283538110, 0.519621296244, 0.519959033190, 0.520295630948,
        										0.520629985226, 0.520961026348, 0.521287738610, 0.521609177330, 0.521924482996, 0.522232892118,
        										0.522533744605, 0.522826487667, 0.523110676440, 0.523385971675, 0.523652134929, 0.523909021753,
        										0.524156573418, 0.524394807658, 0.524623808921};
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
        for(size_t i=0; i < valuesObsNewPoints.size(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(meanReweighting3)
	{
		/*
		 * Here we compare our estimate of the observables with error with the same of the Reference Code.
		 * Now that we implemented also bootstrap, also the error has to match!
		 * In this particular test we use 1 as binsize and this forces the bootstrap to choose always the
		 * same value! Thus the error must be zero! Zero actually up to machine precision. This means that
		 * in the bootstrap error calculation the first moment square should be equal to the second moment,
		 * but the difference could be not zero but around 1.e-15 and then the square root returns something
		 * of the order of 1.e-7 or 1.e-8.
		 */
		std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
        ReweighterTester reweighter(options, true);
		double referenceValuesObsNewPoints[] = {0.511622217173, 0.511770790824, 0.511922922294, 0.512078816370, 0.512238695862, 0.512402801577,
												0.512571391956, 0.512744742291, 0.512923143444, 0.513106900003, 0.513296327766, 0.513491750494,
												0.513693495837, 0.513901890370, 0.514117253685, 0.514339891515, 0.514570087889, 0.514808096378,
												0.515054130536, 0.515308353702, 0.515570868411, 0.515841705718, 0.516120814849, 0.516408053606,
												0.516703180054, 0.517005846018, 0.517315592906, 0.517631850357, 0.517953938106, 0.518281071314,
												0.518612369493, 0.518946868901, 0.519283538110, 0.519621296244, 0.519959033190, 0.520295630948,
												0.520629985226, 0.520961026348, 0.521287738610, 0.521609177330, 0.521924482996, 0.522232892118,
												0.522533744605, 0.522826487667, 0.523110676440, 0.523385971675, 0.523652134929, 0.523909021753,
												0.524156573418, 0.524394807658, 0.524623808921};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
			BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 1.e-8);
			BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].mean.error, 1.e-6);
		}
	}

/*
 * Now that we implemented also bootstrap, we could naively think that in a test against the
 * reference code also the error has to match! Actually the error relies on random numbers and
 * this cannot guarantee a good test (and should not be used in tests). One should develop a test
 * in which this randomness is ruled out and this could be done developing new code or accumulating
 * statistics in a long test (this second way should be also be avoided). For the moment it is enough
 * the above test that relies on an analytic result.
 */

    BOOST_AUTO_TEST_CASE(meanReweighting4)
    {
        /*
         * A final test for the observables is to repeat the reweighting in the same range, but different
         * number of points. if the same new point is touched in the reweighting, then we can check that
         * the observables values and errors are the same there.
         */
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													   "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=11", "--deactivateReweightingForVariance",
													   "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
        ReweighterTester reweighter(options1, true);
        std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.getReweightedObservables();
        std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=21", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
        reweighter = ReweighterTester(options2, true);
        std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.getReweightedObservables();

        for(size_t i=0; i<valuesObsNewPoints1.size(); i++){
        		BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.estimate,
        							valuesObsNewPoints2[2*i][0].mean.estimate, doublePrecisionInPercent);
				BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.error,
									valuesObsNewPoints2[2*i][0].mean.error, doublePrecisionInPercent);
        }
    }

    /*
     * Trivial test where mean is set by hand to zero
     */
    BOOST_AUTO_TEST_CASE(meanReweighting5)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForVariance",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder", "--isMeanKnownToBeZero"};
		ReweighterTester reweighter(options, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
			BOOST_REQUIRE_EQUAL(valuesObsNewPoints[i][0].mean.estimate, 0.0);
			BOOST_REQUIRE_EQUAL(valuesObsNewPoints[i][0].mean.error, 0.0);
		}
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(suscReweighting)

    /*
     * Same tests as the suite meanReweighting, but on the susceptibility. Read the comments
     * of that suite for more information (e.g. for the precision used here in the following cases).
     */
	BOOST_AUTO_TEST_CASE(suscReweighting1)
    {

       std::string fileThatDoesExist = "RealTestData/configfile_4";
       std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
												     "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--deactivateReweightingForMean",
													 "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
       ReweighterTester reweighter(options, true);
       double referenceValuesObs1NewPoints[] = {1.360382497662e-05, 1.380206166679e-05, 1.400187465334e-05, 1.420317840867e-05, 1.440588225665e-05,
    		   	   	   	   	   	   	   	   	    1.460988958896e-05, 1.481509729023e-05, 1.502139672521e-05, 1.522867261927e-05, 1.543680369204e-05,
    		   	   	   	   	   	   	   	   	    1.564566235018e-05, 1.585511466440e-05, 1.606502048768e-05, 1.627523344711e-05, 1.648560084400e-05,
    		   	   	   	   	   	   	   	   	    1.669596383169e-05, 1.690615752914e-05, 1.711601110549e-05, 1.732534802779e-05, 1.753398594168e-05,
    		   	   	   	   	   	   	   	   	    1.774173728597e-05, 1.794840926933e-05, 1.815380404546e-05, 1.835771925196e-05, 1.855994818922e-05,
    		   	   	   	   	   	   	   	   	    1.876028030456e-05, 1.895850142108e-05, 1.915439410266e-05, 1.934773840867e-05, 1.953831225004e-05};

       double referenceValuesObs2NewPoints[] = {1.427912489987e-05, 1.447082186090e-05, 1.466399284294e-05, 1.485855167371e-05, 1.505440624228e-05,
    		   	   	   	   	   	   	   	   	    1.525145868059e-05, 1.544960513528e-05, 1.564873585130e-05, 1.584873494864e-05, 1.604948062173e-05,
    		   	   	   	   	   	   	   	   	    1.625084475421e-05, 1.645269338274e-05, 1.665488615382e-05, 1.685727680922e-05, 1.705971313576e-05,
    		   	   	   	   	   	   	   	   	    1.726203684049e-05, 1.746408385900e-05, 1.766568464595e-05, 1.786666369691e-05, 1.806684052041e-05,
    		   	   	   	   	   	   	   	   	    1.826602939597e-05, 1.846403963840e-05, 1.866067613976e-05, 1.885573915039e-05, 1.904902525324e-05,
    		   	   	   	   	   	   	   	   	    1.924032734962e-05, 1.942943513494e-05, 1.961613529244e-05, 1.980021250880e-05, 1.998144945525e-05};
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
       for(size_t i=0; i < valuesObsNewPoints.size(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
           BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].susceptibility.estimate, 2e-7);
       }
   }

	BOOST_AUTO_TEST_CASE(suscReweighting2)
	{
	   /*
	    * See comment in meanReweighting2 test.
	    */
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													 "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForMean",
													 "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighter(options, true);
		double referenceValuesObsNewPoints[] = {1.180411821738e-05, 1.208232047961e-05, 1.237626373973e-05, 1.268738973464e-05, 1.301714973234e-05,
											   1.336698100046e-05, 1.373827541457e-05, 1.413234305597e-05, 1.455036960651e-05, 1.499336639338e-05,
											   1.546211444214e-05, 1.595710172614e-05, 1.647845354596e-05, 1.702585886609e-05, 1.759849318694e-05,
											   1.819493996719e-05, 1.881311462187e-05, 1.945019478350e-05, 2.010256123064e-05, 2.076575570353e-05,
											   2.143446069708e-05, 2.210250796952e-05, 2.276292125730e-05, 2.340799772488e-05, 2.402943174945e-05,
											   2.461848171409e-05, 2.516617797466e-05, 2.566356670136e-05, 2.610198085368e-05, 2.647332626765e-05,
											   2.677036803039e-05, 2.698700072329e-05, 2.711848567277e-05, 2.716163955918e-05, 2.711496103230e-05,
											   2.697868722473e-05, 2.675477541869e-05, 2.644681210945e-05, 2.605985582160e-05, 2.560022551895e-05,
											   2.507524896922e-05, 2.449298741691e-05, 2.386195408399e-05, 2.319084028335e-05, 2.248826432305e-05,
											   2.176255198727e-05, 2.102155488620e-05, 2.027251168252e-05, 1.952194950905e-05, 1.877562633973e-05,
											   1.803850752757e-05};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++)
		   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
	}

	BOOST_AUTO_TEST_CASE(suscReweighting3)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													 "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForMean",
													 "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighter(options, true);
		double referenceValuesObsNewPoints[] = {1.180411821738e-05, 1.208232047961e-05, 1.237626373973e-05, 1.268738973464e-05, 1.301714973234e-05,
											   1.336698100046e-05, 1.373827541457e-05, 1.413234305597e-05, 1.455036960651e-05, 1.499336639338e-05,
											   1.546211444214e-05, 1.595710172614e-05, 1.647845354596e-05, 1.702585886609e-05, 1.759849318694e-05,
											   1.819493996719e-05, 1.881311462187e-05, 1.945019478350e-05, 2.010256123064e-05, 2.076575570353e-05,
											   2.143446069708e-05, 2.210250796952e-05, 2.276292125730e-05, 2.340799772488e-05, 2.402943174945e-05,
											   2.461848171409e-05, 2.516617797466e-05, 2.566356670136e-05, 2.610198085368e-05, 2.647332626765e-05,
											   2.677036803039e-05, 2.698700072329e-05, 2.711848567277e-05, 2.716163955918e-05, 2.711496103230e-05,
											   2.697868722473e-05, 2.675477541869e-05, 2.644681210945e-05, 2.605985582160e-05, 2.560022551895e-05,
											   2.507524896922e-05, 2.449298741691e-05, 2.386195408399e-05, 2.319084028335e-05, 2.248826432305e-05,
											   2.176255198727e-05, 2.102155488620e-05, 2.027251168252e-05, 1.952194950905e-05, 1.877562633973e-05,
											   1.803850752757e-05};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
		   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
		   BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].susceptibility.error, 1.e-6);
		}
	}

	BOOST_AUTO_TEST_CASE(suscReweighting4)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													   "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=11", "--deactivateReweightingForMean",
													   "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		ReweighterTester reweighter(options1, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.getReweightedObservables();
		std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=21", "--deactivateReweightingForMean",
													  "--deactivateReweightingForSkewness", "--deactivateReweightingForBinder"};
		reweighter = ReweighterTester(options2, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.getReweightedObservables();
		for(size_t i=0; i<valuesObsNewPoints1.size(); i++){
		   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.estimate,
							   valuesObsNewPoints2[2*i][0].susceptibility.estimate, doublePrecisionInPercent);
		   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.error,
							   valuesObsNewPoints2[2*i][0].susceptibility.error, doublePrecisionInPercent);
		}
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(skewReweighting)

    /*
     * For the skewness we do not have a reference code, then we just auto test our code (third case above)
     */
	BOOST_AUTO_TEST_CASE(skewReweighting1)
    {
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													   "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=11", "--deactivateReweightingForMean",
													   "--deactivateReweightingForVariance", "--deactivateReweightingForBinder"};
		ReweighterTester reweighter(options1, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.getReweightedObservables();
		std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=21", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForBinder"};
		reweighter = ReweighterTester(options2, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.getReweightedObservables();
		for(size_t i=0; i<valuesObsNewPoints1.size(); i++){
					BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.estimate,
										valuesObsNewPoints2[2*i][0].skewness.estimate, doublePrecisionInPercent);
					BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.error,
										valuesObsNewPoints2[2*i][0].skewness.error, doublePrecisionInPercent);
		}
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(bindReweighting)

    /*
     * Same tests as the suite suscReweighting, but on the Binder cumulant. Read the comments
     * of that suite for more information (e.g. for the precision used here in the following cases).
     *
	 * In this test we had to increase manually the precision in some cases because of rounding errors.
	 * Indeed, having the four moments discrepancies below 1.e-12, the binder can be different on the
	 * sixth digit like the following example shows:
	 *
	 *   x1 = 0.512078816369961	x2 = 0.262237401564598	x3 = 0.134299237823898	x4 = 0.0687818157251345	 ->   Binder = 3.74781141215248
	 *   x1 = 0.512078816369975	x2 = 0.26223740156461	x3 = 0.134299237823896	x4 = 0.0687818157251355	 ->   Binder = 3.74790400766947
	 *
	 * where x1,x2,x3,x4 are the four moments and Binder=(x4-4*x3*x1+6*x2*x1*x1-3*x1*x1*x1*x1)/(pow(x2-x1*x1, 2.0)).
	 */
    BOOST_AUTO_TEST_CASE(bindReweighting1)
    {
       std::string fileThatDoesExist = "RealTestData/configfile_4";
       std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
												     "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--deactivateReweightingForMean",
													 "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
       ReweighterTester reweighter(options, true);
       double referenceValuesObs1NewPoints[] = {3.5205774110429, 3.4807737008814, 3.442477210548, 3.4037029932582, 3.3653794540319,
    		   	   	   	   	   	   	   	   	    3.3271694919781, 3.2893191924935, 3.2517669935291, 3.2145794083487, 3.1776753963335,
    		   	   	   	   	   	   	   	   	    3.1412806437041, 3.1050859101799, 3.0692214035074, 3.0339697047747, 2.9992945338063,
    		   	   	   	   	   	   	   	   	    2.964863198033, 2.9309712131526, 2.8974492080148, 2.8647880816396, 2.8323795139447,
    		   	   	   	   	   	   	   	   	    2.8006289747345, 2.7696074074128, 2.7390329606036, 2.7092668179192, 2.6799608951626,
    		   	   	   	   	   	   	   	   	    2.6513990026967, 2.6234188717639, 2.5962574744089, 2.5695977262519, 2.5439835141037};
       double referenceValuesObs2NewPoints[] = {3.4359305019427, 3.4029554561537, 3.3692843338757, 3.3356419804267, 3.3027418555954,
    		   	   	   	   	   	   	   	   	    3.269674983937, 3.2367272416051, 3.2039112085534, 3.1712624586088, 3.1389758519909,
    		   	   	   	   	   	   	   	   	    3.1066851847734, 3.0747419315319, 3.0432158762126, 3.0117309466358, 2.9807535591756,
    		   	   	   	   	   	   	   	   	    2.9499462312277, 2.9195688180622, 2.8897888452079, 2.8602595356053, 2.8311959138636,
    		   	   	   	   	   	   	   	   	    2.8027897318024, 2.7745873579532, 2.7472733252531, 2.7201933236896, 2.6936569294425,
    		   	   	   	   	   	   	   	   	    2.6677822776443, 2.6426871106605, 2.6179747425739, 2.5940106172587, 2.5708020671102};
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
       for(size_t i=0; i < valuesObsNewPoints.size(); i++){
    	   if(i==9 || i==20 || i==24 || i==28){
               BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.003);
               BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 0.003);
    	   }else{
               BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
               if(i==29)
            	   BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 0.0005);
               else
            	   BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 1.e-8);
    	   }
       }
    }

	BOOST_AUTO_TEST_CASE(bindReweighting2)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													 "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForMean",
													 "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
		ReweighterTester reweighter(options, true);
		double referenceValuesObsNewPoints[] = {3.663706836831, 3.696484687413, 3.723885962225, 3.747903662848, 3.765346818340, 3.776222888418,
											   3.779868405965, 3.775221978283, 3.762713121619, 3.740995303963, 3.709608980699, 3.669847657356,
											   3.621044095174, 3.563347667599, 3.497876974793, 3.425192488040, 3.346254664697, 3.262279245763,
											   3.174213891238, 3.083592999229, 2.991712008107, 2.899850309765, 2.809412714627, 2.721682512149,
											   2.637894251316, 2.559181676674, 2.486588832258, 2.421062038709, 2.363377226072, 2.314220158046,
											   2.274161525348, 2.243574635506, 2.222823530362, 2.212114121404, 2.211507215736, 2.220941285527,
											   2.240339735026, 2.269438789375, 2.307890805072, 2.355298699153, 2.411128319662, 2.474664796862,
											   2.545320647525, 2.622395651726, 2.704936325050, 2.792237859045, 2.883234474431, 2.977133174541,
											   3.073140076998, 3.169952202463, 3.267167961059};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
		   if(i==3 || i==22 || i==27 || i==36 || i==41)
			   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.0025);
		   else
			   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
		}

	}

	BOOST_AUTO_TEST_CASE(bindReweighting3)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													 "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=51", "--deactivateReweightingForMean",
													 "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
		ReweighterTester reweighter(options, true);
		double referenceValuesObsNewPoints[] = {3.663706836831, 3.696484687413, 3.723885962225, 3.747903662848, 3.765346818340, 3.776222888418,
											   3.779868405965, 3.775221978283, 3.762713121619, 3.740995303963, 3.709608980699, 3.669847657356,
											   3.621044095174, 3.563347667599, 3.497876974793, 3.425192488040, 3.346254664697, 3.262279245763,
											   3.174213891238, 3.083592999229, 2.991712008107, 2.899850309765, 2.809412714627, 2.721682512149,
											   2.637894251316, 2.559181676674, 2.486588832258, 2.421062038709, 2.363377226072, 2.314220158046,
											   2.274161525348, 2.243574635506, 2.222823530362, 2.212114121404, 2.211507215736, 2.220941285527,
											   2.240339735026, 2.269438789375, 2.307890805072, 2.355298699153, 2.411128319662, 2.474664796862,
											   2.545320647525, 2.622395651726, 2.704936325050, 2.792237859045, 2.883234474431, 2.977133174541,
											   3.073140076998, 3.169952202463, 3.267167961059};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
		   if(i==3 || i==22 || i==27 || i==36 || i==41)
			   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.0025);
		   else
			   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
		   BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].binderCumulant.error, 1.e-6);
		}
	}

	BOOST_AUTO_TEST_CASE(bindReweighting4)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													   "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=11", "--deactivateReweightingForMean",
													   "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
		ReweighterTester reweighter(options1, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.getReweightedObservables();
		std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=21", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness"};
		reweighter = ReweighterTester(options2, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.getReweightedObservables();
		for(size_t i=0; i<valuesObsNewPoints1.size(); i++){
		   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.estimate,
							   valuesObsNewPoints2[2*i][0].binderCumulant.estimate, doublePrecisionInPercent);
		   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.error,
							   valuesObsNewPoints2[2*i][0].binderCumulant.error, doublePrecisionInPercent);
		}
	}

	/*
	* The following test is the same as bindReweighting1
	* but imposing the mean of the observable zero a priori
	*/
	BOOST_AUTO_TEST_CASE(bindReweighting5)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_4";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--deactivateReweightingForMean",
													  "--deactivateReweightingForVariance", "--deactivateReweightingForSkewness", "--isMeanKnownToBeZero"};
		ReweighterTester reweighter(options, true);
		double referenceValuesObs1NewPoints[] = {1.0002080009271, 1.0002109757484, 1.0002139718772, 1.0002169879343, 1.0002200224499,
											   	 1.000223073867, 1.0002261405343, 1.0002292207125, 1.0002323125673, 1.0002354141729,
											     1.0002385235095, 1.0002416384649, 1.0002447568338, 1.0002478763189, 1.0002509945324,
											     1.000254108997, 1.0002572171479, 1.0002603163353, 1.0002634038266, 1.0002664768095,
											     1.0002695323963, 1.0002725676281, 1.0002755794772, 1.0002785648541, 1.0002815206128,
											     1.0002844435548, 1.0002873304381, 1.0002901779811, 1.0002929828719, 1.0002957417751};
		double referenceValuesObs2NewPoints[] = {1.00021870046, 1.0002215815738, 1.0002244825491, 1.0002274019851, 1.0002303383938,
											     1.0002332902004, 1.0002362557408, 1.0002392332605, 1.000242220915, 1.0002452167689,
											     1.0002482187969, 1.0002512248821, 1.000254232818, 1.0002572403095, 1.0002602449731,
											     1.0002632443403, 1.0002662358576, 1.0002692168908, 1.0002721847263, 1.0002751365755,
											     1.0002780695779, 1.0002809808061, 1.0002838672684, 1.0002867259175, 1.0002895536515,
											     1.0002923473237, 1.0002951037463, 1.0002978196993, 1.000300491936, 1.0003031171917};
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.getReweightedObservables();
		for(size_t i=0; i < valuesObsNewPoints.size(); i++){
		  BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
		  BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 1.e-8);
		}
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(quantitiesReweighting)

	/*
	 * This test is a consistency check for the case in which several reweighting procedures are needed
	 */
	BOOST_AUTO_TEST_CASE(bindReweighting4)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_7";
		std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													   "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=11"};
		ReweighterTester reweighter(options1, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.getReweightedObservables();
		std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.363", "--numberOfNewBetaPoints=21"};
		reweighter = ReweighterTester(options2, true);
		std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.getReweightedObservables();

		for(size_t i=0; i<valuesObsNewPoints1.size(); i++){
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.estimate,
							    valuesObsNewPoints2[2*i][0].mean.estimate, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.error,
							    valuesObsNewPoints2[2*i][0].mean.error, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.estimate,
							    valuesObsNewPoints2[2*i][0].susceptibility.estimate, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.error,
							    valuesObsNewPoints2[2*i][0].susceptibility.error, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.estimate,
							    valuesObsNewPoints2[2*i][0].skewness.estimate, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.error,
							    valuesObsNewPoints2[2*i][0].skewness.error, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.estimate,
							    valuesObsNewPoints2[2*i][0].binderCumulant.estimate, doublePrecisionInPercent);
			BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.error,
							    valuesObsNewPoints2[2*i][0].binderCumulant.error, doublePrecisionInPercent);
		}
	}

BOOST_AUTO_TEST_SUITE_END()

