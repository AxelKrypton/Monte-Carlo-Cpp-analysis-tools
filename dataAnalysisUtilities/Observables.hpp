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
     * with mean mu)
     */
    EstimateAndError mean;            // <x>
    EstimateAndError susceptibility;  // <(x-mu)^2>
    EstimateAndError skewness;        // <(x-mu)^3> / <(x-mu)^2>^(3/2)
    EstimateAndError binderCumulant;  // <(x-mu)^4> / <(x-mu)^2>^2
};

#endif /* OBSERVABLES_HPP_ */
