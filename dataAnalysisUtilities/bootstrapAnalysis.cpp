#include "bootstrapAnalysis.hpp"

/*
 * The problem of the following function, if naively implemented, is that it could return nan if
 * the input data sample contains the same entry several times. To avoid this case we take the
 * absolute value before the square root.
 */
realFloat calculateBootstrapError(DataSample& sampleWithBootstrapEstimators)
{
	return sqrt(std::abs(sampleWithBootstrapEstimators.getNthMoment(2) - pow(sampleWithBootstrapEstimators.getNthMoment(1), 2.)));
}



