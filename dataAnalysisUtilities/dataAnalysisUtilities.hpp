#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include "DataSample.hpp"
#include "../Parameters/Parameters.hpp"
#include "EstimateAndError.hpp"

//The following 4 functions DO BINNING(!) on sampleIn and then calculate the observable
EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcBinderAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
//The following two functions are not used in the rest of the project. Remove them!?
EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample & sampleIn);
EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn);

void calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
