#include "bootstrapAnalysis.hpp"

double calculateBootstrapError(DataSample sampleWithBootstrapEstimators)
{
    return sqrt(sampleWithBootstrapEstimators.getNthMoment(2) - pow(sampleWithBootstrapEstimators.getNthMoment(1), 2.));
}



