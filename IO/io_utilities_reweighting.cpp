#include "io_utilities_reweighting.hpp"

#include <fstream>
#include <boost/lexical_cast.hpp>

class LqcdReweightedData
{
public:
	LqcdReweightedData(std::string quantityNameIn, std::string outputfilePrefix) : quantityName(quantityNameIn)
	{
		filename = outputfilePrefix + "_" + quantityName;
	}

	void append(realFloat betaValue, Observables observables)
	{
		std::pair<realFloat, Observables > tmpPair(betaValue, observables );
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
	std::vector < std::pair<realFloat, Observables> > values;
};


static void checkInputSizes(std::vector<std::vector<realFloat> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData)
{
	if ( reweightedData.size() == 0 )
	{
		throw std::exception();
	}

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


void writeLqcdReweightingResultsToFile(std::vector<std::vector<realFloat> > & newBetaValues, std::vector<std::vector<Observables> > & reweightedData, std::string outputfilePrefix)
{
	try{
		checkInputSizes(newBetaValues, reweightedData);
	}catch(std::invalid_argument& e){
		throw e;
	}catch(std::exception& e){
		std::cout << "\n   No reweighting procedure has been performed, probably because none was asked. No file will be created.\n" << std::endl;
		return;
	}

	std::vector<LqcdReweightedData> ReweightedQuantities;
	uint numberOfNewPoints = reweightedData.size();
	uint numberOfQuantities = reweightedData[0].size();

	for (uint quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++)
	{
		LqcdReweightedData reweightedQuantity( "quantity" + boost::lexical_cast<std::string>(quantityIndex + 1), outputfilePrefix );

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



