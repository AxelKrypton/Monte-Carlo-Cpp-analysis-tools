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
	RawAndBinnedDataSample(std::string filename, Parameters parameters)
	{
		rawData = DataSample(filename);
		if ( parameters.useBinning)
		{
			if (parameters.calcAutocorrelation)
			{
				std::cout << "Do not perform binning as the autocorrelation should be estimated!" << std::endl;
				binnedData = rawData;
			}
			else
			{
				std::cout << "Perform binning on data sample..." << std::endl;
				if ( parameters.useNumberOfBinsForBinning)
				{
					binnedData = BinnedDataSampleFromNumberOfBins(rawData, parameters.numberOfBins, parameters.binningMustFitDataSampleSize);
				}
				else
				{
					binnedData = BinnedDataSampleFromBinsize(rawData, parameters.binsize, parameters.binningMustFitDataSampleSize);
				}
			}
		}
		binnedData = rawData;
	}
	DataSample & getRawData() {return rawData;}
	DataSample & getbinnedData() {return binnedData;}
private:
	DataSample rawData;
	DataSample binnedData;
};

EstimateAndError calcMeanAndErrorOfDataSample(DataSample & sampleIn);
EstimateAndError calcVarianceAndErrorOfDataSample(DataSample & sampleIn);
EstimateAndError calcSkewnessAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcKurtosisAndErrorOfDataSample(DataSample & sampleIn, Parameters parameters);
EstimateAndError calcAutocorrelationAndErrorOfDataSample(DataSample & sample, Parameters parameters);
std::vector<EstimateAndError> calcArrayOfAutocorrelationAndErrorEsitmatesOfDataSample(DataSample & sample, Parameters parameters);
DataSample createDataSampleFromDatafile(std::string filename, Parameters parameters);

#endif /* DATAANALYSISUTILITIES_HPP_ */
