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
		std::cout << "# Writing reweighted data for \"" << quantityName << "\" to file \"" << specificFilename << "\"" << std::endl;
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

void writeReweightingResultsToFile(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<EstimateAndError> > & reweightedData)
{
	for ( uint i = 0; i < reweightedData.size(); i++)
	{
		for (uint j = 0; j < reweightedData[0].size(); j++)
		{
			std::cout << i << " " << j << " " << 
			"beta: " << boost::lexical_cast<std::string>(newBetaValues[i][0]) <<  "\t" <<  
			"value: " << boost::lexical_cast<std::string>(reweightedData[i][j].estimate) << std::endl;
		}
		
	}
	
	
	
	
	
	
	
	//todo: make this an argument
	std::vector<std::string> quantityNames(1, "plaq" );
	quantityNames.push_back( "poly_re");
	quantityNames.push_back( "poly_im");
	quantityNames.push_back( "poly_im_abs");
	quantityNames.push_back( "poly_sq");
	quantityNames.push_back( "poly_ph");
	uint numberOfQuantitiesExpected = quantityNames.size();
	std::vector<std::string> observableNames(1, "mean" );
	uint numberOfObservablesExpected = observableNames.size();
	
	uint numberOfNewPoints = reweightedData.size();
	uint numberOfReweightedDataTotal = reweightedData[0].size();
	
	if ( numberOfReweightedDataTotal <= 0 )
	{
		throw std::invalid_argument("Number of reweighted datasets must be greater than 0! Aborting!");
	}
	std::cout << "Found " << numberOfReweightedDataTotal << " reweighted datasets." << std::endl;
	
	uint numberOfObservables = numberOfReweightedDataTotal / numberOfQuantitiesExpected;
	if ( numberOfObservables != numberOfObservablesExpected)
	{
		throw std::invalid_argument("Got wrong number of observables! Expected " + boost::lexical_cast<std::string>(numberOfObservablesExpected) + ", got " + boost::lexical_cast<std::string>(numberOfObservables) + " instead. Aborting!");
	}
	if ( numberOfReweightedDataTotal % numberOfQuantitiesExpected != 0)
	{
		throw std::invalid_argument("Did not get all observables for all quantities! Aborting!");
	}
	std::cout << "Expected " << numberOfQuantitiesExpected << " reweighted quantities." << std::endl;
	std::cout << "Found " << numberOfObservables << " different observable(s) for each quantity." << std::endl;
	

	if ( newBetaValues.size() != numberOfNewPoints )
	{
		throw std::invalid_argument("Number of new beta points and reweighted observables does not match! Aborting!");
	}
	if ( newBetaValues[0].size() != 1 )
	{
		throw std::invalid_argument("Found new values for more than one parameter! Aborting!");
	}
	
	std::vector<ReweightedData> ReweightedQuantities;
	for (uint quantityIndex = 0; quantityIndex < numberOfQuantitiesExpected; quantityIndex++)
	{
// 		std::cout << "quant: " << quantityIndex << std::endl;
		ReweightedData tmp( "quantity" + boost::lexical_cast<std::string>(quantityIndex + 1), observableNames );
			
		for (uint iteration=0; iteration < numberOfNewPoints; iteration ++)
		{
// 			std::cout << "iter: " << iteration << std::endl;
// 			std::cout << newBetaValues[iteration][0] << std::endl;
				
			std::vector<EstimateAndError> observables;
			for( uint observableIndex = 0; observableIndex < numberOfObservables; observableIndex ++)
			{
// 				std::cout << "obs: " << observableIndex << std::endl;
				uint index = quantityIndex + observableIndex*numberOfQuantitiesExpected;
// 				std::cout << "idx: " << index << std::endl;
// 				std::cout << "value: " << boost::lexical_cast<std::string>(reweightedData[iteration][index].estimate) << std::endl;
				observables.push_back( reweightedData[iteration][index] );
			}
			tmp.append( newBetaValues[iteration][0], observables ) ;
		}
		ReweightedQuantities.push_back (tmp);
	}
	std::cout << "print to file" << std::endl;
	for(uint iteration = 0; iteration < numberOfQuantitiesExpected; iteration ++)
	{
		ReweightedQuantities[iteration].printToFile();
	}
}
