#ifndef IO_UTILITIES_REWEIGHTING_HPP_
#define IO_UTILITIES_REWEIGHTING_HPP_

#include <iostream>
#include <vector>
#include "../types.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

void writeLqcdReweightingResultsToFile(std::vector<std::vector<realFloat> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData, std::string outputfilePrefix);


#endif /* IO_UTILITIES_REWEIGHTING_HPP_ */
