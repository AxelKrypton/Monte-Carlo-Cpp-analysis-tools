#include "io_utilities.hpp"

std::string getFilenameForObservables(Parameters parameters)
{
	return parameters.analysisOutputFilePrefix + parameters.file + parameters.analysisOutputFilePostfix;
}

