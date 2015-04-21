#ifndef BOOTSTRAPANALYSIS_HPP_
#define BOOTSTRAPANALYSIS_HPP_

#include "DataSample.hpp"

/*
 * Here the Barkema's book is followed (in particular Eq.(3.41) at page 71). The name "Bootstrap Estimator" is not
 * common in the literature, it is used here just to mean the result of the calculation of the chosen observable on
 * the re-sampled data set
 */
double calculateBootstrapError(DataSample& sampleWithBootstrapEstimators);

#endif /* BOOTSTRAPANALYSIS_HPP_ */
