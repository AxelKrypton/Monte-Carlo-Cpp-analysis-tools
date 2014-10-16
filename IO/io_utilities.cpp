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
	
	void append(double betaValue, std::vector<EstimateAndError> estimateAndError)
	{
		std::pair<double, std::vector<EstimateAndError> > tmpPair(betaValue, estimateAndError );
		values.push_back( tmpPair );
	}
	
	void printToFile()
	{
		std::string specificFilename = "reweightedData_" + quantityName;
		std::cout << "# Writing reweighted data for " << quantityName << "\" to file \"" << specificFilename << "\"" << std::endl;
		uint numberOfObservables = values[0].second.size();
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
				for (uint index2 = 0; index2 < numberOfObservables; index2 ++)
				{
					outputstream << values[index].second[index2].estimate << "\t" << values[index].second[index2].error ;
				}
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
	std::vector < std::pair<double, std::vector<EstimateAndError> > > values;
};

void writeReweightingResultsToFile(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<EstimateAndError> > & reweightedObservables)
{
	uint numberOfNewPoints = reweightedObservables.size();
	uint numberOfObservables = reweightedObservables[0].size();
	if ( numberOfObservables <= 0 )
	{
		throw std::invalid_argument("Number of observables must be greater than 0! Aborting!");
	}
	uint numberOfQuantities = reweightedObservables[0].size() / numberOfObservables;

	//todo: make this an argument
	std::vector<std::string> observableNames(1, "mean" );
	std::vector<ReweightedData> ReweightedQuantities;
	
	if ( newBetaValues.size() != reweightedObservables.size() )
	{
		throw std::invalid_argument("Number of new beta points and reweighted observables does not match! Aborting!");
	}
	if ( numberOfObservables != observableNames.size() )
	{
		throw std::invalid_argument("Number of observables and names of observables do not match! Aborting!");
	}
	if ( reweightedObservables.size() % numberOfObservables != 0)
	{
		throw std::invalid_argument("Did not get all observables for all quantities! Aborting!");
	}
	
	for (uint iteration=0; iteration < numberOfNewPoints; iteration ++)
	{
		ReweightedQuantities.push_back (ReweightedData( "quantity" + boost::lexical_cast<std::string>(iteration + 1), observableNames ) );
		for (uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++)
		{
			std::vector<EstimateAndError> observables;
			for( uint observableIndex = 0; observableIndex < numberOfObservables; observableIndex ++)
			{
				uint index = quantityIndex*numberOfQuantities + observableIndex;
				observables.push_back( reweightedObservables[iteration][index] );
			}
			ReweightedQuantities[quantityIndex].append( newBetaValues[iteration][quantityIndex], observables ) ;
		}
	}
	
	for(uint iteration = 0; iteration < numberOfQuantities; iteration ++)
	{
		ReweightedQuantities[iteration].printToFile();
	}
}
