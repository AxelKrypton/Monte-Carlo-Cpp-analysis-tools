/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
 *  Copyright (c) 2016,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "io_utilities.hpp"

#include <fstream>

std::string getFilenameForObservables(Parameters parameters)
{
    return parameters.analysisOutputFilePrefix + parameters.file + parameters.analysisOutputFilePostfix;
}

void printEstimateAndError(std::string estimateName, realFloat estimateValue, realFloat errorValue)
{
    std::cout << "# \033[40m\033[1;32m" << estimateName << ": \033[1;36m" << std::scientific;
    std::cout << estimateValue << " \033[0m\u00B1\033[1;36m " << errorValue << "\033[0m" << std::endl;
}

// todo: add test for exception
// todo: merge these two fcts.
void writeEstimateAndErrorToFile(std::string estimateName, realFloat estimateValue, realFloat errorValue, std::string filename)
{
    std::ofstream outputstream;
    outputstream.open(filename.c_str(), std::ios::app);
    if (outputstream.is_open()) {
        std::cout << "# Writing estimate and error of \"" << estimateName << "\" to file \"" << filename << "\"" << std::endl;
        outputstream << "# " << estimateName << "\t\tError" << '\n';
        outputstream << std::scientific << estimateValue << "\t" << errorValue << '\n';
        outputstream.close();
    } else {
        throw std::invalid_argument("Could open file for estimate and error output. Aborting!");
    }
}

void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<realFloat> estimate, std::vector<realFloat> error,
                                       std::string filename)
{
    std::cout << "# Writing estimate and error array of \"" << estimateName << "\" to file \"" << filename << "\"" << std::endl;
    std::ofstream outputstream;
    outputstream.open(filename.c_str(), std::ios::app);
    if (outputstream.is_open()) {
        // todo: check if both are of equal size
        for (int i = 0; i < int(estimate.size()); i++) {
            outputstream << std::scientific << estimate[i] << "\t" << error[i] << '\n';
        }
        outputstream.close();
    } else {
        throw std::invalid_argument("Could open file for estimate and error output. Aborting!");
    }
}
