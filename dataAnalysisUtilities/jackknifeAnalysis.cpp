#include "jackknifeAnalysis.hpp"
#include "jackknifeEstimators.hpp"

realFloat calculateJacknifeEstimate(DataSample sampleWithJackknifeEstimators)
{
    return sampleWithJackknifeEstimators.getNthMoment(1);
}

realFloat calculateJacknifeError(DataSample sampleWithJackknifeEstimators)
{
    DataSample tmp ( (sampleWithJackknifeEstimators - sampleWithJackknifeEstimators.getNthMoment(1) )^( (realFloat(2)) )  );
    return sqrt(tmp.getNthMoment(1) * (sampleWithJackknifeEstimators.getNumberOfElements() - 1));
}

EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData1, DataSample sampleWithUncorrelatedData2, DataSample (*function)(DataSample&, DataSample&) )
{
    JackknifeEstimators jackSample1(sampleWithUncorrelatedData1);
    JackknifeEstimators jackSample2(sampleWithUncorrelatedData2);
	
	DataSample functionAppliedToEstimators = function(jackSample1, jackSample2);
	
    realFloat estimate = calculateJacknifeEstimate(functionAppliedToEstimators);
	realFloat error = calculateJacknifeError(functionAppliedToEstimators);
	
	return EstimateAndError(estimate, error);
}

EstimateAndError jackknifeAnalysis(DataSample sampleWithUncorrelatedData, DataSample (*function)(DataSample&) )
{
    JackknifeEstimators jackSample(sampleWithUncorrelatedData);

    DataSample functionAppliedToEstimators = function(jackSample);

    realFloat estimate = calculateJacknifeEstimate(functionAppliedToEstimators);
    realFloat error = calculateJacknifeError(functionAppliedToEstimators);

    return EstimateAndError(estimate, error);
}

EstimateAndError jackknifeAnalysis(std::vector<DataSample> samplesWithUncorrelatedData, DataSample (*function)(std::vector<DataSample>) )
{
    std::vector<DataSample> jackknifeEstimators;
	for(size_t i=0; i<samplesWithUncorrelatedData.size(); i++){
		//Here we implicitly cast JackknifeEstimators to DataSample, but it should be fine!
		jackknifeEstimators.push_back(JackknifeEstimators(samplesWithUncorrelatedData[i]));
	}

    DataSample functionAppliedToEstimators = function(jackknifeEstimators);

    realFloat estimate = calculateJacknifeEstimate(functionAppliedToEstimators);
    realFloat error = calculateJacknifeError(functionAppliedToEstimators);

    return EstimateAndError(estimate, error);
}
