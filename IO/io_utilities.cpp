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

class LqcdReweightedData
{
public:
	LqcdReweightedData(std::string quantityNameIn) : quantityName(quantityNameIn)
	{
		filename = "reweightedData_" + quantityName;
	}
	
	void append(double betaValue, Observables observables)
	{
		std::pair<double, Observables > tmpPair(betaValue, observables );
		values.push_back( tmpPair );
	}
	
	void printToFile()
	{
		std::cout << "# Writing reweighted data for \"" << quantityName << "\" to file \"" << filename << "\"" << std::endl;
		std::ofstream outputstream;
		outputstream.open(filename.c_str(), std::ios::app);
		if(outputstream.is_open()) {
			outputstream << "# beta\t\t" << values[0].second.getMetaInformation() << std::endl;
			
			for (uint index = 0; index < values.size(); index ++)
			{
				outputstream << std::scientific << values[index].first << "\t" << values[index].second.getObservablesAsString() << std::endl;
			}
			outputstream.close();
		} 
		else {
			throw std::invalid_argument("Could open file for output. Aborting!");
		}
	}
	
private:
	std::string quantityName;
	std::string filename;
	std::vector < std::pair<double, Observables> > values;
};

static void checkInputSizes(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData)
{
	uint numberOfNewPoints = reweightedData.size();
	uint numberOfQuantities = reweightedData[0].size();
	
	if ( numberOfQuantities <= 0 )
	{
		throw std::invalid_argument("Number of reweighted quantities must be greater than 0! Aborting!");
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
}

void writeLqcdReweightingResultsToFile(std::vector<std::vector<double> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData)
{
	checkInputSizes(newBetaValues, reweightedData);
	
	std::vector<LqcdReweightedData> ReweightedQuantities;
	uint numberOfNewPoints = reweightedData.size();
	uint numberOfQuantities = reweightedData[0].size();
	
	for (uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++)
	{
		LqcdReweightedData reweightedQuantity( "quantity" + boost::lexical_cast<std::string>(quantityIndex + 1) );

		for (uint iteration=0; iteration < numberOfNewPoints; iteration ++)
		{
			reweightedQuantity.append( newBetaValues[iteration][0], reweightedData[iteration][quantityIndex] ) ;
		}
		
		ReweightedQuantities.push_back (reweightedQuantity);
	}

	for(uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex ++)
	{
		ReweightedQuantities[quantityIndex].printToFile();
	}
}
