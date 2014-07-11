#ifndef IO_UTILITIES_HPP_
#define IO_UTILITIES_HPP_

#include <iostream>
#include "../Parameters/Parameters.hpp"

std::string getFilenameForObservables(Parameters parameters);

void printEstimateAndError(std::string estimateName, double estimateValue, double errorValue);
void writeEstimateAndErrorToFile(std::string estimateName, double estimateValue, double errorValue, std::string filename);

#endif /* IO_UTILITIES_HPP_ */
