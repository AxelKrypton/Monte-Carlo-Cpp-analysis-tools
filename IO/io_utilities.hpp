#ifndef IO_UTILITIES_HPP_
#define IO_UTILITIES_HPP_

#include <iostream>
#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/EstimateAndError.hpp"

std::string getFilenameForObservables(Parameters parameters);

void printEstimateAndError(std::string estimateName, double estimateValue, double errorValue);
void writeEstimateAndErrorToFile(std::string estimateName, double estimateValue, double errorValue, std::string filename);
void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<double> estimate, std::vector<double> error, std::string filename);
void writeReweightingResultsToFile(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<EstimateAndError> > & reweightedObservables);

#endif /* IO_UTILITIES_HPP_ */
