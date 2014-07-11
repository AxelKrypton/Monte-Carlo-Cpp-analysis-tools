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

