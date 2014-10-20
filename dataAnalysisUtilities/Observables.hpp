#ifndef OBSERVABLES_HPP_
#define OBSERVABLES_HPP_

#include "../dataAnalysisUtilities/EstimateAndError.hpp"

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
    
    std::vector<std::string> observableNames;
};

#endif /* OBSERVABLES_HPP_ */
