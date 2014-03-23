#include <iostream>
#include <fstream>
#include <sstream>
#include "dataSample.hpp"

DataSample::DataSample(int length)
{
	setValues(std::valarray<double>(length));
}

DataSample::DataSample(std::valarray<double> valuesIn)
{
	setValues(valuesIn);
}

DataSample::DataSample(std::string dataFilename, int column, int offset)
{
	setValues(readDataFromFile(dataFilename, column, offset));
}

void DataSample::setValues(std::valarray<double> valuesIn)
{
	checkIfNumberOfElementsIsValid(valuesIn.size());
	values = valuesIn;
	numberOfElements = valuesIn.size();
}

DataSample 	DataSample::pow(int n)
{
	return DataSample( std::pow(values, double(n)) );
}

DataSample DataSample::shiftAndPow(int n, double shift)
{
	return DataSample( std::pow((values - shift), double(n)) );
}

double DataSample::sum()
{
	return values.sum();
}

double& DataSample::operator[](size_t index)
{
	return values[index];
}


int DataSample::getNumberOfElements()
{
	return numberOfElements;
}

void DataSample::checkIfOffsetIsValid(int offset)
{
	if(offset < 0)
		throw std::invalid_argument("Offset must be greater than or equal to zero!");
	if(offset > 0)
		throw std::invalid_argument("Usage of offset parameter is not implemented yet. Aborting!");
}

void DataSample::checkIfNumberOfElementsIsValid(int length)
{
	if(length <= 0)
		throw std::invalid_argument("Cannot create dataSample with zero or less elements!");
	//todo: think about better warning!
	if(length > roughEstimateOfNumberOfEntriesWhereDoublePrecisionMayBeInvalid)
		std::cout << "Warning: the datasize is such that double precision may not be valid anymore (depending on the data)!" << std::endl;
}

void DataSample::checkIfColumnIsValid(int column)
{
	if(column <= 0)
		throw std::invalid_argument("Numbers of columns must be bigger than zero!");
}

double defaultFunction(double in)
{
	return in;
}

DataSample DataSample::applyFunction(double (*function)(double))
{
	std::valarray<double> functionAppliedToArray = values.apply(function);
	DataSample dataSample(functionAppliedToArray);
	return dataSample;
}

void DataSample::checkIfDatafileExists(std::string filename)
{
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}

//todo: refactor, perhaps think about column numbering
//todo: implement offset
std::valarray<double> DataSample::readDataFromFile(std::string filename, int column, int offset)
{
	std::ifstream infile;
	std::string line;
	std::vector<double> data;
	double aux;

	checkIfDatafileExists(filename);
	checkIfColumnIsValid(column);
	checkIfOffsetIsValid(offset);

	infile.open(filename.c_str());

	while (std::getline(infile, line))
	{
		if(line[0] != '#') //ignore lines beginning by # since they are comments for gnuplot
		{
			std::stringstream ss(line);
			for(int i=0; i<column; i++)
			{
				if(ss >> aux)
				{
					if(i==column-1)
						data.push_back(aux);
				}
				else
				{
					throw std::runtime_error("Error reading datafile");
				}
			}
		}
	}
	//todo: this is not covered in a test yet...
	//Note: this happens for example if an empty line is contained in the file
	if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
	{
		throw std::runtime_error("Error reading datafile");
	}
	infile.close();

	return std::valarray<double>(data.data(), data.size());
}

void DataSample::checkSliceParameters(int start, int size, int stride)
{
	if( start < 0 || start >= numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"start\" must be between 0 and (number of datapoints -1)!");
	if( size < 1 || size > numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"size\" must be between 1 and number of datapoints!");
	if( stride < 1 || stride >= numberOfElements)
		throw std::invalid_argument("sampleSlice parameter \"stride\" must be between 1 and number of datapoints!");
	if( (start + (stride*size) ) > numberOfElements)
		throw std::invalid_argument("product of sampleSlice parameters \"slice\" and \"stride\" must be between 1 and number of datapoints!");
}

DataSample DataSample::sampleSlice(int start, int size, int stride)
{
	checkSliceParameters(start, size, stride);
	return DataSample(values[std::slice(start, size, stride	)]);
}
