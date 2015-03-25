#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include "DataSample.hpp"
#include "../Parameters/Parameters.hpp"
#include "EstimateAndError.hpp"

/*
 * In all the quantities calculation, it is taken into account whether the mean is known to be zero or not.
 * This means that, if known, the mean is not estimated out from the sampleIn, but manually set to zero.
 */

//The following 4 functions DO BINNING(!) on sampleIn and then calculate the observable
EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcBinderAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
//The following two functions are not used in the rest of the project. They are implemented here
//for testing reasons (in tests binning is often not needed and it could be annoying to use the
//functions above). TODO: Improve using the functions above in tests.
EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample & sampleIn, bool isMeanKnownToBeZero = false);
EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn, bool isMeanKnownToBeZero = false);


void calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
