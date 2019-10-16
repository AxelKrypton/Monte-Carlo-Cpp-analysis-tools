#ifndef IO_UTILITIES_REWEIGHTING_HPP_
#define IO_UTILITIES_REWEIGHTING_HPP_

#include <iostream>
#include <vector>
#include "../Reweighting/Histogram.hpp"
#include "../types.hpp"
#include "../dataAnalysisUtilities/Observables.hpp"

void writeLqcdReweightedObservablesToFile(const std::vector<std::vector<realFloat> >& newBetaValues, const std::vector<std::vector<Observables> >& reweightedData, std::string outputfilePrefix);
void writeLqcdReweightedObservablesEstimatorsToFile(const std::vector<std::vector<realFloat> >& newBetaValues,
                                                    const std::vector<std::vector<std::map<std::string,DataSample> > >& reweightedEstimators, std::string outputfilePrefix);
void writeLqcdReweightedProbabilityDistributionsToFile(const std::vector<std::vector<realFloat> >& newBetaValues,
                                                       const std::vector<std::vector<ProbabilityDistribution> >& reweightedProbabilityDistribution, std::string outputfilePrefix);

#endif /* IO_UTILITIES_REWEIGHTING_HPP_ */
