#include "jackknifeAnalysis.hpp"
#include "jackknifeEstimators.hpp"

double calculateJacknifeEstimator(DataSample & sampleIn)
{
	return sampleIn.getNthMoment(1);
}

double calculateJacknifeError(DataSample & sampleIn)
{
	int jackknifeNormalization = sampleIn.getNumberOfElements() - 1;
	DataSample tmp ( (sampleIn - sampleIn.getNthMoment(1) )^( (double(2)) )  );
	return tmp.getNthMoment(1) * jackknifeNormalization;
}

EstimateAndError jackknifeAnalysis(DataSample sample1, DataSample sample2, DataSample (*function)(DataSample&, DataSample&) )
{
	JackknifeEstimators jackSample1(sample1);
	JackknifeEstimators jackSample2(sample2);
	
	DataSample functionAppliedToEstimators = function(jackSample1, jackSample2);
	
	double estimate = calculateJacknifeEstimator(functionAppliedToEstimators);
	double error = calculateJacknifeError(functionAppliedToEstimators);
	
	return EstimateAndError(estimate, error);
}