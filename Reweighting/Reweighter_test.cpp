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
    ReweighterTest(std::string configurationFileIn, double precisionToCalculateLogZ = 1.e-7)
     : ReweighterAbstract(configurationFileIn, precisionToCalculateLogZ) {}
    ReweighterTest(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                   std::vector<unsigned int>  newNumberOfPointsOfParametersIn, double precisionToCalculateLogZ = 1.e-7)
     : ReweighterAbstract(configurationFileIn, newRangesOfParametersIn,
                          newNumberOfPointsOfParametersIn, precisionToCalculateLogZ) {}

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

    std::vector<double> testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(const int entryToBeLeftOut){
        return calculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(entryToBeLeftOut);
    }

    std::vector<double> testCalculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(const int entryToBeLeftOut,
                                                                                         std::vector<double> logZSim){
        return calculateLogZAtNewPointsUsingBinnedDataAndLeavingOutOneEntry(entryToBeLeftOut, logZSim);
    }

    void testPrepareObservablesBeforeReweighting(std::vector<double>& minima){
        prepareObservablesBeforeReweighting(minima);
    }

    void testRestoreObservablesAfterReweighting(std::vector<double>& minima,\
                                                std::valarray<std::vector<std::vector<double> > >* jack){
        restoreObservablesAfterReweighting(minima, jack);
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
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, 0.0), std::range_error);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, -1.), std::range_error);
    }

    BOOST_AUTO_TEST_CASE(build3)
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

    BOOST_AUTO_TEST_CASE(build4)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        ReweighterTest reweighter(fileThatDoesExist);
        double referenceLogZAtSimulatedPoints[3] = {3.14, 6.28, 9.42};
        BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<reweighter.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
    }

    BOOST_AUTO_TEST_CASE(build5)
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
        ReweighterTest reweighter2(fileThatDoesExist, 0.001);
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
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, 0.1);
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
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut);
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
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut);
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
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut);
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
        double referenceLogZAtSimulatedPoints[] = {1.337622033616, -38.670337444638, -98.348640300104};
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
        const double referenceOriginalBinnedObservables[] = {0.333333333333333, 0.866666666666667, -1.2};
        std::valarray<double> referencePreparedRawObservables(referenceOriginalRawObservables, 9);
        std::valarray<double> referencePreparedBinnedObservables(referenceOriginalBinnedObservables, 3);
        referencePreparedRawObservables = log(referencePreparedRawObservables - 2*referenceMinimumOfObservables);
        referencePreparedBinnedObservables = log(referencePreparedBinnedObservables - 2*referenceMinimumOfObservables);

        std::vector<double> minimumOfObservables(numberOfObservablesInFiles, std::numeric_limits<double>::max());
        reweighter.testPrepareObservablesBeforeReweighting(minimumOfObservables);
        std::vector<std::vector<double> > preparedRawObservables = reweighter.getObservablesFromSimulationData(true, numberOfObservablesInFiles);
        std::vector<std::vector<double> > preparedBinnedObservables = reweighter.getObservablesFromSimulationData(false, numberOfObservablesInFiles);
        reweighter.testRestoreObservablesAfterReweighting(minimumOfObservables, NULL);
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
        std::vector<double> logZSim = reweighter.testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut);
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
        std::vector<double> logZSim = reweighter.testCalculateLogZAtSimulatedPointsUsingBinnedDataAndLeavingOutOneEntry(pointToBeLeftOut);
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
         *
         * REMARK: Actually there is an important difference between this test case and "observables2".
         *         Here we reweight the observables using as value the jackknife estimate that is
         *         different and in general more accurate for the result. That is why the precision in
         *         this test is 1.e-4 (the reference values are still those of "observables2").
         */
        std::string fileThatDoesExist = "RealTestData/configfile_4";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        double referenceValuesObsNewPoints[] = {0.51320168201844, 0.51325851090442, 0.51331616521147, 0.51337465130085,
                                                0.51343397516939, 0.51349414242501, 0.51355515826122, 0.51361702743295,
                                                0.51367975423067, 0.51374334245435, 0.51380779538793, 0.51387311577323,
                                                0.51393930578366, 0.51400636699839, 0.51407430037612, 0.51414310623005,
                                                0.51421278420172, 0.51428333323675, 0.51435475156028, 0.51442703665301,
                                                0.51450018522894, 0.51457419321289, 0.51464905571958, 0.51472476703396,
                                                0.51480132059263, 0.51487870896691, 0.51495692384690, 0.51503595602794,
                                                0.51511579539832, 0.51519643092919};
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 1.e-4);
    }

    BOOST_AUTO_TEST_CASE(meanReweighting2)
    {
        /*
         * Here we compare our estimate of the observables with error with the same of the Reference Code.
         * Since in the Ref. Code they are estimated using bootstrap, we cannot really compare the error.
         * What we do, since the value itself is compatible up to 0.05%, is to check how different are the
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
            BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-4);
            BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].mean.error, 45);
        }
    }

    BOOST_AUTO_TEST_CASE(meanReweighting3)
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
                if(valuesNewPoints2[j] == valuesNewPoints1[i])
                    BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.estimate,
                                        valuesObsNewPoints2[j][0].mean.estimate, doublePrecisionInPercent);
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(suscReweighting)

    /*
     * Same tests as the suite meanReweighting, but on the susceptibility. Read the comments
     * of that suite for more information.
     */
    BOOST_AUTO_TEST_CASE(suscReweighting1)
    {

       std::string fileThatDoesExist = "RealTestData/configfile_4";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 30);
       newRanges.push_back(std::make_pair(5.348, 5.3509));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObsNewPoints[] = {1.3603824802255e-05,
                                               1.3802061604562e-05,
                                               1.4001874343861e-05,
                                               1.4203178181516e-05,
                                               1.4405882156732e-05,
                                               1.4609889324058e-05,
                                               1.4815097160059e-05,
                                               1.5021396598758e-05,
                                               1.5228672402612e-05,
                                               1.5436803437741e-05,
                                               1.5645662103325e-05,
                                               1.5855114451124e-05,
                                               1.6065020334299e-05,
                                               1.6275233248653e-05,
                                               1.6485600652982e-05,
                                               1.6695963580227e-05,
                                               1.690615731148e-05,
                                               1.7116010902651e-05,
                                               1.732534777954e-05,
                                               1.7533985776141e-05,
                                               1.7741737131538e-05,
                                               1.7948409099688e-05,
                                               1.8153803826637e-05,
                                               1.8357719036799e-05,
                                               1.8559948000318e-05,
                                               1.8760280085406e-05,
                                               1.8958501205424e-05,
                                               1.9154393932075e-05,
                                               1.9347738313358e-05,
                                               1.9538311996248e-05};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 1.e-4);
   }

//   BOOST_AUTO_TEST_CASE(suscReweighting2)
//   {
//       /*
//        * Here we compare our estimate of the observables with error with the same of the Reference Code.
//        * Since in the Ref. Code they are estimated using bootstrap, we cannot really compare the error.
//        * What we do, since the value itself is compatible up to 0.05%, is to check how different are the
//        * errors. But, again, due to the different methods, one cannot expect a real good agreement. The
//        * reference code use bootstrap, resamplig 100 times the data, here we use jackknife. A disagreement
//        * of 44% on some points is what happens here.
//        *
//        * NOTE: Plotting the reference and our reults gives indeed a real qualitative agreement.
//        */
//       std::string fileThatDoesExist = "RealTestData/configfile_5";
//       std::vector<std::pair<double, double> > newRanges;
//       std::vector< unsigned int> newNumPoints(1, 51);
//       newRanges.push_back(std::make_pair(5.348, 5.363));
//       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
//       double referenceValuesObsNewPoints[] = {0.5116222366607, 0.5117708109991, 0.5119229431982, 0.5120788380495,
//                                               0.5122387183650, 0.5124028249557, 0.5125714162663, 0.5127447675902,
//                                               0.5129231697944, 0.5131069274668, 0.5132963564092, 0.5134917803835,
//                                               0.5136935270402, 0.5139019229535, 0.5141172877143, 0.5143399270515,
//                                               0.5145701249893, 0.5148081350933, 0.5150541709091, 0.5153083957655,
//                                               0.5155709121835, 0.5158417512059, 0.5161208620410, 0.5164081024727,
//                                               0.5167032305472, 0.5170058980666, 0.5173156464176, 0.5176319052190,
//                                               0.5179539941819, 0.5182811284495, 0.5186124275148, 0.5189469276189,
//                                               0.5192835973224, 0.5196213557414, 0.5199590927561, 0.5202956903662,
//                                               0.5206300442829, 0.5209610848380, 0.5212877963393, 0.5216092341180,
//                                               0.5219245386777, 0.5222329465491, 0.5225337976607, 0.5228265392424,
//                                               0.5231107264513, 0.5233860200586, 0.5236521816392, 0.5239090667638,
//                                               0.5241566167174, 0.5243948492488, 0.5246238488203};
//       double referenceErrorsObsNewPoints[] = {0.0008030982720, 0.0007802295133, 0.0007576602812, 0.0007354310230,
//                                               0.0007135806103, 0.0006921468735, 0.0006711672542, 0.0006506795537,
//                                               0.0006307227551, 0.0006113378642, 0.0005925687297, 0.0005744627553,
//                                               0.0005570714308, 0.0005404505755, 0.0005246601866, 0.0005097637873,
//                                               0.0004958271770, 0.0004829165234, 0.0004710958003, 0.0004604236486,
//                                               0.0004509498423, 0.0004427116537, 0.0004357304902, 0.0004300092360,
//                                               0.0004255306907, 0.0004222574077, 0.0004201330569, 0.0004190852175,
//                                               0.0004190293031, 0.0004198731524, 0.0004215217478, 0.0004238815243,
//                                               0.0004268638477, 0.0004303873788, 0.0004343792351, 0.0004387750320,
//                                               0.0004435180391, 0.0004485577642, 0.0004538483259, 0.0004593469343,
//                                               0.0004650127288, 0.0004708061028, 0.0004766885469, 0.0004826229349,
//                                               0.0004885741132, 0.0004945096355, 0.0005004004760, 0.0005062216043,
//                                               0.0005119523373, 0.0005175764449, 0.0005230820069};
//       reweighter.testCalculateLogZAtSimulatedPoints();
//       reweighter.testCalculateLogZAtNewPoints();
//       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
//       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
//           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].mean.estimate, 2.e-4);
//           BOOST_REQUIRE_CLOSE(referenceErrorsObsNewPoints[i], valuesObsNewPoints[i][0].mean.error, 45);
//       }
//   }

//   BOOST_AUTO_TEST_CASE(suscReweighting3)
//   {
//       /*
//        * A final test for the observables is to repeat the reweighting in the same range, but different
//        * number of points. if the same new point is touched in the reweighting, then we can check that
//        * the observables values and errors are the same there.
//        */
//       std::string fileThatDoesExist = "RealTestData/configfile_5";
//       std::vector<std::pair<double, double> > newRanges;
//       std::vector< unsigned int> newNumPoints(1, 11);
//       newRanges.push_back(std::make_pair(5.348, 5.363));
//       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
//       reweighter.testCalculateLogZAtSimulatedPoints();
//       reweighter.testCalculateLogZAtNewPoints();
//       std::vector<std::vector<Observables> > valuesObsNewPoints1 = reweighter.testCalculateAndGetReweightedObservables();
//       std::vector<std::vector<double> > valuesNewPoints1 = reweighter.getValuesOfNewParameters();
//       newNumPoints[0]=21;
//       reweighter.setNewNumberOfPointsOfParameters(newNumPoints);
//       reweighter.testCalculateLogZAtSimulatedPoints();
//       reweighter.testCalculateLogZAtNewPoints();
//       std::vector<std::vector<Observables> > valuesObsNewPoints2 = reweighter.testCalculateAndGetReweightedObservables();
//       std::vector<std::vector<double> > valuesNewPoints2 = reweighter.getValuesOfNewParameters();
//       for(size_t i=0; i<valuesNewPoints1.size(); i++){
//           for(size_t j=0; j<valuesNewPoints2.size(); j++){
//               if(valuesNewPoints2[j] == valuesNewPoints1[i])
//                   BOOST_REQUIRE_CLOSE(valuesObsNewPoints1[i][0].mean.estimate,
//                                       valuesObsNewPoints2[j][0].mean.estimate, doublePrecisionInPercent);
//           }
//       }
//   }

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
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, 1.e-5));
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints));
        BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints, 1.e-5));
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
        Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints);
        std::vector<double> simulatedLogZ = reweighter.getLogZAtSimulatedPoints();
        std::vector<double> newLogZ = reweighter.getLogZAtNewPoints();
        for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
            BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
    }

BOOST_AUTO_TEST_SUITE_END()









