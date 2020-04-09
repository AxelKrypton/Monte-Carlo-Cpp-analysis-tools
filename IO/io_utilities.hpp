/*
 *
 *  Copyright (c) 2014-2015 Christopher Pinke
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

#ifndef IO_UTILITIES_HPP_
#define IO_UTILITIES_HPP_

#include "../Parameters/Parameters.hpp"
#include "../Quantities/Constants.hpp"

#include <iostream>

void PrintRepeatedSymbol(char symbol = '-', int times = 80);
std::string getFilenameForObservables(Parameters parameters);

void printEstimateAndError(std::string estimateName, realFloat estimateValue, realFloat errorValue);
void writeEstimateAndErrorToFile(std::string estimateName, realFloat estimateValue, realFloat errorValue, std::string filename);
void writeEstimateAndErrorArraysToFile(std::string estimateName, std::vector<realFloat> estimate, std::vector<realFloat> error,
                                       std::string filename);

namespace Color {
    enum Code {
        DEFAULT = 0,
        FG_DEFAULT = 39,
        FG_BLACK = 30,
        FG_RED = 31,
        FG_GREEN = 32,
        FG_YELLOW = 33,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_LIGHT_GRAY = 37,
        FG_DARK_GRAY = 90,
        FG_LIGHT_RED = 91,
        FG_LIGHT_GREEN = 92,
        FG_LIGHT_YELLOW = 93,
        FG_LIGHT_BLUE = 94,
        FG_LIGHT_MAGENTA = 95,
        FG_LIGHT_CYAN = 96,
        FG_WHITE = 97,
        BG_BLACK = 41,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_BLUE = 44,
        BG_DEFAULT = 49,
    };
    const std::map<std::string, Color::Code> observables = {{constants::observableName<Mean>, FG_LIGHT_BLUE},
                                                            {constants::observableName<Variance>, FG_LIGHT_GRAY},
                                                            {constants::observableName<Skewness>, FG_LIGHT_GREEN},
                                                            {constants::observableName<Kurtosis>, FG_LIGHT_MAGENTA}};
    inline std::ostream& operator<<(std::ostream& os, Code code) { return os << "\033[" << static_cast<int>(code) << "m"; }
}

namespace Font {
    enum Code {
        DEFAULT = 0,
        BOLD = 1,
        UNDERLINE = 4,
        NO_BOLD = 22,
        NO_UNDERLINE = 24,
    };
    inline std::ostream& operator<<(std::ostream& os, Code code) { return os << "\033[" << static_cast<int>(code) << "m"; }
}

#endif /* IO_UTILITIES_HPP_ */
