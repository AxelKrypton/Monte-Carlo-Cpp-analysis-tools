#ifndef JACKKNIFEANALYSIS_HPP_
#define JACKKNIFEANALYSIS_HPP_

#include "DataSample.hpp"
#include "EstimateAndError.hpp"

double calculateJacknifeEstimator(DataSample & sampleIn);
double calculateJacknifeError(DataSample & sampleIn);

EstimateAndError jackknifeAnalysis(DataSample sample1, DataSample sample2, DataSample (*function)(DataSample&, DataSample&) );

#endif