#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter_objects
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "Reweighter.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp" // For doublePreciosionInPercent

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
};


BOOST_AUTO_TEST_SUITE( build)

	BOOST_AUTO_TEST_CASE(build1)
	{
        BOOST_REQUIRE_THROW(ReweighterTest reweighter, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesNotExist = "fileThatShouldNotBe";
		std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_3"; //wrong structure
		std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_4"; //correct structure but with two identical set of parameters
		std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_5"; //correct structure but with two identical filenames
		std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_6"; //correct structure but with not existing file inside
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesNotExist), std::exception);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong1), std::exception);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong2), std::exception);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong3), std::exception);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong4), std::exception);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_7"; //correct structure but with datafile with wrong number of columns
		std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_8"; //correct structure but with different parameters name in two lines
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong1), std::logic_error);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExistButWrong2), std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_1";
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, 0.0), std::range_error);
        BOOST_REQUIRE_THROW(ReweighterTest reweighter(fileThatDoesExist, -1.), std::range_error);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
        std::string fileThatDoesExist1 = "GeneralTestFiles/simulationDataContainer.configfile_1";
        std::string fileThatDoesExist2 = "GeneralTestFiles/simulationDataContainer.configfile_2";
        std::string fileThatDoesExist3 = "GeneralTestFiles/simulationDataContainer.configfile_3";
        BOOST_REQUIRE_NO_THROW(ReweighterTest reweighter(fileThatDoesExist1));
        BOOST_REQUIRE_NO_THROW(ReweighterTest reweighter(fileThatDoesExist2));
        BOOST_REQUIRE_NO_THROW(ReweighterTest reweighter(fileThatDoesExist3));
	}

	BOOST_AUTO_TEST_CASE(build6)
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

    BOOST_AUTO_TEST_CASE(build7)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_2";
        ReweighterTest reweighter(fileThatDoesExist);
        double referenceLogZAtSimulatedPoints[3] = {3.14, 6.28, 9.42};
        BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 3);
        for(size_t i=0; i<reweighter.getLogZAtSimulatedPoints().size(); i++)
            BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints()[i], referenceLogZAtSimulatedPoints[i]);
    }

    BOOST_AUTO_TEST_CASE(build8)
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
        ReweighterTest reweighter(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(reweighter.getLogZAtNewPoints().size(), 0);
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


BOOST_AUTO_TEST_SUITE(functionalities)

    BOOST_AUTO_TEST_CASE(writeNewConfigFile1)
    {
        std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile_3";
        ReweighterTest reweighter(fileThatDoesExist);
        std::string outputFileName = fileThatDoesExist + "_new";
        reweighter.writeNewConfigurationFileWithLogZ();
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
    }

    BOOST_AUTO_TEST_CASE(writeNewConfigFile2)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest* reweighter = new ReweighterTest(fileThatDoesExist, newRanges, newNumPoints);
        std::vector<double> simulatedLogZ = reweighter->testCalculateLogZAtSimulatedPoints();
        std::string outputFileName = "testWritingConfigFile";
        reweighter->writeNewConfigurationFileWithLogZ(outputFileName);
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        delete reweighter;
        reweighter = new ReweighterTest(outputFileName);
        for(size_t i=0; i<simulatedLogZ.size(); i++)
            BOOST_REQUIRE_CLOSE(reweighter->getLogZAtSimulatedPoints()[i], simulatedLogZ[i], doublePrecisionInPercent);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
        delete reweighter;
    }

    BOOST_AUTO_TEST_CASE(writeNewPointsToFile)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_1";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        std::string outputFileName = "testWritingNewPoints";
        std::ofstream outputFile;
        outputFile.open(outputFileName.c_str());
        outputFile << "Hello!";
        outputFile.close();
        BOOST_REQUIRE_MESSAGE(boost::filesystem::exists( outputFileName ) == true, "Something bad happened creating a file!");
        BOOST_REQUIRE_THROW(reweighter.writeNewPointsToFileWithLogZ(outputFileName), std::invalid_argument);
        boost::filesystem::remove(outputFileName);
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtNewPoints();
        reweighter.writeNewPointsToFileWithLogZ(outputFileName);
        BOOST_REQUIRE_EQUAL(boost::filesystem::exists( outputFileName ), true);
        if(boost::filesystem::exists(outputFileName))
            boost::filesystem::remove(outputFileName);
    }


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(reweight)

	BOOST_AUTO_TEST_CASE(reweight1)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
		//for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
		//	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
		double referenceLogZAtNewPoints[] = {141.695776537821,	139.673539281497, 137.651598792759, 135.629957919910, 133.608619564411,
											  131.587586681050, 129.566862277888, 127.546449416238, 125.526351210336, 123.506570827080,
											  121.487111485644, 119.467976456805, 117.449169062325, 115.430692674084, 113.412550713185,
											  111.394746648728, 109.377283996608, 107.360166318050, 105.343397218084, 103.326980343670,
											  101.310919381828,  99.295218057480,  97.279880131209,  95.264909396662,  93.250309677931,
											   91.236084826628,  89.222238718882,  87.208775251947,  85.195698340792,  83.183011914383};
        std::vector<double> newLogZ = reweighter.testCalculateLogZAtNewPoints();
		for(int i=0; i < reweighter.getNumberOfNewPoints(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtNewPoints[i], newLogZ[i], 1.e-8);
	}

	BOOST_AUTO_TEST_CASE(reweight2)
	{
        std::string fileThatDoesExist = "RealTestData/configfile_1";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
		//for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
		//	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
		double referenceLogZAtSimulatedPoints[] = {1.305544243113, -38.693372513854, -98.366081552663};
		std::vector<double> simulatedLogZ = reweighter.testCalculateLogZAtSimulatedPoints();
		for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
	}

    BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( reweight3, 2 )
    BOOST_AUTO_TEST_CASE(reweight3)
    {
        std::string fileThatDoesExist = "RealTestData/configfile_2";
        std::vector<std::pair<double, double> > newRanges;
        std::vector< unsigned int> newNumPoints(1, 30);
        newRanges.push_back(std::make_pair(5.348, 5.3509));
        ReweighterTest reweighter(fileThatDoesExist, newRanges, newNumPoints);
        //for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
        //	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
        double referenceLogZAtSimulatedPoints[] = {1.305544243113, -38.693372513854, -98.366081552663};
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
        double referenceLogZAtSimulatedPoints[] = {1.305544243113, -38.693372513854, -98.366081552663};
        double referenceLogZAtNewPoints[] = {142.441104343211, 140.415043785678, 138.389221202136,
                                             136.363638116277, 134.338296081536, 132.313196681783,
                                             130.288341531890, 128.263732278401, 126.239370600000,
                                             124.215258208119, 122.191396847539, 120.167788296795,
                                             118.144434368740, 116.121336910983, 114.098497806397,
                                             112.075918973426, 110.053602366528, 108.031549976496,
                                             106.009763830825, 103.988245993877, 101.966998567182,
                                             99.946023689575, 97.925323537411, 95.904900324531,
                                             93.884756302372, 91.864893759899, 89.845315023592,
                                             87.826022457196, 85.807018461570, 83.788305474381};
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









