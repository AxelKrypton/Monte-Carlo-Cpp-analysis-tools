/*
 *
 *  Copyright (c) 2014 Christopher Pinke
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

#ifndef DATAFILEANALYZER_HPP_
#define DATAFILEANALYZER_HPP_

#include "../IO/io_utilities.hpp"
#include "DataSampleAnalyzer.hpp"

class ObservableAnalyzer {
  public:
    ObservableAnalyzer(std::vector<DataSample> data, Parameters parameters);
    ~ObservableAnalyzer();

  private:
    void PrintResultOfAnalysisToOutput();
    void PrintResultOfAnalysisToFile();
    Parameters parameters;
    Quantities quantities;
};

class DatafileAnalyzer {
  public:
    DatafileAnalyzer(Parameters parameters)
    {
        PrintRepeatedSymbol();
        DataSample data(parameters.file, parameters.column, parameters.offset);
        DataSampleAnalyzer analyzer(data, parameters);
        std::cout << "\n==================\n\n";
        if (parameters.calcAutocorrelation)
            AutocorrelationAnalyzer analyzer(data, parameters);
        else
            ObservableAnalyzer analyzer(std::vector<DataSample>({data}), parameters);
        PrintRepeatedSymbol();
        std::cout << "\n";
    }
};

#endif /* DATAFILEANALYZER_HPP_ */
