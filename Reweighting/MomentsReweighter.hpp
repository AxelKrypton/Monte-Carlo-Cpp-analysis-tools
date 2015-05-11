#ifndef MOMENTSREWEIGHTER_H_
#define MOMENTSREWEIGHTER_H_

#include "SimulationDataContainer.hpp"
#include "MomentsReweighterHelper.hpp"
#include "../dataAnalysisUtilities/dataAnalysisUtilities.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

/*
 * The idea underlying the implementation of the MomentsReweighter class is that the user construct a
 * MomentsReweighter object, sets the values of new parameters (ranges and number of points) either in
 * the constructor or using the setter setNewParameters, and then uses methods such as
 * getLogZAtNewPoints to get the result of the calculation. This means that the user has not
 * to invoke a method to start the calculation itself.
 *
 * Given what written above, one has to consider that to calculate the logZ at the new points
 * the value of logZ at the simulated points is needed and, of course, one must have the values
 * of the new points. There are basically two scenarios:
 *  - In the constructor the new points are not set and they are set afterwards with the setter.
 *    This means that the constructor will calculate logZ only at the simulated points, while logZ
 *    at the new points will be calculated in the setter.
 *  - In the constructor everything is set. Thus both the logZ at the simulated points and logZ
 *    at the new points will be calculated by the constructor.
 *
 * NOTE: Actually there are three kind of setters: one to set only the ranges, one to set only
 *       the number of new points, and one to set both. The first two are thought to be used
 *       only to re-set the values: one cannot instantiate an object without giving the new parameters
 *       and then call first on single setter and then the other. For this case, the complete
 *       setter should be used. However, all these three setters will calculate again the values
 *       of logZ at the new points.
 *
 * Now, it should be clear that, to make this class more user-friendly, some functionalities in
 * principle different have been grouped in single methods (like the setting operation and the
 * calculation). The problem is that in the tests the single functionality should be tested. Hence
 * one would like to test the setter itself, without carrying out any calculation in it. The calculation
 * should to be tested in a separate test case. This is the reason why we proceed implementing a
 * MomentsReweighterAbstract class that has the setters as pure virtual functions. In the derived classes
 * MomentsReweighter and MomentsReweighterTest these setters will be properly defined, i.e. in the MomentsReweighter the
 * calculation of logZ at new points will be done and in ReweighterTest not. About the calculation
 * in the constructor, one would like to do something similar, but we know it is not possible to have
 * a virtual constructor. Thus we will not put this calculation in the ReweighterAbstract class
 * constructor, but only in the real MomentsReweighter class.
 *
 * NOTE: Since abstract classes cannot be used to instantiate objects, it could be misleading to
 *       have a public constructor. Actually, it seems to make no difference to have a public or
 *       a private constructor in an abstract class, we will make them protected (read this reference
 *       http://stackoverflow.com/questions/1363147/is-a-public-constructor-in-an-abstract-class-a-codesmell
 *       for more details).
 *
 * REMARK: For each pure virtual method, the base class can still have an implementation of such a method
 *         that can be explicitly called in the children with the scope resolution operator. In this way,
 *         derived classes can call this sort of default implementation. Pure virtual (=0) means that
 *         derived classes must provide an implementation, not that the base class can not provide an implementation!
 *
 *
 * It is also worth to spend some words about the reweighting procedure of the observables. To get the value
 * of the observables at the new points, basically the Eq.(8.39) of Barkema's book has to be implemented.
 * Thus one has to know the values of each observable at each step of the Monte Carlo, or more precisely for
 * each given configuration. So it is not sufficient to know the value of the logZ at the simulated points
 * and the value of each observable at each simulated points (i.e. only the mean over the Monte Carlo history)
 * in order to calculate the values of the observables at the new points. This slightly complicates the
 * implementation. It is indeed rather natural to make the MomentsReweighter class responsible also for calculating
 * the values of the moments of the observables, though in this way the "Single responsability" principle is partially
 * violated (even if actually one could think as responsability the momentss reweighting).
 * The reason why it is quite easy to include here the observable moments reweighting procedure is that one can
 * easily give the observables data to the class, without changing almost anything. It is enough to give
 * them as columns after the conjugated quantities in each simulation data file. Since the number of conjugated
 * quantities is known, being it equal to the number of reweighting parameters, one can easily deduce the
 * number of observables that have to be reweighted.
 *
 * TODO: Think if it makes sense (and if it is possible...) to make the test class friend of the
 *       ReweighterAbstract class to have access to private member for testing purposes.
 */


class MomentsReweighterAbstract {
    friend class MomentsReweighterHelper;
public:
    virtual ~MomentsReweighterAbstract() {}
	//Getters
	std::vector<std::vector<double> > getValuesOfSimulationParameters();
	std::vector<std::vector<double> > getValuesOfNewParameters();
	int getNumberOfNewPoints();
	std::vector<double> getLogZAtSimulatedPoints();
	std::vector<double> getLogZAtNewPoints();
	double getPrecisionToCalculateLogZ();
	std::vector<std::vector<Moments> > getMomentsAtNewPoints();
	std::vector<std::vector<MomentsEstimators> > getMomentsEstimatorsAtNewPoints();
	//Setters
    virtual void setPrecisionToCalculateLogZ(double precisionToCalculateLogZ) = 0;
    virtual void setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn) = 0;
    virtual void setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn) = 0;
    virtual void setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                                  std::vector<unsigned int> newNumberOfPointsOfParametersIn) = 0;
protected:
    MomentsReweighterAbstract() = delete;
    MomentsReweighterAbstract(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformationIn);

    //Some of the following method could be static functions in the .cpp file but are here for testing purposes
    void calculateAndSetLogZAtSimulatedPoints();
    void calculateAndSetLogZAtNewPoints();
    void calculateAndSetReweightedMomentsAndMomentsEstimators();
    void prepareObservablesBeforeReweighting(std::vector<double> &);
    void restoreObservablesAfterReweighting(std::vector<double> minimumOfEachObservable,
                                            std::vector<std::vector<double> > *reweightedObservablesFromRawData,
                                            std::valarray<std::vector<std::vector<double> > > *jackknifePartialPred);
    std::vector<std::vector<double> > calculateReweightedObservableValues(bool useUncorrData = false,
                                                                          const int entryToBeLeftOut = -1,
                                                                          std::vector<double> *logZAtSimulationPointToBeUsed = NULL,
                                                                          std::vector<double> *logZAtNewPointsToBeUsed = NULL);
    std::vector<double> calculateLogZAtSimulatedPoints(bool useUncorrData, const int entryToBeLeftOut,
                                                       std::vector<double> *logZAtSimulationPointToStartFrom = NULL, bool printUserInfo = false);
    std::vector<double> calculateLogZAtNewPoints(std::vector<std::vector<double> > valuesOfParametersAtWhichLogZIsCalculated,
                                                 bool useUncorrData = false, const int entryToBeLeftOut = -1,
                                                 std::vector<double> *logZAtSimulationPointToBeUsed = NULL);
    //This getter again only for testing reason
    SimulationDataContainer getSimulationDataContainer(bool raw = true);
    //Method used in calculateAndGetReweightedObservables to select data to calculate observables and errors and to set them
	void extractAndSetReweightedMomentsAndMomentsEstimators(const std::vector<std::vector<double> >& reweightedObservablesFromRawData,
															const std::valarray<std::vector<std::vector<double> > >& estimatorsForErrorsCalculation);

private:
    //Method in which "valuesOfNewParameters" is filled and some checks are done
    void calculateNewPoints();

    //Members
    MomentsReweighterHelper momentsReweighterHelper;
    std::vector<std::string> reweightingParameterNames;

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
	std::vector<std::vector<Moments> > momentsAtNewPoints;
	std::vector<std::vector<MomentsEstimators> > momentsEstimatorsAtNewPoints;

	/*
	 * The new points are so far WITHIN the given range without counting the boundaries.
	 * They are at the same distance one from each other.
	 *
	 * NOTE: the following two elements could be unified in c++11 with std::tuple
	 */
	std::vector<std::pair<double, double> >  newRangesOfParameters;
	std::vector<unsigned int>  newNumberOfPointsOfParameters;

	double precisionOfIterativeProcedureToCalculateLogZ;
};


class MomentsReweighter : public MomentsReweighterAbstract{
public:
    MomentsReweighter() = delete;

    explicit MomentsReweighter(RawDataForReweightingAndMetainformation rawDataForReweightingAndMetainformationIn)
    	: MomentsReweighterAbstract(rawDataForReweightingAndMetainformationIn)
    {
		calculateAndSetLogZAtSimulatedPoints();
		calculateAndSetLogZAtNewPoints();
		calculateAndSetReweightedMomentsAndMomentsEstimators();
    }

    //Setters
    void setNewRangesOfParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn){
        MomentsReweighterAbstract::setNewRangesOfParameters(newRangesOfParametersIn);
        calculateAndSetLogZAtNewPoints();
        calculateAndSetReweightedMomentsAndMomentsEstimators();
    }

    void setNewNumberOfPointsOfParameters(std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        MomentsReweighterAbstract::setNewNumberOfPointsOfParameters(newNumberOfPointsOfParametersIn);
        calculateAndSetLogZAtNewPoints();
        calculateAndSetReweightedMomentsAndMomentsEstimators();
    }

    void setNewParameters(std::vector<std::pair<double, double> >  newRangesOfParametersIn,
                          std::vector<unsigned int> newNumberOfPointsOfParametersIn){
        MomentsReweighterAbstract::setNewParameters(newRangesOfParametersIn, newNumberOfPointsOfParametersIn);
        calculateAndSetLogZAtNewPoints();
        calculateAndSetReweightedMomentsAndMomentsEstimators();
    }

    void setPrecisionToCalculateLogZ(double precisionToCalculateLogZ){
    	MomentsReweighterAbstract::setPrecisionToCalculateLogZ(precisionToCalculateLogZ);
		calculateAndSetLogZAtSimulatedPoints();
		calculateAndSetLogZAtNewPoints();
		calculateAndSetReweightedMomentsAndMomentsEstimators();
    }
};



#endif /* MOMENTSREWEIGHTER_H_ */
