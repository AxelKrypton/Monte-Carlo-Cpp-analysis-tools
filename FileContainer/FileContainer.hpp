/**
 * Definition of the dataSample class
 */

#ifndef FILECONTAINER_HPP_
#define FILECONTAINER_HPP_

#include<iostream>
#include<vector>
#include<fstream>
#include<utility>      // std::pair, std::make_pair
#include<stdexcept>

class FileContainer{
public:

	FileContainer(std::vector<std::string> filenames, bool input=true);
	~FileContainer();

	//Todo: Speak with Christopher about the default constructors (c++11)
	//FileContainer& operator=(const FileContainer&) = delete;
	//FileContainer(const FileContainer&) = delete;
	//FileContainer() = delete;

	void printReportStateOfFiles();
	std::fstream* getOpenFile(int indexOfTheFile);
	void closeOpenFile(int indexOfTheFile);

private:
	std::vector<std::pair<std::fstream*, std::string> > files;
	bool input;
};






#endif //FILECONTAINER_HPP_
