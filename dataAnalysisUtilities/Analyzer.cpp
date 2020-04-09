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

#include "Analyzer.hpp"

#include "../IO/io_utilities.hpp"
#include "dataAnalysisUtilities.hpp"

#include <fstream>

DatafileAnalyzer::DatafileAnalyzer(Parameters parameters)
{
    PrintRepeatedSymbol();
    std::vector<DataSample> data;
    for (int i = 0; i < parameters.numberOfColumnsToBeConsidered; i++)
        data.push_back(DataSample{parameters.file, parameters.column + i, parameters.offset});
    if (parameters.calcAutocorrelation)
        AutocorrelationAnalyzer analyzer(data[0], parameters);
    else
        ObservableAnalyzer observableAnalyzer(data, parameters);
    PrintRepeatedSymbol();
    std::cout << "\n";
}

ObservableAnalyzer::ObservableAnalyzer(std::vector<DataSample> data, Parameters parameters)
    : parameters(parameters)
    , quantities(data, parameters)
{
}

ObservableAnalyzer::~ObservableAnalyzer()
{
    PrintResultOfAnalysisToOutput();
    PrintResultOfAnalysisToFile();
}

static void PrintQuantityToOutput(std::string name, const Quantities& quantities)
{
    PrintRepeatedSymbol();
    std::ios oldState(nullptr);
    oldState.copyfmt(std::cout);
    std::ostringstream value;
    operator<<(value << std::scientific << std::setprecision(6), {quantities[name].value, " ± "});
    std::cout << "# " << Color::observables.at(name) << name << ": " << Color::FG_LIGHT_CYAN << value.str() << Color::DEFAULT << "\n";
    std::cout.copyfmt(oldState);
}

void ObservableAnalyzer::PrintResultOfAnalysisToOutput()
{
    if (! parameters.doNotAnalyzeMean)
        PrintQuantityToOutput(constants::observableName<Mean>, quantities);
    if (! parameters.doNotAnalyzeVariance)
        PrintQuantityToOutput(constants::observableName<Variance>, quantities);
    if (! parameters.doNotAnalyzeSkewness)
        PrintQuantityToOutput(constants::observableName<Skewness>, quantities);
    if (! parameters.doNotAnalyzeKurtosis)
        PrintQuantityToOutput(constants::observableName<Kurtosis>, quantities);
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

//----------------------------------------------------------------------------------------------------------------------

AutocorrelationAnalyzer::AutocorrelationAnalyzer(DataSample& sample, Parameters parameters)
    : estimateName("AUTOCORRELATION")
    , outputFilename(getFilenameForObservables(parameters))
{
    PrintRepeatedSymbol();
    std::cout << "# Analyse " << Color::FG_LIGHT_CYAN << estimateName << Color::DEFAULT << "...\n";
    calcAutocorrelationAndErrorOfDataSample(sample, parameters);
}
