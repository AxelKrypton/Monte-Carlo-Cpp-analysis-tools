#include<sys/stat.h>

#include"FileContainer.hpp"


static bool checkIfFileExists(const std::string& filename){
	struct stat buffer;
	return (stat (filename.c_str(), &buffer) == 0);
}


FileContainer::FileContainer(std::vector<std::string> filenames, bool input) : input(input){
	if(input == true){
		for(size_t i = 0; i < filenames.size(); i++){
			if(!checkIfFileExists(filenames[i]))
				throw std::invalid_argument("File "+filenames[i]+" does not exist!");
		}
	}
	for(size_t i = 0; i < filenames.size(); i++)
		files.push_back(std::make_pair(new std::fstream(), filenames[i]));
}

FileContainer::~FileContainer(){
	for(size_t i = 0; i < files.size(); i++){
		if(files[i].first->is_open())
			files[i].first->close();
		delete files[i].first;
	}
}

void FileContainer::printReportStateOfFiles(){
	std::cout << std::endl << "=============================================================================================" << std::endl;
	std::cout << " FileContainer contains " << ((input == true) ? "INPUT" : "OUTPUT") << " files." << std::endl;
	for(size_t i=0; i<files.size(); i++){
		std::cout << "\tFile number " << i << " -> Name: " << files[i].second << "\t\tStatus: ";
		std::cout << ((files[i].first->is_open()) ? "OPEN" : "CLOSE") << std::endl;
	}
	std::cout << "=============================================================================================" << std::endl << std::endl;
}

/**Todo: Discuss with Christopher about this method. In particular it is quite safe to return the pointer
 *        to the fstream object, because in general one will create a FileContainer object once at the beginning
 *        of the program, he will call GetOpenFile several time to do input/output operations and then,
 *        ONLY at the end of the program, the fileContainer instance will go out of scope and the destructor
 *        will be called. The idea is that the destructor is called when ALL input/output operations have been done.
 *        What would produce an error would be something like
 *           std::vector<std::string> OutputFile;
 *           OutputFile.push_back("FileOutputForGetOpenFileTest");
 *           FileContainer fileContainer(OutputFile, false);
 *           std::fstream *openFile = fileContainer.getOpenFile(0);
 *           fileContainer.~FileContainer();
 *           std::cout << ((openFile->is_open()) ? "File still open!" : "File close!") << std::endl;
 *        because in the last line the pointer openFile points to an area of memory that has been deleted by the destructor.
 */
std::fstream* FileContainer::getOpenFile(int indexOfTheFile){
	if(files.at(indexOfTheFile).first->is_open() == false)
		files[indexOfTheFile].first->open(files[indexOfTheFile].second.c_str(), (input == true) ? std::fstream::in : std::fstream::out);
	return files[indexOfTheFile].first;
}

void FileContainer::closeOpenFile(int indexOfTheFile){
	if(files.at(indexOfTheFile).first->is_open())
		files[indexOfTheFile].first->close();
}

