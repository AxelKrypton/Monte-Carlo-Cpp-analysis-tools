#ifndef IO_UTILITIES_HPP_
#define IO_UTILITIES_HPP_

#include <iostream>
#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

std::string getFilenameForObservables(Parameters parameters);

void printEstimateAndError(std::string estimateName, realFloat estimateValue, realFloat errorValue);
void writeEstimateAndErrorToFile(std::string estimateName, realFloat estimateValue, realFloat errorValue, std::string filename);
void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<realFloat> estimate, std::vector<realFloat> error, std::string filename);
void writeLqcdReweightingResultsToFile(std::vector<std::vector<realFloat> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData, std::string outputfilePrefix);

#endif /* IO_UTILITIES_HPP_ */
