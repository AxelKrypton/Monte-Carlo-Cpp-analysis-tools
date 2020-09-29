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
#include <iomanip>

void PrintRepeatedSymbol(char symbol, int times)
{
    std::cout << "#" << std::setfill(symbol) << std::setw(times) << "\n" << std::setfill(' ');
}

static std::string insertPostfixIntoFilename(const std::string& filename, const std::string& postfix)
{
    std::size_t found = filename.find_last_of(".");
    return filename.substr(0, found) + postfix + filename.substr(found);
}

std::string getFilenameForObservables(Parameters parameters)
{
    return insertPostfixIntoFilename(parameters.file, "_quantities");
}

std::string getFilenameForAutocorrelation(Parameters parameters)
{
    return insertPostfixIntoFilename(parameters.file, "_autocorrelation");
}

void printEstimateAndError(std::string estimateName, realFloat estimateValue, realFloat errorValue)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);
    std::cout << "# \033[40m\033[1;32m" << estimateName << ": \033[1;36m" << std::scientific;
    std::cout << estimateValue << " \033[0m\u00B1\033[1;36m " << errorValue << "\033[0m" << std::endl;
    std::cout.copyfmt(oldState);
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

void writeEstimateAndErrorArraysToFile(std::vector<EstimateAndError> values, std::string filename)
{
    std::cout << "# Writing analysis result to file \"" << filename << "\"\n";
    std::ofstream outputstream;
    outputstream.open(filename.c_str(), std::ios::app);
    if (outputstream.is_open()) {
        outputstream << "#Tau_int\t\terror\n";
        for (int i = 0; i < int(values.size()); i++)
            outputstream << std::scientific << std::setprecision(12) << values[i] << '\n';
        outputstream.close();
    } else {
        throw std::invalid_argument("Could open file for estimate and error output. Aborting!");
    }
}
