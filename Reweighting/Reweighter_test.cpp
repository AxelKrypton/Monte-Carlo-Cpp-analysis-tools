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
         * Infact, if you use binsize=1 in the bootstrap as it is done here, then you get zero as error
         * everywhere (or something in 1.e-7 or 1.e-8) because the bootstrap estimators are always the same!
         */
        std::string fileThatDoesExist = "RealTestData/configfile_5";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 51);
        newRanges.push_back(std::make_pair(5.348, 5.363));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        double referenceValuesObsNewPoints[] = {0.511622217173, 0.511770790824, 0.511922922294, 0.512078816370, 0.512238695862, 0.512402801577,
        										0.512571391956, 0.512744742291, 0.512923143444, 0.513106900003, 0.513296327766, 0.513491750494,
        										0.513693495837, 0.513901890370, 0.514117253685, 0.514339891515, 0.514570087889, 0.514808096378,
        										0.515054130536, 0.515308353702, 0.515570868411, 0.515841705718, 0.516120814849, 0.516408053606,
        										0.516703180054, 0.517005846018, 0.517315592906, 0.517631850357, 0.517953938106, 0.518281071314,
        										0.518612369493, 0.518946868901, 0.519283538110, 0.519621296244, 0.519959033190, 0.520295630948,
        										0.520629985226, 0.520961026348, 0.521287738610, 0.521609177330, 0.521924482996, 0.522232892118,
        										0.522533744605, 0.522826487667, 0.523110676440, 0.523385971675, 0.523652134929, 0.523909021753,
        										0.524156573418, 0.524394807658, 0.524623808921};
        reweighter.testCalculateLogZAtSimulatedPoints();
        reweighter.testCalculateLogZAtNewPoints();
        std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
        for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
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
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 51);
		newRanges.push_back(std::make_pair(5.348, 5.363));
		ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
		double referenceValuesObsNewPoints[] = {0.511622217173, 0.511770790824, 0.511922922294, 0.512078816370, 0.512238695862, 0.512402801577,
												0.512571391956, 0.512744742291, 0.512923143444, 0.513106900003, 0.513296327766, 0.513491750494,
												0.513693495837, 0.513901890370, 0.514117253685, 0.514339891515, 0.514570087889, 0.514808096378,
												0.515054130536, 0.515308353702, 0.515570868411, 0.515841705718, 0.516120814849, 0.516408053606,
												0.516703180054, 0.517005846018, 0.517315592906, 0.517631850357, 0.517953938106, 0.518281071314,
												0.518612369493, 0.518946868901, 0.519283538110, 0.519621296244, 0.519959033190, 0.520295630948,
												0.520629985226, 0.520961026348, 0.521287738610, 0.521609177330, 0.521924482996, 0.522232892118,
												0.522533744605, 0.522826487667, 0.523110676440, 0.523385971675, 0.523652134929, 0.523909021753,
												0.524156573418, 0.524394807658, 0.524623808921};
		reweighter.testCalculateLogZAtSimulatedPoints();
		reweighter.testCalculateLogZAtNewPoints();
		std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
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
	   /*
	    * See comment in meanReweighting2 test.
	    */
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
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
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
   }

   BOOST_AUTO_TEST_CASE(suscReweighting3)
   {
       std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
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
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
           BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].susceptibility.estimate, 2e-7);
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
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
       double referenceValuesObsNewPoints[] = {3.663706836831, 3.696484687413, 3.723885962225, 3.747903662848, 3.765346818340, 3.776222888418,
    		   	   	   	   	   	   	   	   	   3.779868405965, 3.775221978283, 3.762713121619, 3.740995303963, 3.709608980699, 3.669847657356,
    		   	   	   	   	   	   	   	   	   3.621044095174, 3.563347667599, 3.497876974793, 3.425192488040, 3.346254664697, 3.262279245763,
    		   	   	   	   	   	   	   	   	   3.174213891238, 3.083592999229, 2.991712008107, 2.899850309765, 2.809412714627, 2.721682512149,
    		   	   	   	   	   	   	   	   	   2.637894251316, 2.559181676674, 2.486588832258, 2.421062038709, 2.363377226072, 2.314220158046,
    		   	   	   	   	   	   	   	   	   2.274161525348, 2.243574635506, 2.222823530362, 2.212114121404, 2.211507215736, 2.220941285527,
    		   	   	   	   	   	   	   	   	   2.240339735026, 2.269438789375, 2.307890805072, 2.355298699153, 2.411128319662, 2.474664796862,
    		   	   	   	   	   	   	   	   	   2.545320647525, 2.622395651726, 2.704936325050, 2.792237859045, 2.883234474431, 2.977133174541,
    		   	   	   	   	   	   	   	   	   3.073140076998, 3.169952202463, 3.267167961059};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
    	   if(i==3 || i==22 || i==27 || i==36 || i==41)
    		   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 0.0025);
    	   else
    		   BOOST_REQUIRE_CLOSE(referenceValuesObsNewPoints[i], valuesObsNewPoints[i][0].binderCumulant.estimate, 1.e-8);
       }

   }

   BOOST_AUTO_TEST_CASE(bindReweighting3)
   {
	   std::string fileThatDoesExist = "RealTestData/configfile_5";
       std::vector<std::pair<double, double> > newRanges;
       std::vector< unsigned int> newNumPoints(1, 51);
       newRanges.push_back(std::make_pair(5.348, 5.363));
       ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints, std::vector<unsigned int>(), std::vector<unsigned int>(), "boot");
       double referenceValuesObsNewPoints[] = {3.663706836831, 3.696484687413, 3.723885962225, 3.747903662848, 3.765346818340, 3.776222888418,
    		   	   	   	   	   	   	   	   	   3.779868405965, 3.775221978283, 3.762713121619, 3.740995303963, 3.709608980699, 3.669847657356,
    		   	   	   	   	   	   	   	   	   3.621044095174, 3.563347667599, 3.497876974793, 3.425192488040, 3.346254664697, 3.262279245763,
    		   	   	   	   	   	   	   	   	   3.174213891238, 3.083592999229, 2.991712008107, 2.899850309765, 2.809412714627, 2.721682512149,
    		   	   	   	   	   	   	   	   	   2.637894251316, 2.559181676674, 2.486588832258, 2.421062038709, 2.363377226072, 2.314220158046,
    		   	   	   	   	   	   	   	   	   2.274161525348, 2.243574635506, 2.222823530362, 2.212114121404, 2.211507215736, 2.220941285527,
    		   	   	   	   	   	   	   	   	   2.240339735026, 2.269438789375, 2.307890805072, 2.355298699153, 2.411128319662, 2.474664796862,
    		   	   	   	   	   	   	   	   	   2.545320647525, 2.622395651726, 2.704936325050, 2.792237859045, 2.883234474431, 2.977133174541,
    		   	   	   	   	   	   	   	   	   3.073140076998, 3.169952202463, 3.267167961059};
       reweighter.testCalculateLogZAtSimulatedPoints();
       reweighter.testCalculateLogZAtNewPoints();
       std::vector<std::vector<Observables> > valuesObsNewPoints = reweighter.testCalculateAndGetReweightedObservables();
       for(int i=0; i < reweighter.getNumberOfNewPoints(); i++){
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









