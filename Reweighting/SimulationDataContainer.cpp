#include "SimulationDataContainer.hpp"


static void checkIfDatafileExists(std::string);
static bool isAnyMapEmpty(std::vector<std::map<std::string, double> >);

/*****************************************************************************************/

SimulationDataContainer::SimulationDataContainer()
{
	throw std::invalid_argument("SimulationData needs input file for construction!");
}

SimulationDataContainer::SimulationDataContainer(std::string informationFile)
{
	checkIfDatafileExists(informationFile);
	std::vector<std::string> dataFilenames;
	std::vector<std::map<std::string, double> > dataParameters;
	extractInformationFromFile(informationFile, dataFilenames, dataParameters);
	if(isAnyMapEmpty(dataParameters))
		throw std::logic_error("In given file \"" + informationFile + "\", no parameters for at least one simulation!");
	for (size_t i = 0; i < dataFilenames.size(); i++)
		simulationDataSet.push_back(SimulationData(dataParameters[i], dataFilenames[i]));
}

SimulationData& SimulationDataContainer::operator[](int index)
{
	return simulationDataSet.at(index);
}

int SimulationDataContainer::getNumberOfDatafiles()
{
	return simulationDataSet.size();
}

int SimulationDataContainer::getNumberOfSimulationParameters(int fileNumber)
{
	return simulationDataSet[fileNumber].getNumberOfSimulationParameters();
}

void SimulationDataContainer::extractInformationFromFile(std::string fileIn,
    std::vector<std::string>& dataFilenames, std::vector<std::map<std::string, double> >& dataParameters)
{
	std::ifstream infile(fileIn.c_str());
	if(!infile.is_open())
		throw std::invalid_argument("Given file \"" + fileIn + "\" failed to be opened!");

	std::string line;
	while (std::getline(infile, line))	{
		//ignore lines beginning by # since they are comments for gnuplot and empty lines
		if(line.find_first_not_of("#\t ") < line.find_first_of("#")){
			line = line.substr(0, line.find("#", 0)); //remove everything after '#' in each line

			std::stringstream ss(line);
			double auxForParameterValue;
			std::string auxForParameterName, auxForFilename;
			std::map<std::string, double> auxMap;

			(ss >> auxForFilename) ? dataFilenames.push_back(auxForFilename)
					               : throw std::runtime_error("Error reading datafile \"" + fileIn + "\"");

			while(ss.good() && (ss.str().substr(ss.tellg())).find_first_not_of(" \t\n") != std::string::npos){
				if(ss >> auxForParameterName >> auxForParameterValue)
					auxMap[auxForParameterName] = auxForParameterValue;
				else
					throw std::runtime_error("Error reading datafile \"" + fileIn + "\"");
			}

			dataParameters.push_back(auxMap);
		}
	}
	if(!(infile.peek() == EOF && infile.eof()) || infile.bad())
	{
		throw std::runtime_error("Error reading datafile");
	}
	infile.close();
}

/*****************************************************************************************/

//todo: use stat of sys/stat.h to check existence and access to the file
void checkIfDatafileExists(std::string filename){
	std::ifstream file;
	file.open(filename.c_str());
	if ( !file.is_open() )
		throw std::invalid_argument("Given file \"" + filename + "\" does not exist!");
	file.close();
}

static bool isAnyMapEmpty(std::vector<std::map<std::string, double> > dataParameters){
	for(size_t i=0; i<dataParameters.size(); i++){
		if(dataParameters[i].size() == 0)
			return true;
	}
	return false;
}

