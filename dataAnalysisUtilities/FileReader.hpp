/**
 * Wrapper class for reading of datafiles in DataSample class.
 */

#ifndef FILEREADER_HPP_
#define FILEREADER_HPP_

static void checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}

static void checkIfColumnIsValid(int column)
{
	if(column <= 0)
		throw std::invalid_argument("Number of column must be bigger than zero!");
}

static void checkIfOffsetIsValid(int offset)
{
	if(offset < 0)
		throw std::invalid_argument("Offset must be greater than or equal to zero!");
}

class FileReader
{
public:
	FileReader()
	{
		throw std::invalid_argument("Need input file to create FileReader!");
	}
	FileReader(std::string filename, int column, int offset):
		filename(filename), column(column), offset(offset), lineCounter(0), anyBadEntry(false)
	{
		checkIfDatafileExists(filename);
		checkIfColumnIsValid(column);
		checkIfOffsetIsValid(offset);

		infile.open(filename.c_str());
	}

	~FileReader()
	{
		infile.close();
	}

	DataSample readDataFromFile(bool* isAnyEntryBad = NULL)
	{
		goTroughFile();

		checkOnFileStreamError();
		checkIfReadWasSuccessful();

		if(isAnyEntryBad != NULL)
			*isAnyEntryBad = anyBadEntry;
		return DataSample( std::valarray<double>(data.data(), data.size()) );
	}

private:
	void goTroughFile()
	{
		while (std::getline(infile, line))
		{
			//ignore lines beginning by # since they are comments for gnuplot and empty lines
			if(line.find_first_not_of("#\t ") < line.find_first_of("#")){
				line = line.substr(0, line.find("#", 0)); //remove everything after '#' in each line
				if (!(lineCounter < offset))
					extractContentFromLine();
				lineCounter++;
			}
		}
	}

	void checkIfReadWasSuccessful()
	{
		if(data.size() == 0)
		{
			std::ostringstream ss ;
			ss << column;
			if(anyBadEntry)
				throw std::invalid_argument("datafile \"" + filename + "\", does not contain valid data in column " + ss.str() + "!");
			else
				throw std::out_of_range("datafile \"" + filename + "\", does not contain column " + ss.str() + "!");
		}
	}

	void checkOnFileStreamError()
	{
		//todo: this is not covered in a test yet...
		if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
		{
			throw std::runtime_error("Error reading datafile");
		}
	}

	void extractContentFromLine()
	{
		std::string currentEntry;
		double currentNumber = 0.;
		std::stringstream ss (line);
		for(int i=0; i<column; i++)
		{
			if(ss >> currentEntry)//TODO: think if this if is really needed
			{
				if(i==column-1)
				{
					currentNumber = std::atof(currentEntry.c_str());
					if(currentNumber != 0.0)
						data.push_back(currentNumber);
					else if(currentEntry.find_first_not_of("0.") != std::string::npos)
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
	std::vector<double> data;
	double currentNumber;
	int column;
	int offset;
	int lineCounter;
	bool anyBadEntry;
};

#endif /* FILEREADER_HPP_ */
