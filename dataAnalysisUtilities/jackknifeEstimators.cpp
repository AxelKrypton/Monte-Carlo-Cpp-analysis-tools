#include "jackknifeEstimators.hpp"

JackknifeEstimatorsFromBinnedDataSample::JackknifeEstimatorsFromBinnedDataSample(DataSampleAnalyzer sampleIn) :
JackknifeEstimators(sampleIn)
{
	int normalization = getJackknifeNormalization();
	double sumOfDataSampleElements = sampleIn.sum();
	setValues( (*this - sumOfDataSampleElements) * (-1./normalization) );
}

DataSample JackknifeEstimators::createJackknifeEstimatorsWithBinning(int numberOfBins, int binsize)
{
	std::cout << "create binned jackknife estimators with number of bins: " << numberOfBins << " and binsize: " << binsize << std::endl;

	DataSample cutSample = sampleSlice(0, binsize*numberOfBins, 1);
	double sumOfAllElementsInBinnedSample = cutSample.sum();
	int numberOfElementsInBinndedSample = cutSample.getNumberOfElements();

	DataSample binnedDataSample(numberOfBins);
	for (int iteration = 0; iteration < numberOfBins; iteration++)
	{
		double sumOfAllElementsInBin = ( sampleSlice(iteration * binsize,	binsize, 1) ).sum();
		binnedDataSample[iteration] = (sumOfAllElementsInBinnedSample - sumOfAllElementsInBin) / (numberOfElementsInBinndedSample - binsize);
	}
	return binnedDataSample;
}

JackknifeEstimatorsFromBinningWithNumberOfBins::JackknifeEstimatorsFromBinningWithNumberOfBins(DataSampleAnalyzer sampleIn, int numberOfBins) :
	JackknifeEstimators(sampleIn)
{
	checkIfJackknifeCanBePerformed(numberOfBins);
	int binsize = calcBinsize(numberOfBins);
	setValues( createJackknifeEstimatorsWithBinning(numberOfBins, binsize) );
}

JackknifeEstimatorsFromBinningWithBinsize::JackknifeEstimatorsFromBinningWithBinsize(DataSampleAnalyzer sampleIn, int binsize) :
	JackknifeEstimators(sampleIn)
{
	checkIfJackknifeCanBePerformedWithBinsize(binsize);
	int numberOfBins = calcBinsize(binsize);
	setValues( createJackknifeEstimatorsWithBinning(numberOfBins, binsize) );
}

double JackknifeEstimators::getJackknifeVariance()
{
	DataSampleAnalyzer tmp ( (*this - getNthMoment(1) )^( (double(2)) )  );
	return tmp.getNthMoment(1) * getJackknifeNormalization();
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed(numberOfElements);
	return numberOfElements - 1;
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1 || n > numberOfElements)
		throw std::invalid_argument("Cannot create jackknifeEstimators with these parameters!");
}

void JackknifeEstimatorsFromBinningWithBinsize::checkIfJackknifeCanBePerformedWithBinsize(int binsize)
{
	if(binsize < 1 || binsize >= numberOfElements)
		throw std::invalid_argument("Cannot create jackknifeEstimators with this binsize!");
}
