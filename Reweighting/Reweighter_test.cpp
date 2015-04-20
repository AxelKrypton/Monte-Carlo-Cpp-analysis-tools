#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter_objects
#include <boost/test/unit_test.hpp>
#include <valarray>

#include "Reweighter.hpp"
#include "ReweightingDataHandler.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePrecisionInPercent

class ReweighterTest : public ReweighterAbstract{
public:
    ReweighterTest() : ReweighterAbstract() {}
    ReweighterTest(std::string configurationFileIn,
                   std::vector<unsigned int> colToBeRewUsingMultipleColumns = std::vector<unsigned int>(),
                   std::vector<unsigned int> colWhoseMeanIsKnownToBeZero = std::vector<unsigned int>(),
                   std::string errorMethodIn = "jackknife", double precisionToCalculateLogZ = 1.e-7)
     : ReweighterAbstract(configurationFileIn, colToBeRewUsingMultipleColumns,
    		              colWhoseMeanIsKnownToBeZero, errorMethodIn, precisionToCalculateLogZ) {}
    ReweighterTest(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                   std::vector<unsigned int>  newNumberOfPointsOfParametersIn,
                   std::vector<unsigned int> colToBeRewUsingMultipleColumns = std::vector<unsigned int>(),
                   std::vector<unsigned int> colWhoseMeanIsKnownToBeZero = std::vector<unsigned int>(),
                   std::string errorMethodIn = "jackknife", double precisionToCalculateLogZ = 1.e-7)
     : ReweighterAbstract(configurationFileIn, newRangesOfParametersIn,
                          newNumberOfPointsOfParametersIn, colToBeRewUsingMultipleColumns,
                          colWhoseMeanIsKnownToBeZero, errorMethodIn, precisionToCalculateLogZ) {}

    //Setters
    void setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn){
        ReweighterAbstract::setNewRangesOfParameters(newRangesOfParametersIn);
    }

    void setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        ReweighterAbstract::setNewNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn);
    }

    void setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                          std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        ReweighterAbstract::setNewParameters(newRangesOfParametersIn, newNumberOfPointsOfParametersIn);
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
        return calculateLogZAtSimulatedPoints(true, entryToBeLeftOut);
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

    std::vector<std::vector<Observables> > testCalculateAndGetReweightedObservables(){
        return calculateAndGetReweightedObservables();
    }
};


BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1)
	{
        BOOST_REQUIRE_THROW(ReweighterTest reweighter, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(), "jack", 0.0), std::range_error);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(), "jack", -1.), std::range_error);
    }

    BOOST_AUTO_TEST_CASE(build3)
    {
    	std::string fileThatDoesExist = "RealTestData/configfile_1";
    	BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(1,2), "boot", 1.e-5), std::invalid_argument);
    	BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(1,0), "jack", 1.e-5), std::invalid_argument);
    	BOOST_REQUIRE_NO_THROW(ReweighterTest reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(1,1), "jack", 1.e-5));
    }


    BOOST_AUTO_TEST_CASE(build4)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints;
		newRanges.push_back(std::make_pair(4.2, 4.6));
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newRanges.push_back(std::make_pair(0.8, 1.2));
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(2);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(3);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(0);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.back() = 1;
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.back() = 5;
        BOOST_REQUIRE_NO_THROW(ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints));
	}

    BOOST_AUTO_TEST_CASE(build5)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        ReweighterTest reweighter(fileThatDoesExist);
        double referenceLogZAtSimulatedPoints[3] = {3.14, 6.28, 9.42};
        BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<reweighter.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
    }

    BOOST_AUTO_TEST_CASE(build6)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        ReweighterTest reweighter(fileThatDoesExist);
        double referenceLogZAtSimulatedPoints[3] = {0.0, 3.14, 0.0};
        BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<reweighter.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getters)

	BOOST_AUTO_TEST_CASE(getters1)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        ReweighterTest reweighter(fileThatDoesExist);
		std::vector<std::vector<double> > refParameters;
		double refLine1[3] = {4.0, 0.7, -1.e12};
		double refLine2[3] = {4.5, 1.0, -5.e12};
		double refLine3[3] = {4.5, 1.3, -1.e13};
		std::vector<double> aux1(refLine1, refLine1 + sizeof(refLine1) / sizeof(double));
		std::vector<double> aux2(refLine2, refLine2 + sizeof(refLine2) / sizeof(double));
		std::vector<double> aux3(refLine3, refLine3 + sizeof(refLine3) / sizeof(double));
		refParameters.push_back(aux1);
		refParameters.push_back(aux2);
		refParameters.push_back(aux3);
        BOOST_REQUIRE(refParameters == reweighter.getValuesOfSimulationParameters());
	}

	BOOST_AUTO_TEST_CASE(getters2)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
		std::vector<std::vector<double> > refPoints;
		double refPoint1[3] = {4.2, 0.8, -1.6e12};
		double refPoint2[3] = {4.2, 0.8, -1.2e12};
		double refPoint3[3] = {4.2, 1.2, -1.6e12};
		double refPoint4[3] = {4.2, 1.2, -1.2e12};
		double refPoint5[3] = {4.6, 0.8, -1.6e12};
		double refPoint6[3] = {4.6, 0.8, -1.2e12};
		double refPoint7[3] = {4.6, 1.2, -1.6e12};
		double refPoint8[3] = {4.6, 1.2, -1.2e12};
		std::vector<double> aux1(refPoint1, refPoint1 + sizeof(refPoint1) / sizeof(double));
		std::vector<double> aux2(refPoint2, refPoint2 + sizeof(refPoint2) / sizeof(double));
		std::vector<double> aux3(refPoint3, refPoint3 + sizeof(refPoint3) / sizeof(double));
		std::vector<double> aux4(refPoint4, refPoint4 + sizeof(refPoint4) / sizeof(double));
		std::vector<double> aux5(refPoint5, refPoint5 + sizeof(refPoint5) / sizeof(double));
		std::vector<double> aux6(refPoint6, refPoint6 + sizeof(refPoint6) / sizeof(double));
		std::vector<double> aux7(refPoint7, refPoint7 + sizeof(refPoint7) / sizeof(double));
		std::vector<double> aux8(refPoint8, refPoint8 + sizeof(refPoint8) / sizeof(double));
		refPoints.push_back(aux1);
		refPoints.push_back(aux2);
		refPoints.push_back(aux3);
		refPoints.push_back(aux4);
		refPoints.push_back(aux5);
		refPoints.push_back(aux6);
		refPoints.push_back(aux7);
		refPoints.push_back(aux8);
        BOOST_REQUIRE(refPoints == reweighter.getValuesOfNewParameters());
	}

	BOOST_AUTO_TEST_CASE(getters3)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
		unsigned int numberOfNewPoints = 64;
        BOOST_REQUIRE_EQUAL(numberOfNewPoints, reweighter.getNumberOfNewPoints());
	}

	BOOST_AUTO_TEST_CASE(getters4)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        ReweighterTest reweighter(fileThatDoesExist);
        double referenceLogZAtSimulatedPoints[3] = {0.0, 0.0, 0.0};
        BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<reweighter.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
	}

	BOOST_AUTO_TEST_CASE(getters5)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        ReweighterTest* reweighter = new ReweighterTest(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->getLogZAtNewPoints().size(), std::logic_error);
        delete reweighter;
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(3, 4);
        newRanges.push_back(std::make_pair(4.2, 4.6));
        newRanges.push_back(std::make_pair(0.8, 1.2));
        newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        reweighter = new ReweighterTest(fileThatDoesExist, newRanges, newNumPoints);
        BOOST_REQUIRE_EQUAL(reweighter->getLogZAtNewPoints().size(), 0);
	}

	BOOST_AUTO_TEST_CASE(getters6)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        ReweighterTest reweighter(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(reweighter.getPrecisionToCalculateLogZ(), 1.e-7);
        ReweighterTest reweighter2(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(), "jack", 0.001);
		BOOST_REQUIRE_EQUAL(reweighter2.getPrecisionToCalculateLogZ(), 0.001);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setters)

	BOOST_AUTO_TEST_CASE(setters1)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest* reweighter = new ReweighterTest(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->setNewRangesOfParameters(newRanges), std::invalid_argument);
		delete reweighter;
        reweighter = new ReweighterTest(fileThatDoesExist, newRanges, newNumPoints);
        newRanges.pop_back();
        BOOST_REQUIRE_THROW(reweighter->setNewRangesOfParameters(newRanges), std::invalid_argument);
        newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		newRanges[1] = std::make_pair(0.7, 1.3);
        BOOST_REQUIRE_NO_THROW(reweighter->setNewRangesOfParameters(newRanges));
		std::vector<std::vector<double> > refPoints;
		double refPoint1[3] = {4.2, 0.7, -1.6e12};
		double refPoint2[3] = {4.2, 0.7, -1.2e12};
		double refPoint3[3] = {4.2, 1.3, -1.6e12};
		double refPoint4[3] = {4.2, 1.3, -1.2e12};
		double refPoint5[3] = {4.6, 0.7, -1.6e12};
		double refPoint6[3] = {4.6, 0.7, -1.2e12};
		double refPoint7[3] = {4.6, 1.3, -1.6e12};
		double refPoint8[3] = {4.6, 1.3, -1.2e12};
		std::vector<double> aux1(refPoint1, refPoint1 + sizeof(refPoint1) / sizeof(double));
		std::vector<double> aux2(refPoint2, refPoint2 + sizeof(refPoint2) / sizeof(double));
		std::vector<double> aux3(refPoint3, refPoint3 + sizeof(refPoint3) / sizeof(double));
		std::vector<double> aux4(refPoint4, refPoint4 + sizeof(refPoint4) / sizeof(double));
		std::vector<double> aux5(refPoint5, refPoint5 + sizeof(refPoint5) / sizeof(double));
		std::vector<double> aux6(refPoint6, refPoint6 + sizeof(refPoint6) / sizeof(double));
		std::vector<double> aux7(refPoint7, refPoint7 + sizeof(refPoint7) / sizeof(double));
		std::vector<double> aux8(refPoint8, refPoint8 + sizeof(refPoint8) / sizeof(double));
		refPoints.push_back(aux1);
		refPoints.push_back(aux2);
		refPoints.push_back(aux3);
		refPoints.push_back(aux4);
		refPoints.push_back(aux5);
		refPoints.push_back(aux6);
		refPoints.push_back(aux7);
		refPoints.push_back(aux8);
        BOOST_REQUIRE(refPoints == reweighter->getValuesOfNewParameters());
        delete reweighter;
	}

	BOOST_AUTO_TEST_CASE(setters2)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest* reweighter = new ReweighterTest(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
		delete reweighter;
        reweighter = new ReweighterTest(fileThatDoesExist, newRanges, newNumPoints);
        newNumPoints.pop_back();
        BOOST_REQUIRE_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
        newNumPoints.push_back(4);
		newNumPoints[1] = 5;
        BOOST_REQUIRE_NO_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints));
		BOOST_REQUIRE_EQUAL(reweighter->getNumberOfNewPoints(), 80);
        delete reweighter;
	}

	BOOST_AUTO_TEST_CASE(setters3)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest reweighter(fileThatDoesExist);
        BOOST_REQUIRE_NO_THROW(reweighter.setNewParameters(newRanges, newNumPoints));
		std::vector<std::vector<double> > refPoints;
		double refPoint1[3] = {4.2, 0.8, -1.6e12};
		double refPoint2[3] = {4.2, 0.8, -1.2e12};
		double refPoint3[3] = {4.2, 1.2, -1.6e12};
		double refPoint4[3] = {4.2, 1.2, -1.2e12};
		double refPoint5[3] = {4.6, 0.8, -1.6e12};
		double refPoint6[3] = {4.6, 0.8, -1.2e12};
		double refPoint7[3] = {4.6, 1.2, -1.6e12};
		double refPoint8[3] = {4.6, 1.2, -1.2e12};
		std::vector<double> aux1(refPoint1, refPoint1 + sizeof(refPoint1) / sizeof(double));
		std::vector<double> aux2(refPoint2, refPoint2 + sizeof(refPoint2) / sizeof(double));
		std::vector<double> aux3(refPoint3, refPoint3 + sizeof(refPoint3) / sizeof(double));
		std::vector<double> aux4(refPoint4, refPoint4 + sizeof(refPoint4) / sizeof(double));
		std::vector<double> aux5(refPoint5, refPoint5 + sizeof(refPoint5) / sizeof(double));
		std::vector<double> aux6(refPoint6, refPoint6 + sizeof(refPoint6) / sizeof(double));
		std::vector<double> aux7(refPoint7, refPoint7 + sizeof(refPoint7) / sizeof(double));
		std::vector<double> aux8(refPoint8, refPoint8 + sizeof(refPoint8) / sizeof(double));
		refPoints.push_back(aux1);
		refPoints.push_back(aux2);
		refPoints.push_back(aux3);
		refPoints.push_back(aux4);
		refPoints.push_back(aux5);
		refPoints.push_back(aux6);
		refPoints.push_back(aux7);
		refPoints.push_back(aux8);
        BOOST_REQUIRE(refPoints == reweighter.getValuesOfNewParameters());
        BOOST_REQUIRE_EQUAL(reweighter.getNumberOfNewPoints(), 8);
	}

	BOOST_AUTO_TEST_CASE(setters4)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "jack", 0.1);
        BOOST_REQUIRE_THROW(reweighter.setPrecisionToCalculateLogZ(-3.e-10), std::range_error);
        BOOST_REQUIRE_NO_THROW(reweighter.setPrecisionToCalculateLogZ(1.e-10));
        BOOST_REQUIRE_EQUAL(reweighter.getPrecisionToCalculateLogZ(), 1.e-10);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(logZ)

    BOOST_AUTO_TEST_CASE(logZ1)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
//        for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
//            std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
        double referenceLogZAtNewPoints[] = {141.808725391091, 139.786001864617, 137.763562229189, 135.741409322221,
                                             133.719546041019, 131.697975343092, 129.676700246250, 127.655723828738,
                                             125.635049229069, 123.614679645934, 121.594618337982, 119.574868623334,
                                             117.555433879136, 115.536317540892, 113.517523101742, 111.499054111443,
                                             109.480914175351, 107.463106953159, 105.445636157561, 103.428505552580,
                                             101.411718951886, 99.395280216830, 97.379193254392, 95.363462014766,
                                             93.348090488913, 91.333082705808, 89.318442729589, 87.304174656315,
                                             85.290282610689, 83.276770742455};
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtNewPoints();
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
	}

    BOOST_AUTO_TEST_CASE(logZ2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        const int pointToBeLeftOut = 1652;
        double referenceLogZAtNewPoints[] = {1.337554308611, -38.670153942900, -98.348187380816};
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        const int pointToBeLeftOut = 0;
        double referenceLogZAtNewPoints[] = {1.337625561178, -38.670359441083, -98.348790292515};
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ4)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        const int pointToBeLeftOut = 1;
        double referenceLogZAtNewPoints[] = {1.337668078209, -38.670327958098, -98.348659434259};
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingUncorrDataAndLeavingOutOneEntry(pointToBeLeftOut);
        for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

    BOOST_AUTO_TEST_CASE(logZ5)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
		//for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
		//	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
//        double referenceLogZAtSimulatedPoints[] = {1.337622033616, -38.670337444638, -98.348640300104};
        double referenceLogZAtSimulatedPoints[] = {1.337621922716, -38.670337462040, -98.348640282292};
		std::vector<double> simulatedLogZ = reweighter.testCalculateLogZAtSimulatedPoints();
		for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
	}

    BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( logZ6, 2 )
    BOOST_AUTO_TEST_CASE(logZ6)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_2";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        //for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
        //	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
        double referenceLogZAtSimulatedPoints[] = {1.337622033616, -38.670337444638, -98.348640300104};
        std::vector<double> simulatedLogZ = reweighter.testCalculateLogZAtSimulatedPoints();

        BOOST_CHECK_CLOSE(referenceLogZAtSimulatedPoints[0], simulatedLogZ[0], 1.e-8);
        BOOST_REQUIRE_EQUAL(referenceLogZAtSimulatedPoints[1], simulatedLogZ[1]);
        BOOST_CHECK_CLOSE(referenceLogZAtSimulatedPoints[2], simulatedLogZ[2], 1.e-8);
        /*
         * For some reason, setting one or more values of logZ in the configfile as done here,
         * leads to slightly different values of logZ at the remaining simulated points then
         * when all the values are calculated. That is why we have above two expected failures
         * and why we repeate here such a failing checks with less precision.
         */
        BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[0], simulatedLogZ[0], 2.e-5);
        BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[2], simulatedLogZ[2], 2.e-5);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(columnsReweighting)

    BOOST_AUTO_TEST_CASE(columnsReweighting1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        ReweighterTest reweighter(fileThatDoesExist);
        const int numberOfObservablesInFiles = 4; //1 obs given + 3 central moments
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
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
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
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist);
        reweighter.setNewParameters(newRanges, newNumPoints);
        const int numberOfObservablesInFiles = 4; //1 obs given + 3 central moments
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
        //since we know the binsize is one we can reweight on the binned data even if on the reference code we used the raw data
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
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist);
        reweighter.setNewParameters(newRanges, newNumPoints);
        const int numberOfObservablesInFiles = 4; //1 obs given + 3 central moments
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
        //since we know the binsize is one we can reweight on the binned data even if on the reference code we used the raw data
        std::vector<std::vector<double> > valuesObsNewPoints =
                reweighter.testCalculateReweightedObservableValues(true, pointToBeLeftOut, &logZSim, &logZNew);
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], exp(valuesObsNewPoints[i][0]), 1.e-8);
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
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        double referenceValuesObs1NewPoints[] = {0.51320168201844, 0.51325851090442, 0.51331616521147, 0.51337465130085,
                                                0.51343397516939, 0.51349414242501, 0.51355515826122, 0.51361702743295,
                                                0.51367975423067, 0.51374334245435, 0.51380779538793, 0.51387311577323,
                                                0.51393930578366, 0.51400636699839, 0.51407430037612, 0.51414310623005,
                                                0.51421278420172, 0.51428333323675, 0.51435475156028, 0.51442703665301,
                                                0.51450018522894, 0.51457419321289, 0.51464905571958, 0.51472476703396,
                                                0.51480132059263, 0.51487870896691, 0.51495692384690, 0.51503595602794,
                                                0.51511579539832, 0.51519643092919};
        double referenceValuesObs2NewPoints[] = {0.51256553898625, 0.51262063821288, 0.51267655627508, 0.51273329924225,
                                                 0.51279087281439, 0.51284928229804, 0.51290853258130, 0.51296862810939,
                                                 0.51302957285910, 0.51309137031375, 0.51315402343707, 0.51321753464798,
                                                 0.51328190579459, 0.51334713812896, 0.51341323228079, 0.51348018823339,
                                                 0.51354800529792, 0.51361668208945, 0.51368621650296, 0.51375660569044,
                                                 0.51382784603832, 0.51389993314671, 0.51397286180825, 0.51404662598974,
                                                 0.51412121881387, 0.51419663254314, 0.51427285856484, 0.51434988737802,
                                                 0.51442770858199, 0.51450631086732};
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
            BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 1.e-8);
            BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].mean.estimate, 1.e-8);
        }
    }

    BOOST_AUTO_TEST_CASE(meanReweighting2)
    {
        /*
         * Here we compare our estimate of the observables with error with the same of the Reference Code.
         * Since in the Ref. Code they are estimated using bootstrap, we cannot really compare the error.
         * What we do, since the value itself is compatible up to 2e-5%, is to check how different are the
         * errors. But, again, due to the different methods, one cannot expect a real good agreement. The
         * reference code use bootstrap, resamplig 100 times the data, here we use jackknife. A disagreement
         * of 44% on some points is what happens here.
         *
         * NOTE: Plotting the reference and our reults gives indeed a real qualitative agreement.
         */
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 51);
        newRanges.push_back(std::make_pair(5.348, 5.363));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        double referenceValuesObsNewPoints[] = {0.5116222366607, 0.5117708109991, 0.5119229431982, 0.5120788380495,
                                                0.5122387183650, 0.5124028249557, 0.5125714162663, 0.5127447675902,
                                                0.5129231697944, 0.5131069274668, 0.5132963564092, 0.5134917803835,
                                                0.5136935270402, 0.5139019229535, 0.5141172877143, 0.5143399270515,
                                                0.5145701249893, 0.5148081350933, 0.5150541709091, 0.5153083957655,
                                                0.5155709121835, 0.5158417512059, 0.5161208620410, 0.5164081024727,
                                                0.5167032305472, 0.5170058980666, 0.5173156464176, 0.5176319052190,
                                                0.5179539941819, 0.5182811284495, 0.5186124275148, 0.5189469276189,
                                                0.5192835973224, 0.5196213557414, 0.5199590927561, 0.5202956903662,
                                                0.5206300442829, 0.5209610848380, 0.5212877963393, 0.5216092341180,
                                                0.5219245386777, 0.5222329465491, 0.5225337976607, 0.5228265392424,
                                                0.5231107264513, 0.5233860200586, 0.5236521816392, 0.5239090667638,
                                                0.5241566167174, 0.5243948492488, 0.5246238488203};
        double referenceErrorsObsNewPoints[] = {0.0008030982720, 0.0007802295133, 0.0007576602812, 0.0007354310230,
                                                0.0007135806103, 0.0006921468735, 0.0006711672542, 0.0006506795537,
                                                0.0006307227551, 0.0006113378642, 0.0005925687297, 0.0005744627553,
                                                0.0005570714308, 0.0005404505755, 0.0005246601866, 0.0005097637873,
                                                0.0004958271770, 0.0004829165234, 0.0004710958003, 0.0004604236486,
                                                0.0004509498423, 0.0004427116537, 0.0004357304902, 0.0004300092360,
                                                0.0004255306907, 0.0004222574077, 0.0004201330569, 0.0004190852175,
                                                0.0004190293031, 0.0004198731524, 0.0004215217478, 0.0004238815243,
                                                0.0004268638477, 0.0004303873788, 0.0004343792351, 0.0004387750320,
                                                0.0004435180391, 0.0004485577642, 0.0004538483259, 0.0004593469343,
                                                0.0004650127288, 0.0004708061028, 0.0004766885469, 0.0004826229349,
                                                0.0004885741132, 0.0004945096355, 0.0005004004760, 0.0005062216043,
                                                0.0005119523373, 0.0005175764449, 0.0005230820069};
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-5);
            BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].mean.error, 45);
        }
    }

    BOOST_AUTO_TEST_CASE(meanReweighting3)
	{
		/*
		 * Here we compare our estimate of the observables with error with the same of the Reference Code.
		 * Now that we implemented also bootstrap, also the error has to match!
		 * In this particular test we use 1 as binsize and this forces the bootstrap to choose always the
		 * same value! Thus the error must be zero!
		 */
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 51);
		newRanges.push_back(std::make_pair(5.348, 5.363));
		ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot_5");
		double referenceValuesObsNewPoints[] = {0.5116222366607, 0.5117708109991, 0.5119229431982, 0.5120788380495,
												0.5122387183650, 0.5124028249557, 0.5125714162663, 0.5127447675902,
												0.5129231697944, 0.5131069274668, 0.5132963564092, 0.5134917803835,
												0.5136935270402, 0.5139019229535, 0.5141172877143, 0.5143399270515,
												0.5145701249893, 0.5148081350933, 0.5150541709091, 0.5153083957655,
												0.5155709121835, 0.5158417512059, 0.5161208620410, 0.5164081024727,
												0.5167032305472, 0.5170058980666, 0.5173156464176, 0.5176319052190,
												0.5179539941819, 0.5182811284495, 0.5186124275148, 0.5189469276189,
												0.5192835973224, 0.5196213557414, 0.5199590927561, 0.5202956903662,
												0.5206300442829, 0.5209610848380, 0.5212877963393, 0.5216092341180,
												0.5219245386777, 0.5222329465491, 0.5225337976607, 0.5228265392424,
												0.5231107264513, 0.5233860200586, 0.5236521816392, 0.5239090667638,
												0.5241566167174, 0.5243948492488, 0.5246238488203};
		double referenceErrorsObsNewPoints = 0.0;
		reweighter.testCalculateLogZAtSimulatedPoints();
		reweighter.testCalculateLogZAtNewPoints();
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
			BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-5);
			BOOST_REQUIRE_EQUAL(referenceErrorsObsNewPoints, valuesObsNewPoints[i][0].mean.error);
		}
	}

//    BOOST_AUTO_TEST_CASE(meanReweighting4)
//	{
//		/*
//		 * Here we compare our estimate of the observables with error with the same of the Reference Code.
//		 * Now that we implemented also bootstrap, also the error has to match!
//		 */
//		std::string fileThatDoesExist = "RealTestData/configfile_5";
//		std::vector<std::pair<double, double> > newRanges;
//		std::vector< unsigned int> newNumPoints(1, 21);
//		newRanges.push_back(std::make_pair(5.348, 5.363));
//		ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot_5");
//		double referenceValuesObsNewPoints[] = {};
//		double referenceErrorsObsNewPoints = 0.0;
//		reweighter.testCalculateLogZAtSimulatedPoints();
//		reweighter.testCalculateLogZAtNewPoints();
//		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
//		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
//			BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-5);
//			BOOST_REQUIRE_EQUAL(referenceErrorsObsNewPoints, valuesObsNewPoints[i][0].mean.error);
//		}
//	}

    BOOST_AUTO_TEST_CASE(meanReweighting5)
    {
        /*
         * A final test for the observables is to repeat the reweighting in the same range, but different
         * number of points. if the same new point is touched in the reweighting, then we can check that
         * the observables values and errors are the same there.
         */
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 11);
        newRanges.push_back(std::make_pair(5.348, 5.363));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
        std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
        newNumPoints[0]=21;
        reweighter.setNewNumberOfPointsOfParameters(newNumPoints);
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.testCalculateAndGetReweightedObservables();
        std::vector<std::vector<double> > valuesNewPoints2 = reweighter.getValuesOfNewParameters();
        for(size_t i=0; i<valuesNewPoints1.size(); i++){
            for(size_t j=0; j<valuesNewPoints2.size(); j++){
                if(valuesNewPoints2[j] == valuesNewPoints1[i]){
                    BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.estimate,
                                        valuesObsNewPoints2[j][0].mean.estimate, doublePrecisionInPercent);
                    BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.error,
                                        valuesObsNewPoints2[j][0].mean.error, doublePrecisionInPercent);
                }
            }
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
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 30);
       newRanges.push_back(std::make_pair(5.348, 5.3509));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObs1NewPoints[] = {1.3603824802255e-05, 1.3802061604562e-05, 1.4001874343861e-05,
                                               1.4203178181516e-05, 1.4405882156732e-05, 1.4609889324058e-05,
                                               1.4815097160059e-05, 1.5021396598758e-05, 1.5228672402612e-05,
                                               1.5436803437741e-05, 1.5645662103325e-05, 1.5855114451124e-05,
                                               1.6065020334299e-05, 1.6275233248653e-05, 1.6485600652982e-05,
                                               1.6695963580227e-05, 1.6906157311480e-05, 1.7116010902651e-05,
                                               1.7325347779540e-05, 1.7533985776141e-05, 1.7741737131538e-05,
                                               1.7948409099688e-05, 1.8153803826637e-05, 1.8357719036799e-05,
                                               1.8559948000318e-05, 1.8760280085406e-05, 1.8958501205424e-05,
                                               1.9154393932075e-05, 1.9347738313358e-05, 1.9538311996248e-05};
       double referenceValuesObs2NewPoints[] = {1.4279124830430e-05, 1.4470821656898e-05, 1.4663992646324e-05,
                                                1.4858551514396e-05, 1.5054406157511e-05, 1.5251458535870e-05,
                                                1.5449605005546e-05, 1.5648735672835e-05, 1.5848734833623e-05,
                                                1.6049480409175e-05, 1.6250844537824e-05, 1.6452693125057e-05,
                                                1.6654885933498e-05, 1.6857276506299e-05, 1.7059712900114e-05,
                                                1.7262036608456e-05, 1.7464083724272e-05, 1.7665684413581e-05,
                                                1.7866663516886e-05, 1.8066840317688e-05, 1.8266029256919e-05,
                                                1.8464039495403e-05, 1.8660675936699e-05, 1.8855738969592e-05,
                                                1.9049025102413e-05, 1.9240327183534e-05, 1.9429434950036e-05,
                                                1.9616135111089e-05, 1.9800212342380e-05, 1.9981449209561e-05};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
           BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].susceptibility.estimate, 2e-7);
       }
   }

   BOOST_AUTO_TEST_CASE(suscReweighting2)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObsNewPoints[] = {1.18041718360e-05, 1.20823773460e-05, 1.23763242581e-05, 1.26874539116e-05,
                                               1.30172179636e-05, 1.33670535187e-05, 1.37383525289e-05, 1.41324251032e-05,
                                               1.45504564428e-05, 1.49934585608e-05, 1.54622119757e-05, 1.59572046249e-05,
                                               1.64785617539e-05, 1.70259724256e-05, 1.75986118885e-05, 1.81950634321e-05,
                                               1.88132425221e-05, 1.94503264212e-05, 2.01026959059e-05, 2.07658925071e-05,
                                               2.14345985314e-05, 2.21026456327e-05, 2.27630572981e-05, 2.34081306522e-05,
                                               2.40295599071e-05, 2.46186033309e-05, 2.51662913313e-05, 2.56636700485e-05,
                                               2.61020725004e-05, 2.64734046627e-05, 2.67704317953e-05, 2.69870487343e-05,
                                               2.71185170846e-05, 2.71616537792e-05, 2.71149578396e-05, 2.69786667071e-05,
                                               2.67547380208e-05, 2.64467585910e-05, 2.60597871739e-05, 2.56001429979e-05,
                                               2.50751539847e-05, 2.44928815711e-05, 2.38618390574e-05, 2.31907176965e-05,
                                               2.24881357849e-05, 2.17624190311e-05, 2.10214190743e-05, 2.02723743396e-05,
                                               1.9521811838e-05, 1.8775489525e-05, 1.80383723162e-05};
       double referenceErrorsObsNewPoints[] = {2.44940550452e-06, 2.44111599552e-06, 2.43133862951e-06, 2.42071833824e-06,
                                               2.40990499163e-06, 2.39953374704e-06, 2.39020356185e-06, 2.38245450490e-06,
                                               2.37674451952e-06, 2.37342726622e-06, 2.37273225289e-06, 2.37474926963e-06,
                                               2.37941875786e-06, 2.38652960891e-06, 2.39572550632e-06, 2.40652014153e-06,
                                               2.41832111382e-06, 2.43046137736e-06, 2.44223669617e-06, 2.45294699216e-06,
                                               2.46193890704e-06, 2.46864689690e-06, 2.47262972406e-06, 2.47359915358e-06,
                                               2.47143777461e-06, 2.46620291043e-06, 2.45811438546e-06, 2.44752496976e-06,
                                               2.43487428013e-06, 2.42062941147e-06, 2.40521864071e-06, 2.38896713387e-06,
                                               2.37204519478e-06, 2.35443895281e-06, 2.33595028638e-06, 2.31622752978e-06,
                                               2.29482239386e-06, 2.27126325731e-06, 2.24513201703e-06, 2.21613166572e-06,
                                               2.18413448983e-06, 2.14920505919e-06, 2.11159711326e-06, 2.07172749828e-06,
                                               2.03013347501e-06, 1.98742090189e-06, 1.94421102233e-06, 1.90109239127e-06,
                                               1.85858273635e-06, 1.81710327631e-06, 1.77696622364e-06};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 1e-3);
           BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.error, 53); //Error discrepancies between 1% and 53%
       }
   }

   BOOST_AUTO_TEST_CASE(suscReweighting3)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
       double referenceValuesObsNewPoints[] = {1.18041718360e-05, 1.20823773460e-05, 1.23763242581e-05, 1.26874539116e-05,
                                               1.30172179636e-05, 1.33670535187e-05, 1.37383525289e-05, 1.41324251032e-05,
                                               1.45504564428e-05, 1.49934585608e-05, 1.54622119757e-05, 1.59572046249e-05,
                                               1.64785617539e-05, 1.70259724256e-05, 1.75986118885e-05, 1.81950634321e-05,
                                               1.88132425221e-05, 1.94503264212e-05, 2.01026959059e-05, 2.07658925071e-05,
                                               2.14345985314e-05, 2.21026456327e-05, 2.27630572981e-05, 2.34081306522e-05,
                                               2.40295599071e-05, 2.46186033309e-05, 2.51662913313e-05, 2.56636700485e-05,
                                               2.61020725004e-05, 2.64734046627e-05, 2.67704317953e-05, 2.69870487343e-05,
                                               2.71185170846e-05, 2.71616537792e-05, 2.71149578396e-05, 2.69786667071e-05,
                                               2.67547380208e-05, 2.64467585910e-05, 2.60597871739e-05, 2.56001429979e-05,
                                               2.50751539847e-05, 2.44928815711e-05, 2.38618390574e-05, 2.31907176965e-05,
                                               2.24881357849e-05, 2.17624190311e-05, 2.10214190743e-05, 2.02723743396e-05,
                                               1.9521811838e-05, 1.8775489525e-05, 1.80383723162e-05};
       double referenceErrorsObsNewPoints[] = {2.44940550452e-06, 2.44111599552e-06, 2.43133862951e-06, 2.42071833824e-06,
                                               2.40990499163e-06, 2.39953374704e-06, 2.39020356185e-06, 2.38245450490e-06,
                                               2.37674451952e-06, 2.37342726622e-06, 2.37273225289e-06, 2.37474926963e-06,
                                               2.37941875786e-06, 2.38652960891e-06, 2.39572550632e-06, 2.40652014153e-06,
                                               2.41832111382e-06, 2.43046137736e-06, 2.44223669617e-06, 2.45294699216e-06,
                                               2.46193890704e-06, 2.46864689690e-06, 2.47262972406e-06, 2.47359915358e-06,
                                               2.47143777461e-06, 2.46620291043e-06, 2.45811438546e-06, 2.44752496976e-06,
                                               2.43487428013e-06, 2.42062941147e-06, 2.40521864071e-06, 2.38896713387e-06,
                                               2.37204519478e-06, 2.35443895281e-06, 2.33595028638e-06, 2.31622752978e-06,
                                               2.29482239386e-06, 2.27126325731e-06, 2.24513201703e-06, 2.21613166572e-06,
                                               2.18413448983e-06, 2.14920505919e-06, 2.11159711326e-06, 2.07172749828e-06,
                                               2.03013347501e-06, 1.98742090189e-06, 1.94421102233e-06, 1.90109239127e-06,
                                               1.85858273635e-06, 1.81710327631e-06, 1.77696622364e-06};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 1e-3);
           BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.error, 1e-3);
       }
   }

   BOOST_AUTO_TEST_CASE(suscReweighting4)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 11);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
       std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
       newNumPoints[0]=21;
       reweighter.setNewNumberOfPointsOfParameters(newNumPoints);
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.testCalculateAndGetReweightedObservables();
       std::vector<std::vector<double> > valuesNewPoints2 = reweighter.getValuesOfNewParameters();
       for(size_t i=0; i<valuesNewPoints1.size(); i++){
           for(size_t j=0; j<valuesNewPoints2.size(); j++){
               if(valuesNewPoints2[j] == valuesNewPoints1[i]){
                   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.estimate,
                                       valuesObsNewPoints2[j][0].susceptibility.estimate, doublePrecisionInPercent);
                   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].susceptibility.error,
                                       valuesObsNewPoints2[j][0].susceptibility.error, doublePrecisionInPercent);
               }
           }
       }
   }

//   BOOST_AUTO_TEST_CASE(suscReweighting4)
//   {
//       std::string fileThatDoesExist = "RealTestData/configfile_real";
//       std::vector<std::pair<double, double> > newRanges;
//       std::vector< unsigned int> newNumPoints(1, 51);
//       newRanges.push_back(std::make_pair(5.204, 5.216));
//       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
//       reweighter.testCalculateLogZAtSimulatedPoints();
//       reweighter.testCalculateLogZAtNewPoints();
//       std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
//       std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
//       for(size_t i=0; i<valuesNewPoints1.size(); i++){
//           std::cout << valuesNewPoints1[i][0];
//           std::cout << "     " << valuesObsNewPoints1[i][0].mean.estimate;
//           std::cout << "     " << valuesObsNewPoints1[i][0].mean.error;
//           std::cout << "     " << valuesObsNewPoints1[i][0].susceptibility.estimate;
//           std::cout << "     " << valuesObsNewPoints1[i][0].susceptibility.error;
//           std::cout << "     " << valuesObsNewPoints1[i][0].binderCumulant.estimate;
//           std::cout << "     " << valuesObsNewPoints1[i][0].binderCumulant.error << "\n";
//       }
//   }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(skewReweighting)

    /*
     * For the skewness we do not have a reference code, then we just auto test our code (third case above)
     */
    BOOST_AUTO_TEST_CASE(skewReweighting1)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 11);
        newRanges.push_back(std::make_pair(5.348, 5.363));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
        std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
        newNumPoints[0]=21;
        reweighter.setNewNumberOfPointsOfParameters(newNumPoints);
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.testCalculateAndGetReweightedObservables();
        std::vector<std::vector<double> > valuesNewPoints2 = reweighter.getValuesOfNewParameters();
        for(size_t i=0; i<valuesNewPoints1.size(); i++){
            for(size_t j=0; j<valuesNewPoints2.size(); j++){
                if(valuesNewPoints2[j] == valuesNewPoints1[i]){
                    BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.estimate,
                                        valuesObsNewPoints2[j][0].skewness.estimate, doublePrecisionInPercent);
                    BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].skewness.error,
                                        valuesObsNewPoints2[j][0].skewness.error, doublePrecisionInPercent);
                }
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(bindReweighting)

    /*
     * Same tests as the suite suscReweighting, but on the Binder cumulant. Read the comments
     * of that suite for more information (e.g. for the precision used here in the following cases).
     */
    BOOST_AUTO_TEST_CASE(bindReweighting1)
    {
       std::string fileThatDoesExist = "RealTestData/configfile_4";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 30);
       newRanges.push_back(std::make_pair(5.348, 5.3509));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObs1NewPoints[] = {3.5206418419434, 3.4811047647748, 3.4422868065820, 3.4037911581673,
                                                3.3654461046483, 3.3266506475774, 3.2893686947969, 3.2521701420186,
                                                3.2146256967831, 3.1775254802705, 3.1411432046910, 3.1050859937156,
                                                3.0694978511434, 3.0340633510535, 2.9993095138561, 2.9648235590273,
                                                2.9310135311294, 2.8975103856420, 2.8646387372190, 2.8323858869864,
                                                2.8006262018092, 2.7697187765980, 2.7390932438978, 2.7092074179912,
                                                2.6799493470121, 2.6512487523515, 2.6234129853386, 2.5961946548546,
                                                2.5698786179766, 2.5438683396179};
       double referenceValuesObs2NewPoints[] = {3.4360361705923, 3.4024769315471, 3.3691198522261, 3.3360068854480,
                                                3.3027327076808, 3.2694128913535, 3.2366412466910, 3.2037629166616,
                                                3.1714853825031, 3.1388150603066, 3.1065025004356, 3.0747345427196,
                                                3.0431198977099, 3.0115887450243, 2.9807063385440, 2.9499729504106,
                                                2.9197574225914, 2.8897981708243, 2.8603155012882, 2.8312839862164,
                                                2.8028490876923, 2.7747154644745, 2.7472141628739, 2.7202043831915,
                                                2.6936680631492, 2.6677930453465, 2.6427634054841, 2.6179334597853,
                                                2.5939597583269, 2.5705999029340};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 2.5); //Discrepancy between 0.1% and 2.5%
           BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 4.e-3); //Discrepancy below 0.004%
       }
   }

   BOOST_AUTO_TEST_CASE(bindReweighting2)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObsNewPoints[] = {3.66375761371, 3.69650616987, 3.72457417712, 3.74795713234, 3.76569768083,
                                               3.77603512058, 3.77981435539, 3.77585380916, 3.76248663931, 3.74085560046,
                                               3.70984835127, 3.66985581037, 3.62077841424, 3.56338601954, 3.49780828080,
                                               3.42518775556, 3.34638333564, 3.26222114882, 3.17417733627, 3.08356659523,
                                               2.99168815163, 2.89985310543, 2.80940281063, 2.72172094692, 2.63789836767,
                                               2.55917645029, 2.48659298893, 2.42106479046, 2.36336287063, 2.31420835294,
                                               2.27412768644, 2.24356543318, 2.22282868431, 2.21211398723, 2.21151026587,
                                               2.22095366319, 2.24033742892, 2.26945464115, 2.30791289566, 2.35529914540,
                                               2.41106011000, 2.47458217738, 2.54541070229, 2.62241661520, 2.70496900329,
                                               2.79224431553, 2.88329365020, 2.97715926384, 3.07288314386, 3.16983392340,
                                               3.26683564435};
       double referenceErrorsObsNewPoints[] = {0.803155817427, 0.769716297759, 0.734388132974, 0.697530087799,
                                               0.659649639951, 0.621180774101, 0.582751555823, 0.544858246402,
                                               0.508260523635, 0.473599767875, 0.441442226967, 0.412251322589,
                                               0.386311828149, 0.363733221674, 0.344314472611, 0.327691172763,
                                               0.313269426192, 0.300373039427, 0.288347851941, 0.276569007168,
                                               0.264564974073, 0.252026646327, 0.238797439711, 0.224892759252,
                                               0.210457052107, 0.195775446726, 0.181250212666, 0.167389477654,
                                               0.154800259571, 0.144163289874, 0.136172354444, 0.131424348833,
                                               0.130271809414, 0.132717417883, 0.138419590183, 0.146823608306,
                                               0.157321569099, 0.169361867275, 0.182510746409, 0.196453376009,
                                               0.210982207398, 0.225963006373, 0.241348054476, 0.257125993366,
                                               0.273341734783, 0.290057095164, 0.307380296300, 0.325397612382,
                                               0.344215367240, 0.363951452755, 0.384719448161};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.05);
           BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.error, 83); //Error discrepancies between 8% and 83%
       }
   }

   BOOST_AUTO_TEST_CASE(bindReweighting3)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
       double referenceValuesObsNewPoints[] = {3.66375761371, 3.69650616987, 3.72457417712, 3.74795713234, 3.76569768083,
                                               3.77603512058, 3.77981435539, 3.77585380916, 3.76248663931, 3.74085560046,
                                               3.70984835127, 3.66985581037, 3.62077841424, 3.56338601954, 3.49780828080,
                                               3.42518775556, 3.34638333564, 3.26222114882, 3.17417733627, 3.08356659523,
                                               2.99168815163, 2.89985310543, 2.80940281063, 2.72172094692, 2.63789836767,
                                               2.55917645029, 2.48659298893, 2.42106479046, 2.36336287063, 2.31420835294,
                                               2.27412768644, 2.24356543318, 2.22282868431, 2.21211398723, 2.21151026587,
                                               2.22095366319, 2.24033742892, 2.26945464115, 2.30791289566, 2.35529914540,
                                               2.41106011000, 2.47458217738, 2.54541070229, 2.62241661520, 2.70496900329,
                                               2.79224431553, 2.88329365020, 2.97715926384, 3.07288314386, 3.16983392340,
                                               3.26683564435};
       double referenceErrorsObsNewPoints[] = {0.803155817427, 0.769716297759, 0.734388132974, 0.697530087799,
                                               0.659649639951, 0.621180774101, 0.582751555823, 0.544858246402,
                                               0.508260523635, 0.473599767875, 0.441442226967, 0.412251322589,
                                               0.386311828149, 0.363733221674, 0.344314472611, 0.327691172763,
                                               0.313269426192, 0.300373039427, 0.288347851941, 0.276569007168,
                                               0.264564974073, 0.252026646327, 0.238797439711, 0.224892759252,
                                               0.210457052107, 0.195775446726, 0.181250212666, 0.167389477654,
                                               0.154800259571, 0.144163289874, 0.136172354444, 0.131424348833,
                                               0.130271809414, 0.132717417883, 0.138419590183, 0.146823608306,
                                               0.157321569099, 0.169361867275, 0.182510746409, 0.196453376009,
                                               0.210982207398, 0.225963006373, 0.241348054476, 0.257125993366,
                                               0.273341734783, 0.290057095164, 0.307380296300, 0.325397612382,
                                               0.344215367240, 0.363951452755, 0.384719448161};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.05);
           BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.error, 0.05);
       }
   }

   BOOST_AUTO_TEST_CASE(bindReweighting4)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 11);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
       std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
       newNumPoints[0]=21;
       reweighter.setNewNumberOfPointsOfParameters(newNumPoints);
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.testCalculateAndGetReweightedObservables();
       std::vector<std::vector<double> > valuesNewPoints2 = reweighter.getValuesOfNewParameters();
       for(size_t i=0; i<valuesNewPoints1.size(); i++){
           for(size_t j=0; j<valuesNewPoints2.size(); j++){
               if(valuesNewPoints2[j] == valuesNewPoints1[i]){
                   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.estimate,
                                       valuesObsNewPoints2[j][0].binderCumulant.estimate, doublePrecisionInPercent);
                   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].binderCumulant.error,
                                       valuesObsNewPoints2[j][0].binderCumulant.error, doublePrecisionInPercent);
               }
           }
       }
   }

   /*
    * The following test is the same as bindReweighting1
    * but imposing the mean of the observable zero a priori
    */
   BOOST_AUTO_TEST_CASE(bindReweighting5)
   {
	  std::string fileThatDoesExist = "RealTestData/configfile_4";
	  std::vector<std::pair<double, double> > newRanges;
	  std::vector< unsigned int> newNumPoints(1, 30);
	  newRanges.push_back(std::make_pair(5.348, 5.3509));
	  std::vector<unsigned int> colWhoseMeanIsZero;
	  colWhoseMeanIsZero.push_back(1);
	  colWhoseMeanIsZero.push_back(2);
	  ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints,
			                    std::vector<unsigned int>(), colWhoseMeanIsZero);
	  double referenceValuesObs1NewPoints[] = {1.0002080009246, 1.0002109757451, 1.0002139718746, 1.0002169879313,
			  	  	  	  	  	  	  	  	   1.0002200224477, 1.0002230738642, 1.0002261405315, 1.0002292207096,
			  	  	  	  	  	  	  	  	   1.0002323125647, 1.0002354141701, 1.0002385235068, 1.0002416384620,
			  	  	  	  	  	  	  	  	   1.0002447568307, 1.0002478763156, 1.0002509945294, 1.0002541089941,
			  	  	  	  	  	  	  	  	   1.0002572171449, 1.0002603163324, 1.0002634038233, 1.0002664768066,
			  	  	  	  	  	  	  	  	   1.0002695323939, 1.0002725676251, 1.0002755794742, 1.0002785648512,
			  	  	  	  	  	  	  	  	   1.0002815206099, 1.0002844435521, 1.0002873304350, 1.0002901779782,
			  	  	  	  	  	  	  	  	   1.0002929828690, 1.0002957417721};
	  double referenceValuesObs2NewPoints[] = {1.0002187004576, 1.0002215815718, 1.0002244825467, 1.0002274019822,
			  	  	  	  	  	     	   	   1.0002303383912, 1.0002332901980, 1.0002362557382, 1.0002392332579,
			  	  	  	  	  	     	  	   1.0002422209122, 1.0002452167664, 1.0002482187944, 1.0002512248791,
			  	  	  	  	  	     	  	   1.0002542328150, 1.0002572403065, 1.0002602449701, 1.0002632443374,
			  	  	  	  	  	     	  	   1.0002662358548, 1.0002692168881, 1.0002721847234, 1.0002751365727,
			  	  	  	  	  	     	  	   1.0002780695753, 1.0002809808031, 1.0002838672659, 1.0002867259146,
			  	  	  	  	  	     	  	   1.0002895536488, 1.0002923473211, 1.0002951037436, 1.0002978196966,
			  	  	  	  	  	     	  	   1.0003004919334, 1.0003031171893};
	  reweighter.testCalculateLogZAtSimulatedPoints();
	  reweighter.testCalculateLogZAtNewPoints();
	  std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
	  for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
		  BOOST_REQUIRE_CLOSE(referenceValuesObs1NewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
		  BOOST_REQUIRE_CLOSE(referenceValuesObs2NewPoints[i], valuesObsNewPoints[i][1].binderCumulant.estimate, 1.e-8);
	  }
  }

BOOST_AUTO_TEST_SUITE_END()


/*********************************************************************************************/
/* The following tests are just to check that there are not exceptions using the Reweighter. */
/* Probably they are unnecessary, but it could be good to have an overall testing.           */
/*********************************************************************************************/

BOOST_AUTO_TEST_SUITE(reweighter)

std::string fileThatDoesExist = "RealTestData/configfile_1";
std::vector<std::pair<double, double> > newRanges(1,std::make_pair(5.348, 5.3509));
std::vector< unsigned int> newNumPoints(1, 30);

    BOOST_AUTO_TEST_CASE(constructors)
    {
        BOOST_REQUIRE_THROW(Reweighter reweighter, std::invalid_argument);
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist));
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, std::vector<unsigned int>(), std::vector<unsigned int>(), "bootstrap", 1.e-5));
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints));
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "jackknife", 1.e-5));
    }

    BOOST_AUTO_TEST_CASE(setters)
    {
        Reweighter *reweighter= new Reweighter(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
        delete reweighter;
        reweighter= new Reweighter(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->setNewRangesOfParameters(newRanges), std::invalid_argument);
        delete reweighter;
        reweighter= new Reweighter(fileThatDoesExist);
        BOOST_REQUIRE_NO_THROW(reweighter->setNewParameters(newRanges, newNumPoints));
        newNumPoints[0]=20;
        BOOST_REQUIRE_NO_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints));
        newRanges[0]=std::make_pair(5.349, 5.350);
        BOOST_REQUIRE_NO_THROW(reweighter->setNewRangesOfParameters(newRanges));
    }

    BOOST_AUTO_TEST_CASE(realrun)
    {
        double referenceLogZAtSimulatedPoints[] = {1.337622033616, -38.670337444638, -98.348640300104};
        double referenceLogZAtNewPoints[] = {142.508757792722, 140.482386290225, 138.456244389920, 136.430333627617,
                                             134.404655574895, 132.379211839817, 130.354004067544, 128.329033941020,
                                             126.304303181484, 124.279813549106, 122.255566843618, 120.231564904744,
                                             118.207809612811, 116.184302889198, 114.161046696889, 112.138043040797,
                                             110.115293968251, 108.092801569325, 106.070567977252, 104.048595368604,
                                             102.026885963617, 100.005442026359, 97.984265864958, 95.963359831593,
                                             93.942726322617, 91.922367778507, 89.902286683861, 87.882485567149,
                                             85.862967000579, 83.843733599776};
        //Reset ranges and num points because of the previous test (newRanges and newNumPoints are out of the cases)
        newNumPoints[0]=30;
        newRanges[0]=std::make_pair(5.348, 5.3509);
        Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "Jackknife");
        std::vector<double> simulatedLogZ = reweighter.getLogZAtSimulatedPoints();
        std::vector<double> newLogZ = reweighter.getLogZAtNewPoints();
        for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

BOOST_AUTO_TEST_SUITE_END()









