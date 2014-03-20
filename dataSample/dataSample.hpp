/**
 * Definition of the dataSample class
 */

#ifndef DATASAMPLE_HPP_
#define DATASAMPLE_HPP_

#include<valarray>
#include<vector>
#include<stdexcept>

const static int roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid = 1e4;

//TODO: move this to better place
double defaultFunction(double in);

//TODO: implement offset
class DataSample
{
public:
	DataSample(int length = defaultSizeOfDataSample);
	DataSample(std::valarray<double> valuesIn);
	//todo: think about creating two or three distinct constructors
	DataSample(std::string dataFilename, int column = 1, int offset = 0);
	int getNumberOfElements();
	int getUpperLimitForNthMoment();
	int getLowerLimitForNthMoment();
	double getNthCentralMoment(int n);
	double getNthMoment(int n);
	DataSample createBinnedDataSampleWithNumberOfBins(int numberOfBins);
	DataSample createBinnedDataSampleWithBinsize(int binsize);
	DataSample applyFunction(double (*function)(double) = defaultFunction);
	DataSample createShiftedDataSample(int order, double shift);
	double sum();

protected:
	double calcNthMoment(int n);
	double calcNthMomentExplicit(int n);
	double calcNthCentralMomentExplicit(int n);
	double calcFirstMomentExplicit();
	void checkIfNIsValid(int n);
	void checkIfNumberOfBinsIsValid(int numberOfBins);
	void checkIfBinsizeIsValid(int binsize);
	void checkIfNumberOfElementsIsValid(int length);
	void checkIfDatafileExists(std::string filename);
	void checkIfColumnIsValid(int column);
	void checkIfOffsetIsValid(int offset);
	int calcBinsize(int numberOfBins);
	int calcNumberOfBins(int binsize);
	void initMembers();
	//todo: init central moments and create member variable for that...
	void initMoments();
	int getNumberOfMoments();
	/**
	 * Following Berg, p.52.
	 */
	DataSample performBinning(int numberOfBins, int binsize);
	std::valarray<double> readDataFromFile(std::string filename, int column, int offset);

	std::valarray<double> values;
	std::vector<double> moments;
	int numberOfElements;
	const static int upperLimitForNthMoment = 4;
	const static int lowerLimitForNthMoment = 0;
	const static int defaultSizeOfDataSample = 1;
};

/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

class JackknifeEstimators: public DataSample
{
public:

	JackknifeEstimators(DataSample sampleIn) :
		DataSample(sampleIn)
	{
		checkIfJackknifeCanBePerformed();
	};

	double getJackknifeVariance();
	double getJackknifeVariance_v2();
	double getJackknifeError();

protected:
	int getJackknifeNormalization();
	void checkIfJackknifeCanBePerformed();
};

class JackknifeEstimatorsFromBinnedDataSample: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinnedDataSample(DataSample sampleIn) :
		JackknifeEstimators(sampleIn)
	{
		int normalization = getJackknifeNormalization();
		double sumOfDataSampleElements = values.sum();
		values = (sumOfDataSampleElements - values) / normalization;
		//todo: this is necessary because otherwise the moments from the above sample are returned
		//       as they are calculated in the constructor -> This must be done on demand!
		initMoments();
	}
};

//todo: generalise this to binsize
class JackknifeDataSampleWithBinning: public JackknifeEstimators
{
public:
	JackknifeDataSampleWithBinning(DataSample sampleIn, int numberOfBins) :
		JackknifeEstimators(sampleIn)
	{
		checkIfJackknifeCanBePerformed(numberOfBins);
		double wholeSum = sampleIn.sum();
		std::cout << std::scientific << wholeSum << std::endl;
		int binsize = calcBinsize(numberOfBins);
		std::valarray<double> binnedDataSample(numberOfBins);
		for(int iteration = 0; iteration < numberOfBins; iteration++)
		{
			double partSum=0.;
			for (int j = 0; j< binsize; j++){
				partSum += values[iteration*binsize + j];
			}
//			std::valarray<double> tmp = values[std::slice(iteration*binsize, binsize, 1)];
//			double partSum = tmp.sum();
//			std::cout << std::scientific << wholeSum << " "<< partSum << std::endl;
			binnedDataSample[iteration] = (wholeSum - partSum) / (sampleIn.getNumberOfElements() - binsize);
		}
		values = binnedDataSample;

		numberOfElements = binnedDataSample.size();
		//todo: this is necessary because otherwise the moments from the above sample are returned
		//       as they are calculated in the constructor -> This must be done on demand!
		initMoments();
		std::cout << getNthMoment(1) << std::endl;

//		for (int i = 0; i< numberOfElements; i++)
//		{
//			std::cout << std::scientific << values[i]<< std::endl;
//		}
	}
private:
	void checkIfJackknifeCanBePerformed(int n);
};

#endif
