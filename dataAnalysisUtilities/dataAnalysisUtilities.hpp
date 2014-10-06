#ifndef DATAANALYSISUTILITIES_HPP_
#define DATAANALYSISUTILITIES_HPP_

#include <iostream>
#include "binnedDataSample.hpp"
#include "DataSample.hpp"
#include "../Parameters/Parameters.hpp"
#include "../IO/io_utilities.hpp"

class EstimateAndError
{
public:
	EstimateAndError() :
		estimate(0.), error(0.)
	{}

	EstimateAndError(double mean, double error) :
		estimate(mean), error(error)
	{}

	double estimate;
	double error;
};

class RawAndBinnedDataSample
{
public:
	RawAndBinnedDataSample(std::string filename, Parameters parameters);
	DataSample & getRawData() {return rawData;}
	BinnedDataSample & getBinnedData() {return binnedData;}
private:
	DataSample rawData;
	BinnedDataSample binnedData;
};

EstimateAndError calcMeanAndErrorOfUncorrelatedDataSample(DataSample & sampleIn);
EstimateAndError calcMeanAndErrorOfDataSample(RawAndBinnedDataSample & sampleIn);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters);

std::vector<EstimateAndError> calcArrayOfAutocorrelationFunctionsAndErrorEsitmatesOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationTimesAndErrorEsitmatesOfDataSample(DataSample & sample, Parameters parameters);


#endif /* DATAANALYSISUTILITIES_HPP_ */
