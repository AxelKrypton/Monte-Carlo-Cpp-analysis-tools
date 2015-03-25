#include "jackknifeAnalysis.hpp"
#include "jackknifeEstimators.hpp"

double calculateJacknifeEstimate(DataSample sampleWithJackknifeEstimators)
{
    return sampleWithJackknifeEstimators.getNthMoment(1);
}

double calculateJacknifeError(DataSample sampleWithJackknifeEstimators)
{
    DataSample tmp ( (sampleWithJackknifeEstimators - sampleWithJackknifeEstimators.getNthMoment(1) )^( (double(2)) )  );
    return sqrt(tmp.getNthMoment(1) * (sampleWithJackknifeEstimators.getNumberOfElements() - 1));
}

EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData1, DataSample sampleWithUncorrelatedData2, DataSample (*function)(DataSample&, DataSample&) )
{
    JackknifeEstimators jackSample1(sampleWithUncorrelatedData1);
    JackknifeEstimators jackSample2(sampleWithUncorrelatedData2);
	
	DataSample functionAppliedToEstimators = function(jackSample1, jackSample2);
	
    double estimate = calculateJacknifeEstimate(functionAppliedToEstimators);
	double error = calculateJacknifeError(functionAppliedToEstimators);
	
	return EstimateAndError(estimate, error);
}

EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData, DataSample (*function)(DataSample&) )
{
    JackknifeEstimators jackSample(sampleWithUncorrelatedData);

    DataSample functionAppliedToEstimators = function(jackSample);

    double estimate = calculateJacknifeEstimate(functionAppliedToEstimators);
    double error = calculateJacknifeError(functionAppliedToEstimators);

    return EstimateAndError(estimate, error);
}
