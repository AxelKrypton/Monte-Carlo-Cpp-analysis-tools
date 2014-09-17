#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Reweighter
#include <boost/test/unit_test.hpp>

#include "Reweighter.hpp"

BOOST_AUTO_TEST_SUITE( build)

	BOOST_AUTO_TEST_CASE(build1)
	{
		BOOST_REQUIRE_THROW(Reweighter reweighter, std::invalid_argument);
	}

	BOOST_AUTO_TEST_CASE(build2)
	{
		std::string fileThatDoesNotExist = "fileThatShouldNotBe";
		std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_3"; //wrong structure
		std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_4"; //correct structure but with two identical set of parameters
		std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_5"; //correct structure but with two identical filenames
		std::string fileThatDoesExistButWrong4 = "GeneralTestFiles/wrong_configfile_6"; //correct structure but with not existing file inside
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesNotExist), std::exception);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong1), std::exception);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong2), std::exception);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong3), std::exception);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong4), std::exception);
	}

	BOOST_AUTO_TEST_CASE(build3)
	{
		std::string fileThatDoesExistButWrong1 = "GeneralTestFiles/wrong_configfile_7"; //correct structure but with datafile with wrong number of columns
		std::string fileThatDoesExistButWrong2 = "GeneralTestFiles/wrong_configfile_8"; //correct structure but with different parameters name in two lines
		std::string fileThatDoesExistButWrong3 = "GeneralTestFiles/wrong_configfile_9"; //correct structure but with logZ as parameter
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong1), std::logic_error);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong2), std::invalid_argument);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExistButWrong3), std::logic_error);
	}

	BOOST_AUTO_TEST_CASE(build4)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter reweighter(fileThatDoesExist);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, 0.0), std::range_error);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, -1.), std::range_error);
	}

	BOOST_AUTO_TEST_CASE(build5)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter reweighter(fileThatDoesExist);
		BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist));
	}

	BOOST_AUTO_TEST_CASE(build6)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints;
		newRanges.push_back(std::make_pair(4.2, 4.6));
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newRanges.push_back(std::make_pair(0.8, 1.2));
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(2);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(3);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.push_back(0);
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.back() = 1;
		BOOST_REQUIRE_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints), std::invalid_argument);
		newNumPoints.back() = 5;
		Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints);
		BOOST_REQUIRE_NO_THROW(Reweighter reweighter(fileThatDoesExist, newRanges, newNumPoints));
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getters)

	BOOST_AUTO_TEST_CASE(getters1)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter* reweighter = new Reweighter(fileThatDoesExist);
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
		BOOST_REQUIRE(refParameters == reweighter->getValuesOfSimulationParameters());
	}

	BOOST_AUTO_TEST_CASE(getters2)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist, newRanges, newNumPoints);
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
		BOOST_REQUIRE(refPoints == reweighter->getValuesOfNewParameters());
	}

	BOOST_AUTO_TEST_CASE(getters3)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist, newRanges, newNumPoints);
		unsigned int numberOfNewPoints = 64;
		BOOST_REQUIRE_EQUAL(numberOfNewPoints, reweighter->getNumberOfNewPoints());
	}

	BOOST_AUTO_TEST_CASE(getters4)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter reweighter(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(reweighter.getLogZAtSimulatedPoints().size(), 0);
	}

	BOOST_AUTO_TEST_CASE(getters5)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter reweighter(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(reweighter.getLogZAtNewPoints().size(), 0);
	}

	BOOST_AUTO_TEST_CASE(getters6)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		Reweighter reweighter(fileThatDoesExist);
		BOOST_REQUIRE_EQUAL(reweighter.getPrecisionToCalculateLogZ(), 1.e-7);
		Reweighter reweighter2(fileThatDoesExist, 0.001);
		BOOST_REQUIRE_EQUAL(reweighter2.getPrecisionToCalculateLogZ(), 0.001);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(setters)

	BOOST_AUTO_TEST_CASE(setters1)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist);
		BOOST_REQUIRE_THROW(reweighter->setNewRangesOfParameters(newRanges), std::invalid_argument);
		delete reweighter;
		reweighter = new Reweighter(fileThatDoesExist, newRanges, newNumPoints);
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
	}

	BOOST_AUTO_TEST_CASE(setters2)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist);
        BOOST_REQUIRE_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
		delete reweighter;
		reweighter = new Reweighter(fileThatDoesExist, newRanges, newNumPoints);
        newNumPoints.pop_back();
        BOOST_REQUIRE_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints), std::invalid_argument);
        newNumPoints.push_back(4);
		newNumPoints[1] = 5;
        BOOST_REQUIRE_NO_THROW(reweighter->setNewNumberOfPointsOfParameters(newNumPoints));
		BOOST_REQUIRE_EQUAL(reweighter->getNumberOfNewPoints(), 80);
	}

	BOOST_AUTO_TEST_CASE(setters3)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 2);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist);
		BOOST_REQUIRE_NO_THROW(reweighter->setNewParameters(newRanges, newNumPoints));
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
		BOOST_REQUIRE(refPoints == reweighter->getValuesOfNewParameters());
		BOOST_REQUIRE_EQUAL(reweighter->getNumberOfNewPoints(), 8);
	}

	BOOST_AUTO_TEST_CASE(setters4)
	{
		std::string fileThatDoesExist = "GeneralTestFiles/simulationDataContainer.configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(3, 4);
		newRanges.push_back(std::make_pair(4.2, 4.6));
		newRanges.push_back(std::make_pair(0.8, 1.2));
		newRanges.push_back(std::make_pair(-1.2e12, -1.6e12));
		Reweighter* reweighter = new Reweighter(fileThatDoesExist, newRanges, newNumPoints, 0.1);
		BOOST_REQUIRE_THROW(reweighter->setPrecisionToCalculateLogZ(-3.e-10), std::range_error);
		BOOST_REQUIRE_NO_THROW(reweighter->setPrecisionToCalculateLogZ(1.e-10));
		BOOST_REQUIRE_EQUAL(reweighter->getPrecisionToCalculateLogZ(), 1.e-10);
	}

BOOST_AUTO_TEST_SUITE_END()


class Reweighter_test : public Reweighter{
public:
	Reweighter_test(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
		     std::vector<unsigned int>  newNumberOfPointsOfParametersIn, double precisionToCalculateLogZ = 1.e-7) :
		    	 Reweighter(configurationFileIn, newRangesOfParametersIn, newNumberOfPointsOfParametersIn, precisionToCalculateLogZ) {}
	std::vector<double> testCalculateLogZAtNewPoints(){return calculateLogZAtNewPoints(getValuesOfNewParameters());}
	std::vector<double> testCalculateLogZAtSimulatedPoints(){calculateLogZAtSimulatedPoints(); return getLogZAtSimulatedPoints();}
};

BOOST_AUTO_TEST_SUITE(reweight)

	BOOST_AUTO_TEST_CASE(reweight1)
	{
		std::string fileThatDoesExist = "RealTestData/configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
		Reweighter_test reweighter(fileThatDoesExist, newRanges, newNumPoints);
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
		std::string fileThatDoesExist = "RealTestData/configfile";
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints(1, 30);
		newRanges.push_back(std::make_pair(5.348, 5.3509));
		Reweighter_test reweighter(fileThatDoesExist, newRanges, newNumPoints);
		//for(size_t i=0; i<reweighter.getValuesOfNewParameters().size(); i++)
		//	std::cout << reweighter.getValuesOfNewParameters()[i][0] << std::endl;
		double referenceLogZAtSimulatedPoints[] = {1.305544243113, -38.693372513854, -98.366081552663};
		std::vector<double> simulatedLogZ = reweighter.testCalculateLogZAtSimulatedPoints();
		for(size_t i=0; i < reweighter.getValuesOfSimulationParameters().size(); i++)
			BOOST_REQUIRE_CLOSE(referenceLogZAtSimulatedPoints[i], simulatedLogZ[i], 1.e-8);
	}

BOOST_AUTO_TEST_SUITE_END()


















