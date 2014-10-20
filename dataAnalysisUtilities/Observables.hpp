#ifndef OBSERVABLES_HPP_
#define OBSERVABLES_HPP_

#include "../dataAnalysisUtilities/EstimateAndError.hpp"

class Observables
{
public:
    Observables() : mean(0.0, 0.0), susceptibility(0.0, 0.0),
                    skewness(0.0, 0.0), binderCumulant(0.0, 0.0) {}
    /*
     * Here in the following the observables we deal with around in the
     * library, together with their definition (thinking to a sample set X
     * with mean mu, x1,x2,x3,x4 are the 1st,2nd,3rd,4th moment of the sample)
     */
    EstimateAndError mean;            // <x>                            --->  x1
    EstimateAndError susceptibility;  // <(x-mu)^2>                     --->  x2-x1^2
    EstimateAndError skewness;        // <(x-mu)^3> / <(x-mu)^2>^(3/2)  --->  (x3-3x2*x1+2*x1^3)/(x2-x1^2)^(3/2)
    EstimateAndError binderCumulant;  // <(x-mu)^4> / <(x-mu)^2>^2      --->  (x4-4*x3*x1+6*x2*x1^2-3*x1^4)/(x2-x1^2)^2
};

#endif /* OBSERVABLES_HPP_ */
