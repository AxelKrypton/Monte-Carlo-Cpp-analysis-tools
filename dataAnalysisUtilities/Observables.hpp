#ifndef OBSERVABLES_HPP_
#define OBSERVABLES_HPP_

#include <map>
#include <sstream>
#include <array>
#include "EstimateAndError.hpp"
#include "DataSample.hpp"

class Parameters;

enum ErrorCalculationMethod { bootstrap = 1, jackknife };

/*
 * NOTE: In order to handle the possibility to have several estimates per moment in the Moment class
 *       and several sets of estimators per moment in the MomentsEstimators class, we use a std::multimap
 *       member. Then we cannot use the access operator[] in the standard way, i.e. both to get and to set
 *       an entry. This is related to the fact that multimap has no operator[] defined. Another thing it
 *       would be cool to have is a method that get the number of the moment and returns either a single
 *       object or a set of object (in the case several were set). Nevertheless overload based on return
 *       value is not allowed in c++. So we decided to do in this way. We have an insert method to set
 *       elements. Then we use the operator[] to get a single value (checking for this case) and we use
 *       the operator() to get a set of values (checking for this case).
 */
class Moments {
public:
	Moments(){};
	void insert(const unsigned int& whichMoment, const double& momentValue){
		moments.insert(std::pair<unsigned int, double>(whichMoment, momentValue));
	}
	double operator[](const unsigned int& whichMoment){
		std::multimap<unsigned int, double>::iterator itWhichMoment = moments.find(whichMoment);
		if(itWhichMoment == moments.end())
			throw std::out_of_range("Moments::[] accessed an invalid moment! Aborting...");
		else if(moments.count(itWhichMoment->first) > 1)
			throw std::invalid_argument("Moments::[] accessed a moment for which several values are set, NOT ALLOWED! Aborting...");
		else
			return itWhichMoment->second;
	}
	std::vector<double> operator()(const unsigned int& whichMoment){
		std::multimap<unsigned int, double>::iterator itWhichMoment = moments.find(whichMoment);
		if(itWhichMoment == moments.end())
			throw std::out_of_range("Moments::() accessed an invalid moment! Aborting...");
		else if(moments.count(itWhichMoment->first) == 1)
			throw std::invalid_argument("Moments::() accessed a moment for which only on value is set, NOT ALLOWED! Aborting...");
		else{
			std::vector<double> returnVec;
			std::multimap<unsigned int, double>::iterator itRangeWhichMoment;
			for (itRangeWhichMoment=moments.equal_range(itWhichMoment->first).first; itRangeWhichMoment!=moments.equal_range(itWhichMoment->first).second; ++itRangeWhichMoment)
				returnVec.push_back(itRangeWhichMoment->second);
			return returnVec;
		}
	}
private:
	std::multimap<unsigned int, double> moments;
};

class MomentsEstimators {
public:
	MomentsEstimators(){};
	void insert(const unsigned int& whichMoment, const DataSample& momentEstValues){
		momentsEstimators.insert(std::pair<unsigned int, DataSample>(whichMoment, momentEstValues));
	}
	DataSample operator[](const unsigned int& whichMoment){
		std::multimap<unsigned int, DataSample>::iterator itWhichMoment = momentsEstimators.find(whichMoment);
		if(itWhichMoment == momentsEstimators.end())
			throw std::out_of_range("MomentsEstimators::[] accessed an invalid moment! Aborting...");
		else if(momentsEstimators.count(itWhichMoment->first) > 1)
			throw std::invalid_argument("MomentsEstimators::[] accessed a moment for which several values are set, NOT ALLOWED! Aborting...");
		else
			return itWhichMoment->second;
	}
	std::vector<DataSample> operator[](const std::initializer_list<unsigned int>& whichMoments){
		std::vector<DataSample> selectedMoments;
		for(unsigned int i: whichMoments){
			DataSample tmp = (*this)[i];
			selectedMoments.push_back(tmp);
		}
		return selectedMoments;
	}

	std::vector<DataSample> operator()(const unsigned int& whichMoment){
		std::multimap<unsigned int, DataSample>::iterator itWhichMoment = momentsEstimators.find(whichMoment);
		if(itWhichMoment == momentsEstimators.end())
			throw std::out_of_range("MomentsEstimators::() accessed an invalid moment! Aborting...");
		else if(momentsEstimators.count(itWhichMoment->first) == 1)
			throw std::invalid_argument("MomentsEstimators::() accessed a moment for which only on value is set, NOT ALLOWED! Aborting...");
		else{
			std::vector<DataSample> returnVec;
			std::multimap<unsigned int, DataSample>::iterator itRangeWhichMoment;
			for (itRangeWhichMoment=momentsEstimators.equal_range(itWhichMoment->first).first; itRangeWhichMoment!=momentsEstimators.equal_range(itWhichMoment->first).second; ++itRangeWhichMoment)
				returnVec.push_back(itRangeWhichMoment->second);
			return returnVec;
		}
	}

//	DataSample& operator[](const unsigned int& whichMoment){ return momentsEstimators[whichMoment]; };
//	MomentsEstimators operator[](const std::initializer_list<unsigned int>& whichMoments){
//		MomentsEstimators selectedMoments;
//		for(unsigned int i: whichMoments)
//			selectedMoments[i] = momentsEstimators.at(i);
//		return selectedMoments;
//	};
//	std::vector<DataSample> operator()(const std::initializer_list<unsigned int>& whichMoments){
//		std::vector<DataSample> selectedMoments;
//		for(unsigned int i: whichMoments)
//			selectedMoments.push_back(momentsEstimators.at(i));
//		return selectedMoments;
//	};

private:
	std::multimap<unsigned int, DataSample> momentsEstimators;
};

/*
 * NOTE: In order to make a function return a pointer to function one must use an horrible syntax.
 *       For example, if you want that a function taking a double return a pointer to a function that
 *       takes one int and one char and returns a bool, you should write
 *
 *          bool (*getFunction(double a))(int, char);
 *
 *       and this is quite not readable. That's why one defines a new type and uses it like
 *
 *          typedef bool (*functionSignature)(int, char);
 *          functionSignature GetFunction(double a);
 *
 * TODO: So far the error method is not a private member of the class, since for the raw data only Jackknife is used!
 *       Think whether put it as private member and in case do it.
 */

typedef DataSample (*functionForEstimators)(std::vector<DataSample>);
typedef double (*functionForObservable)(Moments);

class ObservableAbstract {
public:
	ObservableAbstract(bool isMeanKnownToBeZero);
	virtual ~ObservableAbstract(){};
	EstimateAndError getValueAndError();
protected:
	//Calculation from raw data
	void calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters);
	//Calculation for Reweighting
	void calculateAndSetValueAndError(Moments moments, MomentsEstimators estimators, ErrorCalculationMethod errorMethod);
	std::string observableName;
	bool isMeanZero;
	EstimateAndError observableEstimateAndError;
private:
	std::vector<DataSample> getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters);
	std::vector<DataSample> calculateNeededMomentsPerDataPoint(DataSample& dataSample);
	//Virtual method that must be provided by children classes
	virtual Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters) = 0;
	virtual void printCorrectBinningInformation(const Parameters& parameters) = 0;
	virtual functionForEstimators getFunctionToBeAppliedToEstimators() = 0;
	virtual functionForObservable getFunctionToCalculateObservable() = 0;
	virtual std::initializer_list<unsigned int> getNeededMoments() = 0;
};


class Mean : public ObservableAbstract {
public:
	Mean() = delete;
	Mean(DataSample& dataSample, Parameters parameters);
	Mean(Moments moments, MomentsEstimators estimators, bool isMeanKnownToBeZero, ErrorCalculationMethod errorMethod);
	static const std::initializer_list<unsigned int> neededMoments;
	static const std::initializer_list<unsigned int> neededMomentsWithZeroMean;
	static const std::string observableName;
private:
	Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters);
	void printCorrectBinningInformation(const Parameters& parameters);
	functionForEstimators getFunctionToBeAppliedToEstimators();
	functionForObservable getFunctionToCalculateObservable();
	std::initializer_list<unsigned int> getNeededMoments();
};


class Variance : public ObservableAbstract {
public:
	Variance() = delete;
	Variance(DataSample& dataSample, Parameters parameters);
	Variance(Moments moments, MomentsEstimators estimators, bool isMeanKnownToBeZero, ErrorCalculationMethod errorMethod);
	static constexpr std::initializer_list<unsigned int> neededMoments = {1, 2};
	static constexpr std::initializer_list<unsigned int> neededMomentsWithZeroMean = { {2} };
	static const std::string observableName;
private:
	Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters);
	void printCorrectBinningInformation(const Parameters& parameters);
	functionForEstimators getFunctionToBeAppliedToEstimators();
	functionForObservable getFunctionToCalculateObservable();
	std::initializer_list<unsigned int> getNeededMoments();
};


class Skewness : public ObservableAbstract {
public:
	Skewness() = delete;
	Skewness(DataSample& dataSample, Parameters parameters);
	Skewness(Moments moments, MomentsEstimators estimators, bool isMeanKnownToBeZero, ErrorCalculationMethod errorMethod);
	static constexpr std::initializer_list<unsigned int> neededMoments = {1, 2, 3};
	static constexpr std::initializer_list<unsigned int> neededMomentsWithZeroMean = {2, 3};
	static const std::string observableName;
private:
	Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters);
	void printCorrectBinningInformation(const Parameters& parameters);
	functionForEstimators getFunctionToBeAppliedToEstimators();
	functionForObservable getFunctionToCalculateObservable();
	std::initializer_list<unsigned int> getNeededMoments();
};


class BinderCumulant : public ObservableAbstract {
public:
	BinderCumulant() = delete;
	BinderCumulant(DataSample& dataSample, Parameters parameters);
	BinderCumulant(Moments moments, MomentsEstimators estimators, bool isMeanKnownToBeZero, ErrorCalculationMethod errorMethod);
	static constexpr std::initializer_list<unsigned int> neededMoments = {1, 2, 3, 4};
	static constexpr std::initializer_list<unsigned int> neededMomentsWithZeroMean = {2, 4};
	static const std::string observableName;
private:
	Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters);
	void printCorrectBinningInformation(const Parameters& parameters);
	functionForEstimators getFunctionToBeAppliedToEstimators();
	functionForObservable getFunctionToCalculateObservable();
	std::initializer_list<unsigned int> getNeededMoments();
};

/*
 * TODO: The following class should be a container for the quantities above and it should allow the possibility to set only some.
 *       One idea could be to use the access operator with the observable name as argument to recover values and do something
 *       when it is asked for an unset quantity (implement operator[] and function at or something like that)
 */

class Observables
{
public:
//    Observables() : mean(0.0, 0.0), susceptibility(0.0, 0.0),
//                    skewness(0.0, 0.0), binderCumulant(0.0, 0.0)
	Observables() : mean(NAN, NAN), susceptibility(NAN, NAN),
                    skewness(NAN, NAN), binderCumulant(NAN, NAN)
										{
												observableNames.push_back("mean" );
												observableNames.push_back("susc");
												observableNames.push_back("skew");
												observableNames.push_back("binder");
										}
										
		std::string getMetaInformation()
		{
			std::string metaInfos = "";
			for (uint index = 0; index < observableNames.size(); index ++)
			{
				metaInfos += observableNames[index] + "\t\terror\t\t";
			}
			return metaInfos;
		}
		
		std::string getObservablesAsString()
		{
			std::stringstream values;
			values << std::scientific;
			values << mean.estimate << "\t" << mean.error<< "\t" ;
			values << susceptibility.estimate << "\t" << susceptibility.error << "\t";
			values << skewness.estimate << "\t" << skewness.error << "\t";
			values << binderCumulant.estimate << "\t" << binderCumulant.error ;
			return values.str();
		}

    /*
     * Here in the following the observables we deal with around in the
     * library, together with their definition (thinking to a sample set X
     * with mean mu, x1,x2,x3,x4 are the 1st,2nd,3rd,4th moment of the sample)
     */
    EstimateAndError mean;            // <x>                            --->  x1
    EstimateAndError susceptibility;  // <(x-mu)^2>                     --->  x2-x1^2
    EstimateAndError skewness;        // <(x-mu)^3> / <(x-mu)^2>^(3/2)  --->  (x3-3x2*x1+2*x1^3)/(x2-x1^2)^(3/2)
    EstimateAndError binderCumulant;  // <(x-mu)^4> / <(x-mu)^2>^2      --->  (x4-4*x3*x1+6*x2*x1^2-3*x1^4)/(x2-x1^2)^2
    
    //TODO: Implement functions to calculate here the observables given either the DataSample or
    //      the moments and the estimators (the latter needed in Reweighting). The idea would be to
    //      have an object to which delegate the calculation of the observables all together: it could
    //      use the object in DataSampleAnalyzer that should be then local to the Observables.cpp file!

    std::vector<std::string> observableNames;
};






















#endif /* OBSERVABLES_HPP_ */
