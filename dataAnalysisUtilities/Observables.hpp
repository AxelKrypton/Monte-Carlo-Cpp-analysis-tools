#ifndef OBSERVABLES_HPP_
#define OBSERVABLES_HPP_

#include <map>
#include <sstream>
#include <array>
#include "EstimateAndError.hpp"
#include "DataSample.hpp"

class Parameters;

enum ErrorCalculationMethod { bootstrap = 1, jackknife };

class Moments {
public:
	Moments(){};
	double at(int whichMoment){
		if(moments.find(whichMoment) == moments.end())
				throw std::out_of_range("Moments::[] accessed an invalid moment! Aborting...");
			else
				return moments[whichMoment];
	};
	double& operator[](int whichMoment){ return moments[whichMoment]; };
	Moments operator[](std::initializer_list<int> whichMoments){
		Moments selectedMoments;
		for(int i: whichMoments)
			selectedMoments[i] = moments.at(i);
		return selectedMoments;
	};
private:
	std::map<int, double> moments;
};

/*
 * NOTE: In order to make a function return a pointer to function one must use and horrible syntax.
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

typedef DataSample (*functionSignature)(std::vector<DataSample>);

class ObservableAbstract {
public:
	ObservableAbstract(std::string obsName, bool isMeanKnownToBeZero);
	virtual ~ObservableAbstract();
	std::string getObservableName();
	EstimateAndError getValueAndError();

protected:
	//Calculation from raw data
	void calculateAndSetValueAndError(DataSample& dataSample, Parameters parameters);
	//Calculation for Reweighting
	void calculateAndSetValueAndError(Moments moments, std::vector<std::valarray<double> > estimators, ErrorCalculationMethod errorMethod);
	std::string observableName;
	bool isMeanZero;
private:
	std::vector<DataSample> getBinnedNeededMoments(std::vector<DataSample> dataSampleToBeBinned, const Parameters& parameters);
	//Virtual method that must be provided by children classes
	virtual std::vector<DataSample> getNeededMomentsPerDataPoint(DataSample& dataSample) = 0;
	virtual Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters) = 0;
	virtual void printCorrectBinningInformation(const Parameters& parameters) = 0;
	virtual functionSignature getFunctionToEvaluateObservable() = 0;
	virtual double calculateValueFromMoments(Moments moments) = 0;
	virtual double calculateErrorFromEstimators(std::vector<std::valarray<double> > estimators, ErrorCalculationMethod errorMethod) = 0;
	EstimateAndError observableEstimateAndError;
};


class Mean : public ObservableAbstract {
public:
	Mean(DataSample& dataSample, Parameters parameters);
	Mean(Moments moments, std::vector<std::valarray<double> > estimators, bool isMeanZero, ErrorCalculationMethod errorMethod);
	const std::array<int, 1> neededMoments{ {1} };
private:
	std::vector<DataSample> getNeededMomentsPerDataPoint(DataSample& dataSample);
	Parameters getLocalParametersWithCorrectBinningInformation(const Parameters& parameters);
	void printCorrectBinningInformation(const Parameters& parameters);
	functionSignature getFunctionToEvaluateObservable();
	double calculateValueFromMoments(Moments moments);
	double calculateErrorFromEstimators(std::vector<std::valarray<double> > estimators, ErrorCalculationMethod errorMethod);
};








class Observables
{
public:
    Observables() : mean(0.0, 0.0), susceptibility(0.0, 0.0),
                    skewness(0.0, 0.0), binderCumulant(0.0, 0.0) 
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
