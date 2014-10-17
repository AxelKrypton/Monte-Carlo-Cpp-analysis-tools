#include "io_utilities.hpp"

#include <fstream>

std::string getFilenameForObservables(Parameters parameters)
{
	return parameters.analysisOutputFilePrefix + parameters.file + parameters.analysisOutputFilePostfix;
}

void printEstimateAndError(std::string estimateName, double estimateValue, double errorValue)
{
	std::cout << "# " << estimateName << "\t\tError" << std::endl;
	std::cout << std::scientific << estimateValue << "\t" << errorValue << std::endl;
}

//todo: add test for exception
//todo: merge these two fcts.
void writeEstimateAndErrorToFile(std::string estimateName, double estimateValue, double errorValue, std::string filename)
{
	std::ofstream outputstream;
	outputstream.open(filename.c_str(), std::ios::app);
	if(outputstream.is_open()) {
		std::cout << "# Writing estimate and error of \"" << estimateName << "\" to file \"" << filename << "\"" << std::endl;
		outputstream << "# " << estimateName << "\t\tError"  << '\n';
		outputstream << std::scientific << estimateValue << "\t" << errorValue << '\n';
		outputstream.close();
	} else {
		throw std::invalid_argument("Could open file for estimate and error output. Aborting!");
	}
}

void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<double> estimate, std::vector<double> error, std::string filename)
{
	std::cout << "# Writing estimate and error array of \"" << estimateName << "\" to file \"" << filename << "\"" << std::endl;
	std::ofstream outputstream;
	outputstream.open(filename.c_str(), std::ios::app);
	if(outputstream.is_open()) {
		//todo: check if both are of equal size
		for (int i = 0; i < int(estimate.size() ); i++)
		{
			outputstream << std::scientific << estimate[i] << "\t" << error[i] << '\n';
		}
		outputstream.close();
	} else {
		throw std::invalid_argument("Could open file for estimate and error output. Aborting!");
	}
}

class ReweightedData
{
public:
	ReweightedData(std::string quantityNameIn, std::vector<std::string> observableNamesIn) :
	quantityName(quantityNameIn), observableNames(observableNamesIn) {}
	
	void append(double betaValue, Observables observables)
	{
		std::pair<double, Observables > tmpPair(betaValue, observables );
		values.push_back( tmpPair );
	}
	
	void printToFile()
	{
		std::string specificFilename = "reweightedData_" + quantityName;
		std::cout << "# Writing reweighted data for \"" << quantityName << "\" to file \"" << specificFilename << "\"" << std::endl;
		uint numberOfObservables = observableNames.size();
		std::ofstream outputstream;
		outputstream.open(specificFilename.c_str(), std::ios::app);
		if(outputstream.is_open()) {
			outputstream << "# beta\t\t";
			for (uint index2 = 0; index2 < numberOfObservables; index2 ++)
			{
				outputstream << observableNames[index2] << "\t\terror\t\t";
			}
			outputstream  << std::endl;
			
			for (uint index = 0; index < values.size(); index ++)
			{
				outputstream << std::scientific << values[index].first << "\t" ;
				outputstream << values[index].second.mean.estimate << "\t" << values[index].second.mean.error<< "\t" ;
				outputstream << values[index].second.susceptibility.estimate << "\t" << values[index].second.susceptibility.error << "\t";
				outputstream << values[index].second.skewness.estimate << "\t" << values[index].second.skewness.error << "\t";
				outputstream << values[index].second.binderCumulant.estimate << "\t" << values[index].second.binderCumulant.error ;
				outputstream << std::endl;
			}
			outputstream.close();
		} 
		else {
			throw std::invalid_argument("Could open file for output. Aborting!");
		}
	}
	
private:
	std::string quantityName;
	std::vector<std::string> observableNames;
	std::vector < std::pair<double, Observables> > values;
};

void writeReweightingResultsToFile(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData)
{
	//todo: make this an argument
	std::vector<std::string> quantityNames(1, "plaq" );
	quantityNames.push_back( "poly_re");
	quantityNames.push_back( "poly_im");
	quantityNames.push_back( "poly_im_abs");
	quantityNames.push_back( "poly_sq");
	quantityNames.push_back( "poly_ph");
	uint numberOfQuantitiesExpected = quantityNames.size();
	std::vector<std::string> observableNames(1, "mean" );
	observableNames.push_back("susc");
	observableNames.push_back("skew");
	observableNames.push_back("binder");
	
	uint numberOfNewPoints = reweightedData.size();
	uint numberOfQuantities = reweightedData[0].size();
	
	if ( numberOfQuantities <= 0 )
	{
		throw std::invalid_argument("Number of reweighted quantities must be greater than 0! Aborting!");
	}
	if ( numberOfQuantities !=  numberOfQuantitiesExpected)
	{
		throw std::invalid_argument("Did not get all expected quantities. Aborting!");
	}
	if ( newBetaValues.size() != numberOfNewPoints )
	{
		throw std::invalid_argument("Number of new beta points and reweighted observables does not match! Aborting!");
	}
	if ( newBetaValues[0].size() != 1 )
	{
		throw std::invalid_argument("Found new values for more than one parameter! Aborting!");
	}
	std::cout << "# Found " << numberOfQuantities << " reweighted quantities." << std::endl;
	
	std::vector<ReweightedData> ReweightedQuantities;
	for (uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++)
	{
		ReweightedData tmp( "quantity" + boost::lexical_cast<std::string>(quantityIndex + 1), observableNames );

		for (uint iteration=0; iteration < numberOfNewPoints; iteration ++)
		{
			tmp.append( newBetaValues[iteration][0], reweightedData[iteration][quantityIndex] ) ;
		}
		ReweightedQuantities.push_back (tmp);
	}

	for(uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex ++)
	{
		ReweightedQuantities[quantityIndex].printToFile();
	}
}
