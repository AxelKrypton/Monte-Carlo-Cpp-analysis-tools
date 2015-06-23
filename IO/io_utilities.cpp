#include "io_utilities.hpp"

#include <fstream>

std::string getFilenameForObservables(Parameters parameters)
{
	return parameters.analysisOutputFilePrefix + parameters.file + parameters.analysisOutputFilePostfix;
}

void printEstimateAndError(std::string estimateName, realFloat estimateValue, realFloat errorValue)
{
	std::cout << "# " << estimateName << "\t\tError" << std::endl;
	std::cout << std::scientific << estimateValue << "\t" << errorValue << std::endl;
}

//todo: add test for exception
//todo: merge these two fcts.
void writeEstimateAndErrorToFile(std::string estimateName, realFloat estimateValue, realFloat errorValue, std::string filename)
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

void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<realFloat> estimate, std::vector<realFloat> error, std::string filename)
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
