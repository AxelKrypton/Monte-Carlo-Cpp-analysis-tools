#ifndef REWEIGHTER_H_
#define REWEIGHTER_H_

#include "SimulationDataContainer.hpp"

class Reweighter {
public:
	Reweighter();
	Reweighter(std::string configurationFileIn);
	Reweighter(std::string configurationFileIn, std::vector<std::pair<double, double> >  newRangesOfParametersIn,
			     std::vector<unsigned int>  newNumberOfPointsOfParametersIn);
	//Getters
	std::vector<std::vector<double> > getValuesOfSimulationParameters();
	std::vector<std::vector<double> > getValuesOfNewParameters();
	double getNumberOfNewPoints();
	//Setters
	void setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn);
	void setNewNumberOfPoints(std::vector<unsigned int> newNumberOfPointsOfParametersIn);
	void setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
			                 std::vector<unsigned int> newNumberOfPointsOfParametersIn);
	//Other functionalities


private:
	/*
	 * Note: The following function is basically the second constructor above. We put it here because it is also the first part
	 *       of the third constructor. In C one constructor cannot call another constructor (in c++11 one would use delegating constructors).
	 */
	void generalInitialization();
	void calculateNewPoints();

	std::string configurationFile;
	std::vector<std::string> reweightingParameterNames;
	SimulationDataContainer simulationDataContainer;

	/*
	 * Here in the following objects the order depending on which parameters are
	 * considered ordered is ALPHABETICAL! This means for example that if in the
	 * configuration file there is in the first line
	 *   input_filename    chem_pot 0.5 beta 4.3 magnetic_field 8.7
	 * then we will have
	 *  -    reweightingParameterNames[3] = {"beta", "chem_pot", "magnetic_field"};
	 *  - valuesOfSimulationParameters[0] = {4.3, 0.5, 8.7};
	 *
	 * Furthermore, the order of points in logZAtSimulatedPoints and in logZAtNewPoints
	 * is the same of that in the outermost vector in valuesOfSimulationParameters and
	 * valuesOfNewParameters, respectively. The order of simulated points is that given
	 * in the configuration file, while that of new points is determined varying more
	 * quickly the last parameter. For example, if we have "beta" and "chem_pot" as
	 * parameters and the new ranges are between 4 and 5 for beta and between 1 and 2
	 * for chemical potential (boundary included), each with 3 new points, thus:
	 *     beta 4.0   chem_pot 1.0
	 *     beta 4.0   chem_pot 1.5
	 *     beta 4.0   chem_pot 2.0
	 *     beta 4.5   chem_pot 1.0
	 *     beta 4.5   chem_pot 1.5
	 *     beta 4.5   chem_pot 2.0
	 *     beta 5.0   chem_pot 1.0
	 *     beta 5.0   chem_pot 1.5
	 *     beta 5.0   chem_pot 2.0
	 */
	std::vector<std::vector<double> >  valuesOfSimulationParameters;
	std::vector<std::vector<double> >  valuesOfNewParameters;
	std::vector<double> logZAtSimulatedPoints;
	std::vector<double> logZAtNewPoints;

	//Todo: think whether keep these two objects as members or not
	//Note: the following two elements could be unified in c++11 with std::tuple
	/*
	 * The new points are so far WITHIN the given range without counting the boundaries.
	 * They are at the same distance one from each other.
	 */
	std::vector<std::pair<double, double> >  newRangesOfParameters;
	std::vector<unsigned int>  newNumberOfPointsOfParameters;
};


#endif /* REWEIGHTER_H_ */
