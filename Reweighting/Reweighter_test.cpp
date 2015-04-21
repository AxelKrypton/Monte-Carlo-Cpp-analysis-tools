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
        double referenceLogZAtNewPoints[] = {1.337554293255, -38.670153945310, -98.348187363050};
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
        double referenceLogZAtNewPoints[] = {1.337625545806, -38.670359443497, -98.348790274729};
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
        double referenceLogZAtNewPoints[] = {1.337668062797, -38.670327960517, -98.348659416428};
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
        double referenceLogZAtSimulatedPoints[] = {1.337622018220, -38.670337447054, -98.348640282292};
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
        double referenceLogZAtSimulatedPoints[] = {1.337622018220, -38.670337447054, -98.348640282292};
        std::vector<double> simulatedLogZ = reweighter.testCalculateLogZAtSimulatedPoints();

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
        double referenceValuesObs1NewPoints[] = {0.51320168205997, 0.51325851094673, 0.51331616525427, 0.51337465134447,
        										 0.51343397521397, 0.51349414247009, 0.51355515830721, 0.51361702747969,
        										 0.51367975427813, 0.51374334250256, 0.51380779543692, 0.51387311582302,
        										 0.51393930583427, 0.51400636704974, 0.51407430042834, 0.51414310628303,
        										 0.51421278425554, 0.51428333329139, 0.51435475161567, 0.51442703670932,
        										 0.51450018528616, 0.51457419327084, 0.51464905577827, 0.51472476709346,
        										 0.51480132065302, 0.51487870902808, 0.51495692390881, 0.51503595609073,
        										 0.51511579546194, 0.51519643099346};
        double referenceValuesObs2NewPoints[] = {0.51256553902672, 0.51262063825395, 0.51267655631684, 0.51273329928470,
        										 0.51279087285772, 0.51284928234208, 0.51290853262608, 0.51296862815487,
        										 0.51302957290539, 0.51309137036075, 0.51315402348481, 0.51321753469642,
        										 0.51328190584385, 0.51334713817891, 0.51341323233161, 0.51348018828503,
        										 0.51354800535046, 0.51361668214269, 0.51368621655703, 0.51375660574529,
        										 0.51382784609409, 0.51389993320322, 0.51397286186553, 0.51404662604780,
        										 0.51412121887281, 0.51419663260284, 0.51427285862526, 0.51434988743924,
        										 0.51442770864402, 0.51450631093006};
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
		 * same value! Thus the error must be zero! Zero actually up to machine precision. This means that
		 * in the bootstrap error calculation the first moment square should be equal to the second moment,
		 * but the difference could be not zero but around 1.e-15 and then the square root returns something
		 * of the order of 1.e-7 or 1.e-8.
		 */
		std::string fileThatDoesExist = "RealTestData/configfile_5";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 51);
		newRanges.push_back(std::make_pair(5.348, 5.363));
		ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
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
		reweighter.testCalculateLogZAtSimulatedPoints();
		reweighter.testCalculateLogZAtNewPoints();
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
			BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-5);
			BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].mean.error, 1.e-6);
		}
	}

/*
 * Now that we implemented also bootstrap, we could naively think that in a test against the
 * reference code also the error has to match! Actually the error relies on random numbers and
 * this cannot guarantee a good test (and should not be used in tests). One should develop a test
 * in which this randomness is ruled out and this could be done developing new code or accumulating
 * statistics in a long test (this second way should be also be avoided). For the moment it is enough
 * the above test that relies on an analitic result.
 */

    BOOST_AUTO_TEST_CASE(meanReweighting4)
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
       double referenceValuesObs1NewPoints[] = {1.3603824976616e-05, 1.380206166679e-05, 1.4001874653335e-05, 1.4203178408667e-05,
    		   	   	   	   	   	   	    		1.4405882256652e-05, 1.4609889588957e-05, 1.4815097290233e-05, 1.5021396725212e-05,
    		   	   	   	   	   	   	    		1.5228672619272e-05, 1.5436803692037e-05, 1.5645662350183e-05, 1.5855114664398e-05,
    		   	   	   	   	   	   	    		1.6065020487677e-05, 1.6275233447105e-05, 1.6485600843996e-05, 1.6695963831692e-05,
    		   	   	   	   	   	   	    		1.690615752914e-05, 1.7116011105489e-05, 1.7325348027786e-05, 1.7533985941676e-05,
    		   	   	   	   	   	   	    		1.774173728597e-05, 1.7948409269331e-05, 1.8153804045462e-05, 1.835771925196e-05,
    		   	   	   	   	   	   	    		1.8559948189223e-05, 1.8760280304564e-05, 1.8958501421085e-05, 1.9154394102661e-05,
    		   	   	   	   	   	   	    		1.934773840867e-05, 1.9538312250045e-05};
       double referenceValuesObs2NewPoints[] = {1.4279124899874e-05, 1.4470821860901e-05, 1.4663992842945e-05, 1.4858551673713e-05,
    		   	   	   	   	   	   	   	   	    1.5054406242276e-05, 1.5251458680587e-05, 1.5449605135276e-05, 1.5648735851304e-05,
    		   	   	   	   	   	   	   	   	    1.5848734948642e-05, 1.6049480621727e-05, 1.6250844754206e-05, 1.645269338274e-05,
    		   	   	   	   	   	   	   	   	    1.6654886153822e-05, 1.6857276809223e-05, 1.7059713135759e-05, 1.7262036840493e-05,
    		   	   	   	   	   	   	   	   	    1.7464083858998e-05, 1.7665684645951e-05, 1.7866663696908e-05, 1.8066840520414e-05,
    		   	   	   	   	   	   	   	   	    1.8266029395975e-05, 1.8464039638399e-05, 1.8660676139759e-05, 1.8855739150392e-05,
    		   	   	   	   	   	   	   	   	    1.9049025253237e-05, 1.9240327349623e-05, 1.9429435134943e-05, 1.9616135292444e-05,
    		   	   	   	   	   	   	   	   	    1.9800212508803e-05, 1.9981449455253e-05};
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
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 1e-3);
           BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].susceptibility.error, 1.e-6);
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
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.05);
           BOOST_REQUIRE_SMALL(valuesObsNewPoints[i][0].binderCumulant.error, 1.e-6);
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
        double referenceLogZAtSimulatedPoints[] = {1.337622018220, -38.670337447054, -98.348640282292};
        double referenceLogZAtNewPoints[] = {142.508757785609, 140.482386283154, 138.456244382891, 136.430333620630, 134.404655567952,
        									 132.379211832918, 130.354004060687, 128.329033934209, 126.304303174718, 124.279813542386,
        									 122.255566836943, 120.231564898118, 118.207809606231, 116.184302882667, 114.161046690406,
        									 112.138043034363, 110.115293961866, 108.092801562990, 106.070567970968, 104.048595362372,
        									 102.026885957437, 100.005442020232, 97.984265858885, 95.963359825574, 93.942726316654,
        									 91.922367772599, 89.902286678009, 87.882485561355, 85.862966994844, 83.843733594099};
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









