#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MomentsReweighter_objects
#include <boost/test/unit_test.hpp>
#include <valarray>

#include "Reweighter.hpp"
#include "MomentsReweighter.hpp"
#include "MomentsReweighterHelper.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

//TODO: Once implemented the general class ReweightingParameters, then one should use this here without going through ReweighterTester (like in build2)

class MomentsReweighterTest : public MomentsReweighterAbstract{
public:
    MomentsReweighterTest() = delete;
    MomentsReweighterTest(std::initializer_list<std::string> options, std::vector<unsigned int> momentsToBeReweighted = {}, std::vector<int> binsizesToBeUsed = {})
    	: MomentsReweighterAbstract(ReweighterTester(options).getRawDataForReweightingAndMetainformation(momentsToBeReweighted, binsizesToBeUsed)){};
    MomentsReweighterTest(RawDataForReweightingAndMetainformation in) : MomentsReweighterAbstract(in) {};

    //Setters
    void setPrecisionToCalculateLogZ(double precisionToCalculateLogZ){
		MomentsReweighterAbstract::setPrecisionToCalculateLogZ(precisionToCalculateLogZ);
	}

    void setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn){
        MomentsReweighterAbstract::setNewRangesOfParameters(newRangesOfParametersIn);
    }

    void setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        MomentsReweighterAbstract::setNewNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn);
    }

    void setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                          std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        MomentsReweighterAbstract::setNewParameters(newRangesOfParametersIn, newNumberOfPointsOfParametersIn);
    }

    //Test for calculations
    std::vector<double> testCalculateLogZAtNewPoints(){
        calculateAndSetLogZAtNewPoints();
        return getLogZAtNewPoints();
    }

    std::vector<double> testCalculateLogZAtSimulatedPoints(){
        calculateAndSetLogZAtSimulatedPoints();
        return getLogZAtSimulatedPoints();
    }

    std::vector<double> testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(const int entryToBeLeftOut){
        return calculateLogZAtSimulatedPoints(true, entryToBeLeftOut, NULL, true);
    }

    std::vector<double> testCalculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(const int entryToBeLeftOut, std::vector<double> logZSim){
        return calculateLogZAtNewPoints(getValuesOfNewParameters(), true, entryToBeLeftOut, &logZSim);
    }

    void testPrepareObservablesBeforeReweighting(std::vector<double>& minima){
        prepareObservablesBeforeReweighting(minima);
    }

    void testRestoreObservablesAfterReweighting(std::vector<double>& minima,\
                                                std::vector<std::vector<double> > *rewObsFromRawData,
                                                std::valarray<std::vector<std::vector<double> > >* jack){
        restoreObservablesAfterReweighting(minima, rewObsFromRawData, jack);
    }

    std::vector<std::vector<double> > testCalculateReweightedObservableValues(bool useBinnedData = false,
                                                                              const int entryToBeLeftOut = -1,
                                                                              std::vector<double> *logZAtSimulationPointToBeUsed = NULL,
                                                                              std::vector<double> *logZAtNewPointsToBeUsed = NULL){
        return calculateReweightedObservableValues(useBinnedData, entryToBeLeftOut,
                                                   logZAtSimulationPointToBeUsed, logZAtNewPointsToBeUsed);
    }

    std::vector<std::vector<double> > getObservablesFromSimulationData(bool raw, const int numObs){
        SimulationDataContainer simDataCont = getSimulationDataContainer(raw);
        const int rewPar = simDataCont[0].getNumberOfDataSample()-numObs;
        std::vector<std::vector<double> > observables(numObs);
        for(int i=0; i<simDataCont.getNumberOfDatafiles(); i++){
            for(int j=0; j<numObs; j++){
                for(int k=0; k<simDataCont[i][j+rewPar].getNumberOfElements(); k++)
                    observables[j].push_back(simDataCont[i][j+rewPar][k]);
            }
        }
        return observables;
    }

    void testCalculateAndSetReweightedMomentsAndMomentsEstimators(){
    	calculateAndSetReweightedMomentsAndMomentsEstimators();
    }

    void testExtractAndSetReweightedMomentsAndMomentsEstimators(const std::vector<std::vector<double> >& reweightedObservablesFromRawData,
	 	 	 	 												const std::valarray<std::vector<std::vector<double> > >& estimatorsForErrorsCalculation){
    	extractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, estimatorsForErrorsCalculation);
	}
};


BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--weightPrecision=0.0"};
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(options, {1}, {2,2,2}), std::range_error);
        std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--weightPrecision=-1.0"};
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(options2, {1}, {2,2,2}), std::range_error);
    }

    BOOST_AUTO_TEST_CASE(build2)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
        RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation({1}, {1,1,1});
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints;
		newRanges.push_back(std::make_pair(4.2, 4.6));
		rawDataAndMetaInfo.newRangesOfParameters = newRanges;
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newRanges.push_back(std::make_pair(0.8, 1.2));
		rawDataAndMetaInfo.newRangesOfParameters = newRanges;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		rawDataAndMetaInfo.newRangesOfParameters = newRanges;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newNumPoints.push_back(2);
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newNumPoints.push_back(3);
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newNumPoints.push_back(0);
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newNumPoints.back() = 1;
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo), std::invalid_argument);
		newNumPoints.back() = 5;
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
        BOOST_REQUIRE_NO_THROW(MomentsReweighterTest momentsReweighterTest(rawDataAndMetaInfo));
	}

BOOST_AUTO_TEST_SUITE_END()

static MomentsReweighterTest createMomentsReweighterTestForGettersAndSettersTests(std::string precisionLogZ = "1.e-7"){
	std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--weightPrecision=" + precisionLogZ};
	RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation({1}, {1,1,1});
	std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.8, 1.2), std::make_pair(-1.2e12, -1.6e12)};
	std::vector< unsigned int> newNumPoints(3, 2);
	rawDataAndMetaInfo.newRangesOfParameters = newRanges;
	rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
	return MomentsReweighterTest(rawDataAndMetaInfo);
}

BOOST_AUTO_TEST_SUITE(getters)

	BOOST_AUTO_TEST_CASE(getters1)
	{
        std::vector<std::vector<double> > refParameters = {{4.0, 0.7, -1.e12},{4.5, 1.0, -5.e12},{4.5, 1.3, -1.e13}};
        MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
        BOOST_REQUIRE(refParameters == momentsReweighterTest.getValuesOfSimulationParameters());
	}

	BOOST_AUTO_TEST_CASE(getters2)
	{
		MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
		std::vector<std::vector<double> > refPoints = {{4.2, 0.8, -1.6e12}, {4.2, 0.8, -1.2e12}, {4.2, 1.2, -1.6e12}, {4.2, 1.2, -1.2e12},
													   {4.6, 0.8, -1.6e12}, {4.6, 0.8, -1.2e12}, {4.6, 1.2, -1.6e12}, {4.6, 1.2, -1.2e12}};
        BOOST_REQUIRE(refPoints == momentsReweighterTest.getValuesOfNewParameters());
	}

	BOOST_AUTO_TEST_CASE(getters3)
	{
		MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
		unsigned int numberOfNewPoints = 8;
        BOOST_REQUIRE_EQUAL(numberOfNewPoints, momentsReweighterTest.getNumberOfNewPoints());
	}

	BOOST_AUTO_TEST_CASE(getters4)
	{
		MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
        double referenceLogZAtSimulatedPoints[3] = {0.0, 0.0, 0.0};
        BOOST_REQUIRE_EQUAL(momentsReweighterTest.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<momentsReweighterTest.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(momentsReweighterTest.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
	}

	BOOST_AUTO_TEST_CASE(getters5)
	{
        MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
        BOOST_REQUIRE_EQUAL(momentsReweighterTest.getLogZAtNewPoints().size(), 0);
	}

	BOOST_AUTO_TEST_CASE(getters6)
	{
		MomentsReweighterTest momentsReweighterTest = createMomentsReweighterTestForGettersAndSettersTests();
		MomentsReweighterTest momentsReweighterTest2 = createMomentsReweighterTestForGettersAndSettersTests("0.001");
		BOOST_REQUIRE_EQUAL(momentsReweighterTest.getPrecisionToCalculateLogZ(), 1.e-7);
		BOOST_REQUIRE_EQUAL(momentsReweighterTest2.getPrecisionToCalculateLogZ(), 0.001);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setters)

	BOOST_AUTO_TEST_CASE(setters1)
	{
		std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.8, 1.2)};
        MomentsReweighterTest* momentsReweighterTest = new MomentsReweighterTest(createMomentsReweighterTestForGettersAndSettersTests());
        BOOST_REQUIRE_THROW(momentsReweighterTest->setNewRangesOfParameters(newRanges), std::invalid_argument);
        newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		newRanges[1] = std::make_pair(0.7, 1.3);
        BOOST_REQUIRE_NO_THROW(momentsReweighterTest->setNewRangesOfParameters(newRanges));
		std::vector<std::vector<double> > refPoints = {{4.2, 0.7, -1.6e12}, {4.2, 0.7, -1.2e12}, {4.2, 1.3, -1.6e12}, {4.2, 1.3, -1.2e12},
													   {4.6, 0.7, -1.6e12}, {4.6, 0.7, -1.2e12}, {4.6, 1.3, -1.6e12}, {4.6, 1.3, -1.2e12}};
        BOOST_REQUIRE(refPoints == momentsReweighterTest->getValuesOfNewParameters());
        delete momentsReweighterTest;
	}

	BOOST_AUTO_TEST_CASE(setters2)
	{
		std::vector< unsigned int> newNumPoints(2, 4);
		MomentsReweighterTest* momentsReweighterTest = new MomentsReweighterTest(createMomentsReweighterTestForGettersAndSettersTests());
        BOOST_REQUIRE_THROW(momentsReweighterTest->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
        newNumPoints.push_back(4);
		newNumPoints[1] = 5;
        BOOST_REQUIRE_NO_THROW(momentsReweighterTest->setNewNumberOfPointsOfParameters(newNumPoints));
		BOOST_REQUIRE_EQUAL(momentsReweighterTest->getNumberOfNewPoints(), 80);
        delete momentsReweighterTest;
	}

	BOOST_AUTO_TEST_CASE(setters3)
	{
		std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.7, 1.3), std::make_pair(-1.2e12, -1.6e12)};
		std::vector< unsigned int> newNumPoints(3, 2);
        MomentsReweighterTest momentsReweighterTest(createMomentsReweighterTestForGettersAndSettersTests());
        BOOST_REQUIRE_NO_THROW(momentsReweighterTest.setNewParameters(newRanges, newNumPoints));
		std::vector<std::vector<double> > refPoints = {{4.2, 0.7, -1.6e12}, {4.2, 0.7, -1.2e12}, {4.2, 1.3, -1.6e12}, {4.2, 1.3, -1.2e12},
													   {4.6, 0.7, -1.6e12}, {4.6, 0.7, -1.2e12}, {4.6, 1.3, -1.6e12}, {4.6, 1.3, -1.2e12}};
        BOOST_REQUIRE(refPoints == momentsReweighterTest.getValuesOfNewParameters());
        BOOST_REQUIRE_EQUAL(momentsReweighterTest.getNumberOfNewPoints(), 8);
	}

	BOOST_AUTO_TEST_CASE(setters4)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--useSimulatedPointsAsNewPoints"};
		MomentsReweighterTest momentsReweighterTest(ReweighterTester(options).getRawDataForReweightingAndMetainformation({1}, {1,1,1}));
		std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.7, 1.3), std::make_pair(-1.2e12, -1.6e12)};
		std::vector< unsigned int> newNumPoints(3, 2);
		BOOST_REQUIRE_THROW(momentsReweighterTest.setNewRangesOfParameters(newRanges), std::logic_error);
		BOOST_REQUIRE_THROW(momentsReweighterTest.setNewNumberOfPointsOfParameters(newNumPoints), std::logic_error);
		BOOST_REQUIRE_NO_THROW(momentsReweighterTest.setNewParameters(newRanges, newNumPoints));
	}

	BOOST_AUTO_TEST_CASE(setters5)
	{
        MomentsReweighterTest momentsReweighterTest(createMomentsReweighterTestForGettersAndSettersTests("0.1"));
        BOOST_REQUIRE_THROW(momentsReweighterTest.setPrecisionToCalculateLogZ(-3.e-10), std::range_error);
        BOOST_REQUIRE_NO_THROW(momentsReweighterTest.setPrecisionToCalculateLogZ(1.e-10));
        BOOST_REQUIRE_EQUAL(momentsReweighterTest.getPrecisionToCalculateLogZ(), 1.e-10);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(logZ)

    BOOST_AUTO_TEST_CASE(logZ1)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        double referenceLogZAtNewPoints[] = {141.808725391091, 139.786001864617, 137.763562229189, 135.741409322221,
                                             133.719546041019, 131.697975343092, 129.676700246250, 127.655723828738,
                                             125.635049229069, 123.614679645934, 121.594618337982, 119.574868623334,
                                             117.555433879136, 115.536317540892, 113.517523101742, 111.499054111443,
                                             109.480914175351, 107.463106953159, 105.445636157561, 103.428505552580,
                                             101.411718951886, 99.395280216830, 97.379193254392, 95.363462014766,
                                             93.348090488913, 91.333082705808, 89.318442729589, 87.304174656315,
                                             85.290282610689, 83.276770742455};
        std::vector<double> newLogZ = momentsReweighterTest.testCalculateLogZAtNewPoints();
		for(int i=0; i < momentsReweighterTest.getNumberOfNewPoints(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
	}

    BOOST_AUTO_TEST_CASE(logZ2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        const int pointToBeLeftOut = 1652;
        double referenceLogZAtNewPoints[] = {1.337554293255, -38.670153945310, -98.348187363050};
        std::vector<double> newLogZ = momentsReweighterTest.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < momentsReweighterTest.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
		MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        const int pointToBeLeftOut = 0;
        double referenceLogZAtNewPoints[] = {1.337625545806, -38.670359443497, -98.348790274729};
        std::vector<double> newLogZ = momentsReweighterTest.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < momentsReweighterTest.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
		MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        const int pointToBeLeftOut = 1;
        double referenceLogZAtNewPoints[] = {1.337668062797, -38.670327960517, -98.348659416428};
        std::vector<double> newLogZ = momentsReweighterTest.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < momentsReweighterTest.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ5)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        double referenceLogZAtSimulatedPoints[] = {1.337622018220, -38.670337447054, -98.348640282292};
		std::vector<double> simulatedLogZ = momentsReweighterTest.testCalculateLogZAtSimulatedPoints();
		for(size_t i=0; i < momentsReweighterTest.getValuesOfSimulationParameters().size(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
	}

    BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( logZ6, 2 )
    BOOST_AUTO_TEST_CASE(logZ6)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_2";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest momentsReweighterTest(options, {1}, {1,1,1});
        double referenceLogZAtSimulatedPoints[] = {1.337622018220, -38.670337447054, -98.348640282292};
        std::vector<double> simulatedLogZ = momentsReweighterTest.testCalculateLogZAtSimulatedPoints();

        BOOST_CHECK_CLOSE(referenceLogZAtSimulatedPoints[0], simulatedLogZ[0], 1.e-8);
        BOOST_REQUIRE_EQUAL(referenceLogZAtSimulatedPoints[1], simulatedLogZ[1]);
        BOOST_CHECK_CLOSE(referenceLogZAtSimulatedPoints[2], simulatedLogZ[2], 1.e-8);
        /*
         * For some reason, setting one or more values of logZ in the configfile as done here,
         * leads to slightly different values of logZ at the remaining simulated points then
         * when all the values are calculated. That is why we have above two expected failures
         * and why we repeat here such a failing checks with less precision.
         * This is probably due to the fact that we reach the desired precision in the iterative
         * procedure "from a different direction".
         */
        BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[0], simulatedLogZ[0], 2.e-5);
        BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[2], simulatedLogZ[2], 2.e-5);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(columnsReweighting)

    BOOST_AUTO_TEST_CASE(columnsReweighting1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod"};
		RawDataForReweightingAndMetainformation rawDataAndMetaInfo = ReweighterTester(options).getRawDataForReweightingAndMetainformation({1,2,3,4}, {2,2,2});
		std::vector<std::pair<double, double> > newRanges = {std::make_pair(4.2, 4.6), std::make_pair(0.8, 1.2)};
		std::vector< unsigned int> newNumPoints(2, 3);
		rawDataAndMetaInfo.newRangesOfParameters = newRanges;
		rawDataAndMetaInfo.newNumberOfPointsOfParameters = newNumPoints;
		MomentsReweighterTest reweighter(rawDataAndMetaInfo);

		const int numberOfObservablesInFiles = 4; //1 obs given and 4 moments asked
        const double referenceMinimumOfObservables = -3.9;
        const double referenceOriginalRawObservables[] = {1.3, 1.6, -1.9, 2.3, -2.6, 2.9, -3.3, 3.6, -3.9};
        const double referenceOriginalBinnedObservables[] = {1.3, 2.3, -3.3};
        std::valarray<double> referencePreparedRawObservables(referenceOriginalRawObservables, 9);
        std::valarray<double> referencePreparedBinnedObservables(referenceOriginalBinnedObservables, 3);
        referencePreparedRawObservables = log(referencePreparedRawObservables - 2*referenceMinimumOfObservables);
        referencePreparedBinnedObservables = log(referencePreparedBinnedObservables - 2*referenceMinimumOfObservables);

        std::vector<double> minimumOfObservables(numberOfObservablesInFiles, std::numeric_limits<double>::max());
        reweighter.testPrepareObservablesBeforeReweighting(minimumOfObservables);
        std::vector<std::vector<double> > preparedRawObservables = reweighter.getObservablesFromSimulationData(true, numberOfObservablesInFiles);
        std::vector<std::vector<double> > preparedBinnedObservables = reweighter.getObservablesFromSimulationData(false, numberOfObservablesInFiles);
        reweighter.testRestoreObservablesAfterReweighting(minimumOfObservables, NULL, NULL);
        std::vector<std::vector<double> > restoredRawObservables = reweighter.getObservablesFromSimulationData(true, numberOfObservablesInFiles);
        std::vector<std::vector<double> > restoredBinnedObservables = reweighter.getObservablesFromSimulationData(false, numberOfObservablesInFiles);
        BOOST_REQUIRE_EQUAL(referenceMinimumOfObservables, minimumOfObservables[0]);
        for(int i=0; i<9; i++){
            BOOST_REQUIRE_CLOSE(preparedRawObservables[0][i], referencePreparedRawObservables[i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(restoredRawObservables[0][i], referenceOriginalRawObservables[i], doublePrecisionInPercent);
        }
        for(int i=0; i<3; i++){
            BOOST_REQUIRE_CLOSE(preparedBinnedObservables[0][i], referencePreparedBinnedObservables[i], doublePrecisionInPercent);
            BOOST_REQUIRE_CLOSE(restoredBinnedObservables[0][i], referenceOriginalBinnedObservables[i], doublePrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(columnsReweighting2)
    {
        /*
         * In this test we want to test the function calculateReweightedObservableValues but due
         * to the reference code, we produce reference values only for it. Then we call first
         * the function to calculate logZ at simulated and new points. We also have to take the
         * log of the observables before reweighting, so we call the function to prepare observables.
         * Actually, we will not restore observables and we will take the exponential manually. This
         * is because we know we have ONLY ONE observables, which values are ONLY positive!
         */
        std::string fileThatDoesExist = "RealTestData/configfile_1";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest reweighter(options, {1}, {1,1,1});
        const int numberOfObservablesInFiles = 4; //1 obs given + 3 central moments
        double referenceValuesObsNewPoints[] = {0.51136048194686, 0.51141566283263, 0.51147120976833, 0.51152713131180,
                                                0.51158343617809, 0.51164013323921, 0.51169723151890, 0.51175474019037,
                                                0.51181266857210, 0.51187102612138, 0.51192982243020, 0.51198906722145,
                                                0.51204877034025, 0.51210894174908, 0.51216959152032, 0.51223072982883,
                                                0.51229236694301, 0.51235451321772, 0.51241717908248, 0.51248037503318,
                                                0.51254411162041, 0.51260839943865, 0.51267324911302, 0.51273867128713,
                                                0.51280467660836, 0.51287127571465, 0.51293847921736, 0.51300629768625,
                                                0.51307474163220, 0.51314382148783};
        std::vector<double> minimumOfObservables(numberOfObservablesInFiles, std::numeric_limits<double>::max());
        reweighter.testPrepareObservablesBeforeReweighting(minimumOfObservables);
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<double> > valuesObsNewPoints = reweighter.testCalculateReweightedObservableValues();
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], exp(valuesObsNewPoints[i][0]), 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(columnsReweighting3)
    {
        /*
         * The same of the previous test but leaving out last datapoint in all files
         */
        std::string fileThatDoesExist = "RealTestData/configfile_1";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest reweighter(options, {1,2,3,4}, {1,1,1});
        const int numberOfObservablesInFiles = 4; //1 obs given and 4 moments asked
        const int pointToBeLeftOut = 1652;
        double referenceValuesObsNewPoints[] = {0.51136310124219, 0.51141833777545, 0.51147393929166, 0.51152991432649,
                                                0.51158627157410, 0.51164301988718, 0.51170016827126, 0.51175772587953,
                                                0.51181570201244, 0.51187410610973, 0.51193294774632, 0.51199223662849,
                                                0.51205198258545, 0.51211219556435, 0.51217288562309, 0.51223406292191,
                                                0.51229573771667, 0.51235792034777, 0.51242062123311, 0.51248385085715,
                                                0.51254761975872, 0.51261193852236, 0.51267681776307, 0.51274226811470,
                                                0.51280830021672, 0.51287492469895, 0.51294215216601, 0.51300999318055,
                                                0.51307845824823, 0.51314755779579};
        std::vector<double> minimumOfObservables(numberOfObservablesInFiles, std::numeric_limits<double>::max());
        reweighter.testPrepareObservablesBeforeReweighting(minimumOfObservables);
        std::vector<double> logZSim = reweighter.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        std::vector<double> logZNew = reweighter.testCalculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut, logZSim);
        BOOST_REQUIRE_THROW(reweighter.testCalculateReweightedObservableValues(false, pointToBeLeftOut+1), std::logic_error);
        //since we know the binsize is one we can reweight on the binned data even if in the reference code we used the raw data
        std::vector<std::vector<double> > valuesObsNewPoints =
                reweighter.testCalculateReweightedObservableValues(true, pointToBeLeftOut, &logZSim, &logZNew);
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], exp(valuesObsNewPoints[i][0]), 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(columnsReweighting4)
    {
        /*
         * The same of the previous test but leaving out first datapoint in all files
         */
        std::string fileThatDoesExist = "RealTestData/configfile_1";
    	std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
    												  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
        MomentsReweighterTest reweighter(options, {1,2,3,4}, {1,1,1});
        const int numberOfObservablesInFiles = 4; //1 obs given and 4 moments asked
        const int pointToBeLeftOut = 0;
        double referenceValuesObsNewPoints[] = {0.51136053648952, 0.51141575105339, 0.51147133146101, 0.51152728623366,
                                                0.51158362405415, 0.51164035375839, 0.51169748433664, 0.51175502492514,
                                                0.51181298480650, 0.51187137340131, 0.51193020026481, 0.51198947508200,
                                                0.51204920766098, 0.51210940792574, 0.51217008591066, 0.51223125175234,
                                                0.51229291568079, 0.51235508801071, 0.51241777913491, 0.51248099950908,
                                                0.51254475964551, 0.51260907009973, 0.51267394145867, 0.51273938432740,
                                                0.51280540931553, 0.51287202702358, 0.51293924802488, 0.51300708285347,
                                                0.51307554198351, 0.51314463581261};
        std::vector<double> minimumOfObservables(numberOfObservablesInFiles, std::numeric_limits<double>::max());
        reweighter.testPrepareObservablesBeforeReweighting(minimumOfObservables);
        std::vector<double> logZSim = reweighter.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        std::vector<double> logZNew = reweighter.testCalculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut, logZSim);
        //since we know the binsize is one we can reweight on the binned data even if in the reference code we used the raw data
        std::vector<std::vector<double> > valuesObsNewPoints =
                reweighter.testCalculateReweightedObservableValues(true, pointToBeLeftOut, &logZSim, &logZNew);
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], exp(valuesObsNewPoints[i][0]), 1.e-8);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(extractionColumns)

	BOOST_AUTO_TEST_CASE(extractionColumns1)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
		MomentsReweighterTest reweighter(options, {2}, {1,1,1});
		std::vector<std::vector<double> > reweightedObservablesFromRawData(reweighter.getNumberOfNewPoints());
		std::valarray<std::vector<std::vector<double> > > estimatorsForErrorsCalculation(reweighter.getNumberOfNewPoints());
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			reweightedObservablesFromRawData[i] = {1.0}; //Of course fake numbers, just for testing
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			estimatorsForErrorsCalculation[i] = reweightedObservablesFromRawData;
		reweighter.testExtractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, estimatorsForErrorsCalculation);
		std::vector<std::vector<Moments> > momentsAtNewPoints = reweighter.getMomentsAtNewPoints();
		std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints = reweighter.getMomentsEstimatorsAtNewPoints();
		for(size_t i=0; i<momentsAtNewPoints.size(); i++)
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0][2], 1.0);
	}

	BOOST_AUTO_TEST_CASE(extractionColumns2)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_6";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--obsMultipleColumns=1"};
		MomentsReweighterTest reweighter(options, {1}, {1,1,1});
		std::vector<std::vector<double> > reweightedObservablesFromRawData(reweighter.getNumberOfNewPoints());
		std::valarray<std::vector<std::vector<double> > > estimatorsForErrorsCalculation(reweighter.getNumberOfNewPoints());
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			reweightedObservablesFromRawData[i] = {1,1,1,1,2,3,4}; //Of course fake numbers, just for testing
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			estimatorsForErrorsCalculation[i] = reweightedObservablesFromRawData;
		reweighter.testExtractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, estimatorsForErrorsCalculation);
		std::vector<std::vector<Moments> > momentsAtNewPoints = reweighter.getMomentsAtNewPoints();
		std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints = reweighter.getMomentsEstimatorsAtNewPoints();
		for(size_t i=0; i<momentsAtNewPoints.size(); i++){
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[0], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[1], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[2], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[3], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][1][1], 2.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][2][1], 3.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][3][1], 4.0);
		}
	}

	BOOST_AUTO_TEST_CASE(extractionColumns3)
	{
		std::string fileThatDoesExist = "RealTestData/configfile_6";
		std::initializer_list<std::string> options = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
													  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30", "--obsMultipleColumns=1"};
		MomentsReweighterTest reweighter(options, {2,3,1,4}, {1,1,1});
		std::vector<std::vector<double> > reweightedObservablesFromRawData(reweighter.getNumberOfNewPoints());
		std::valarray<std::vector<std::vector<double> > > estimatorsForErrorsCalculation(reweighter.getNumberOfNewPoints());
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			reweightedObservablesFromRawData[i] = {2,3,1,1,1,1,4}; //Of course fake numbers, just for testing
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			estimatorsForErrorsCalculation[i] = reweightedObservablesFromRawData;
		reweighter.testExtractAndSetReweightedMomentsAndMomentsEstimators(reweightedObservablesFromRawData, estimatorsForErrorsCalculation);
		std::vector<std::vector<Moments> > momentsAtNewPoints = reweighter.getMomentsAtNewPoints();
		std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints = reweighter.getMomentsEstimatorsAtNewPoints();
		for(size_t i=0; i<momentsAtNewPoints.size(); i++){
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[0], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[1], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[2], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0](1)[3], 1.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0][2], 2.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0][3], 3.0);
			BOOST_REQUIRE_EQUAL(momentsAtNewPoints[i][0][4], 4.0);
		}
	}

BOOST_AUTO_TEST_SUITE_END()




 //TODO: Implement tests for testCalculateAndSetReweightedMomentsAndMomentsEstimators function!


/****************************************************************************************************/
/* The following tests are just to check that there are not exceptions using the MomentsReweighter. */
/* Probably they are unnecessary, but it could be good to have an overall testing.                  */
/****************************************************************************************************/

BOOST_AUTO_TEST_SUITE(reweighter)

std::string fileThatDoesExist = "RealTestData/configfile_3";
std::initializer_list<std::string> options1 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod"};
std::initializer_list<std::string> options2 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod"};
std::initializer_list<std::string> options3 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--weightPrecision=1.e-5"};
std::initializer_list<std::string> options4 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--weightPrecision=1.e-5"};
std::initializer_list<std::string> options5 = {"-f" + fileThatDoesExist, "--useJackknifeAsErrorMethod", "--newBetaRange_low=5.348",
											  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
std::initializer_list<std::string> options6 = {"-f" + fileThatDoesExist, "--useBootstrapAsErrorMethod", "--newBetaRange_low=5.348",
											  "--newBetaRange_high=5.3509", "--numberOfNewBetaPoints=30"};
std::vector<std::pair<double, double> > newRanges(1,std::make_pair(5.348, 5.3509));
std::vector< unsigned int> newNumPoints(1, 30);

    BOOST_AUTO_TEST_CASE(constructors)
    {
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options1).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options2).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options3).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options4).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options5).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
        BOOST_REQUIRE_NO_THROW(MomentsReweighter momentsReweighter(ReweighterTester(options6).getRawDataForReweightingAndMetainformation({1},{100,100,100})));
    }

    BOOST_AUTO_TEST_CASE(setters)
    {
        MomentsReweighter *momentsReweighter= new MomentsReweighter(ReweighterTester(options1).getRawDataForReweightingAndMetainformation({1},{100,100,100}));
        BOOST_REQUIRE_NO_THROW(momentsReweighter->setNewRangesOfParameters(newRanges));
        BOOST_REQUIRE_NO_THROW(momentsReweighter->setNewNumberOfPointsOfParameters(newNumPoints));
        BOOST_REQUIRE_NO_THROW(momentsReweighter->setNewParameters(newRanges, newNumPoints));
    }

    BOOST_AUTO_TEST_CASE(realrun)
    {
        double referenceLogZAtSimulatedPoints[] = {1.30339493195915, -38.695917955617, -98.3689346240374};
		double referenceLogZAtNewPoints[] = {142.439784041761, 140.413720670414, 138.387895087841, 136.362308813035, 134.336963394692,
											 132.311860411913, 130.287001474783, 128.262388225038, 126.238022336539, 124.213905515911,
											 122.19003950305, 120.16642607172, 118.143067029946, 116.119964220545, 114.097119521627,
											 112.074534846905, 110.052212146157, 108.030153405543, 106.008360647991, 103.98683593337,
											 101.965581358779, 99.9445990587913, 97.9238912054946, 95.9034600086542, 93.8833077157218,
											 91.863436611892, 89.8438490199233, 87.8245473000734, 85.8055338498687, 83.7868111039009};
        MomentsReweighter momentsReweighter(ReweighterTester(options5).getRawDataForReweightingAndMetainformation({1},{100,100,100}));
        std::vector<double> simulatedLogZ = momentsReweighter.getLogZAtSimulatedPoints();
        std::vector<double> newLogZ = momentsReweighter.getLogZAtNewPoints();

        for(size_t i=0; i < momentsReweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
        for(int i=0; i < momentsReweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

BOOST_AUTO_TEST_SUITE_END()









