/*
 *
 *  Copyright (c) 2014,2020 Alessandro Sciarra
 *  Copyright (c) 2014-2015 Christopher Pinke
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

/**
 * Wrapper class for reading of datafiles in DataSample class.
 */

#ifndef FILEREADER_HPP_
#define FILEREADER_HPP_

#include "../IO/io_utilities.hpp"

#include <fstream>
#include <sstream>

static void checkIfDatafileExists(std::string filename)
{
    std::ifstream file;
    file.open(filename.c_str());
    if (! file.is_open())
        throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
    file.close();
}

static void checkIfColumnIsValid(int column)
{
    if (column <= 0)
        throw std::invalid_argument("Number of column must be bigger than zero!");
}

static void checkIfOffsetIsValid(int offset)
{
    if (offset < 0)
        throw std::invalid_argument("Offset must be greater than or equal to zero!");
}

class FileReader {
  public:
    FileReader() = delete;
    FileReader(std::string filename, int column, int offset)
        : filename(filename)
        , column(column)
        , offset(offset)
        , lineCounter(0)
        , anyBadEntry(false)
    {
        checkIfDatafileExists(filename);
        checkIfColumnIsValid(column);
        checkIfOffsetIsValid(offset);

        infile.open(filename.c_str());
    }

    ~FileReader() { infile.close(); }

    DataSampleBasic readDataFromFile(bool* isAnyEntryBad = NULL)
    {
        goTroughFile();

        checkOnFileStreamError();
        checkIfReadWasSuccessful();

        if (isAnyEntryBad != NULL)
            *isAnyEntryBad = anyBadEntry;

        std::cout << "# Read " << Color::FG_LIGHT_CYAN << data.size() << Color::DEFAULT << " data points from column " << Color::FG_LIGHT_CYAN
                  << column << Color::DEFAULT << " of file " << Color::FG_LIGHT_CYAN << filename << Color::DEFAULT << "\n";

        return DataSampleBasic(std::valarray<realFloat>(data.data(), data.size()));
    }

  private:
    void goTroughFile()
    {
        while (std::getline(infile, line)) {
            // ignore lines beginning by # since they are comments for gnuplot and empty lines
            if (line.find_first_not_of("#\t ") < line.find_first_of("#")) {
                line = line.substr(0, line.find("#", 0));  // remove everything after '#' in each line
                if (! (lineCounter < offset))
                    extractContentFromLine();
                lineCounter++;
            }
        }
    }

    void checkIfReadWasSuccessful()
    {
        if (data.size() == 0) {
            std::ostringstream ss;
            ss << column;
            if (anyBadEntry)
                throw std::invalid_argument("datafile \"" + filename + "\", does not contain valid data in column " + ss.str() + "!");
            else
                throw std::out_of_range("datafile \"" + filename + "\", does not contain column " + ss.str() + "!");
        }
    }

    void checkOnFileStreamError()
    {
        // todo: this is not covered in a test yet...
        if (! (infile.peek() == EOF && infile.eof()) || infile.bad()) {
            throw std::runtime_error("Error reading datafile");
        }
    }

    void extractContentFromLine()
    {
        std::string currentEntry;
        realFloat currentNumber = 0.;
        std::stringstream ss(line);
        for (int i = 0; i < column; i++) {
            if (ss >> currentEntry)  // TODO: think if this if is really needed
            {
                if (i == column - 1) {
                    currentNumber = std::atof(currentEntry.c_str());
                    if (currentNumber != 0.0)
                        data.push_back(currentNumber);
                    else if (currentEntry.find_first_not_of("0.") != std::string::npos)
                        anyBadEntry = true;
                    else
                        data.push_back(0.0);
                }
            }
        }
    }

    std::ifstream infile;
    std::string line;
    std::string filename;
    std::vector<realFloat> data;
    int column;
    int offset;
    int lineCounter;
    bool anyBadEntry;
};

#endif /* FILEREADER_HPP_ */
