#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include "DataSample.hpp"
#include "../Parameters/Parameters.hpp"
#include "EstimateAndError.hpp"

DataSample performBinning(DataSample & rawData, const Parameters parameters);

EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn);
EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcVarianceAndErrorOfUncorrelatedDataSample(DataSample & sampleIn);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcBinderAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
void calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters);

std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEstimatesOfDataSample(DataSample & sample, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
