#ifndef JACKKNIFEANALYSIS_HPP_
#define JACKKNIFEANALYSIS_HPP_

#include "DataSample.hpp"
#include "EstimateAndError.hpp"

double calculateJacknifeEstimate(DataSample sampleWithJackknifeEstimators);
double calculateJacknifeError(DataSample sampleWithJackknifeEstimators);

EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData, DataSample (*function)(DataSample &) );
EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData1,
                                   DataSample sampleWithUncorrelatedData2, DataSample (*function)(DataSample&, DataSample&) );

#endif
