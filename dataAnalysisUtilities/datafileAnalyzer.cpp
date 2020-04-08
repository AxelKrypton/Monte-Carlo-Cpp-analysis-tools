/*
 *
 *  Copyright (c) 2020 Alessandro Sciarra
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

#include "datafileAnalyzer.hpp"

#include "../IO/io_utilities.hpp"

#include <fstream>

ObservableAnalyzer::ObservableAnalyzer(std::vector<DataSample> data, Parameters parameters)
    : parameters(parameters)
    , quantities(data[0], parameters)
{
    if (data.size() > 1)
        throw std::invalid_argument("Analysis of observable with multiple columns not implemented yet!");
}

ObservableAnalyzer::~ObservableAnalyzer()
{
    PrintResultOfAnalysisToOutput();
    PrintResultOfAnalysisToFile();
}

static void PrintQuantityToOutput(std::string name, const Quantities& quantities, Color::Code color)
{
    PrintRepeatedSymbol();
    std::cout << "# " << color << name << ": " << Color::FG_LIGHT_CYAN << quantities[name].value << Color::DEFAULT << "\n";
}

void ObservableAnalyzer::PrintResultOfAnalysisToOutput()
{
    if (! parameters.doNotAnalyzeMean)
        PrintQuantityToOutput(constants::observableName<Mean>, quantities, Color::FG_LIGHT_BLUE);
    if (! parameters.doNotAnalyzeVariance)
        PrintQuantityToOutput(constants::observableName<Variance>, quantities, Color::FG_LIGHT_YELLOW);
    if (! parameters.doNotAnalyzeSkewness)
        PrintQuantityToOutput(constants::observableName<Skewness>, quantities, Color::FG_LIGHT_GREEN);
    if (! parameters.doNotAnalyzeKurtosis)
        PrintQuantityToOutput(constants::observableName<Kurtosis>, quantities, Color::FG_LIGHT_MAGENTA);
    PrintRepeatedSymbol();
}
void ObservableAnalyzer::PrintResultOfAnalysisToFile()
{
    std::ofstream outputstream;
    std::string filename = getFilenameForObservables(parameters);
    outputstream.open(filename.c_str(), std::ios::app);
    if (outputstream.is_open()) {
        std::cout << "# Writing observable analysis results to file \"" << filename << "\"" << std::endl;
        outputstream << quantities.getMetaInformation() << "\n";
        outputstream << quantities.getObservablesAsString() << '\n';
        outputstream.close();
    } else {
        throw std::invalid_argument("Could not open file for observable. Aborting!");
    }
}
