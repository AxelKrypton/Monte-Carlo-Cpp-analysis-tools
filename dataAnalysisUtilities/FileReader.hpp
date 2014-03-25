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
		filename(filename), column(column), offset(offset), lineCounter(0)
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

	DataSample readDataFromFile()
	{
		goTroughFile();

		checkOnFileStreamError();
		checkIfReadWasSuccessful();

		return DataSample( std::valarray<double>(data.data(), data.size()) );
	}

private:
	void goTroughFile()
	{
		while (std::getline(infile, line))
		{
			if(line[0] != '#' && !( line.empty() ) ) //ignore comment or empty lines
			{
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
			throw std::logic_error("datafile \"" + filename + "\", does not contain valid data in column " + ss.str() + "!");
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
		double currentNumber = 0.;
		std::stringstream ss (line);
		for(int i=0; i<column; i++)
		{
			if(ss >> currentNumber)
			{
				if(i==column-1)
					data.push_back(currentNumber);
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
};

#endif /* FILEREADER_HPP_ */
