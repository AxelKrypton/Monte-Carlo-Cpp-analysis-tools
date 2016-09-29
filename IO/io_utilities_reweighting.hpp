#ifndef IO_UTILITIES_REWEIGHTING_HPP_
#define IO_UTILITIES_REWEIGHTING_HPP_

#include <iostream>
#include <vector>
#include "../types.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

void writeLqcdReweightedObservablesToFile(std::vector<std::vector<realFloat> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData, std::string outputfilePrefix);
void writeLqcdReweightedObservablesEstimatorsToFile(std::vector<std::vector<realFloat> >& newBetaValues,
                                                    std::vector<std::vector<std::map<std::string,DataSample> > >& reweightedEstimators, std::string outputfilePrefix);

#endif /* IO_UTILITIES_REWEIGHTING_HPP_ */
