// use the boost test framework
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FileContainer
#include <boost/test/unit_test.hpp>
#include <stdlib.h> //system

#include "FileContainer.hpp"

BOOST_AUTO_TEST_SUITE(build)

	BOOST_AUTO_TEST_CASE(build1){
		std::vector<std::string> notExistingFile;
		notExistingFile.push_back("FileThatDoesNotExist");
		BOOST_REQUIRE_THROW(FileContainer fileContainer(notExistingFile), std::invalid_argument);
	}
	BOOST_AUTO_TEST_CASE(build2){
		std::vector<std::string> ExistingFile;
		ExistingFile.push_back("input_existing_file");
		BOOST_REQUIRE_NO_THROW(FileContainer fileContainer(ExistingFile));
	}
	BOOST_AUTO_TEST_CASE(build3){
		std::vector<std::string> OutputFile;
		OutputFile.push_back("FileOutputForBuildTest");
		BOOST_REQUIRE_NO_THROW(FileContainer fileContainer(OutputFile, false));
	}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(getOpenFile)

	BOOST_AUTO_TEST_CASE(getOpenFile1){
		std::vector<std::string> ExistingFile;
		ExistingFile.push_back("input_existing_file");
		FileContainer fileContainer(ExistingFile);
		int invalidIndex = 1;
		BOOST_REQUIRE_THROW(std::fstream *openFile = fileContainer.getOpenFile(invalidIndex), std::out_of_range	);
	}
	BOOST_AUTO_TEST_CASE(getOpenFile2){
		std::vector<std::string> ExistingFile;
		ExistingFile.push_back("input_existing_file");
		FileContainer fileContainer(ExistingFile);
		std::fstream *openFile;
		BOOST_REQUIRE_NO_THROW(openFile = fileContainer.getOpenFile(0));
		BOOST_REQUIRE(openFile->is_open());
	}
	BOOST_AUTO_TEST_CASE(getOpenFile3){
		std::vector<std::string> OutputFile;
		OutputFile.push_back("FileOutputForGetOpenFileTest");
		FileContainer fileContainer(OutputFile, false);
		std::fstream *openFile = fileContainer.getOpenFile(0);
		BOOST_REQUIRE(openFile->is_open());
		openFile->close();
		system("rm FileOutputForGetOpenFileTest");
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(closeOpenFile)

	BOOST_AUTO_TEST_CASE(closeOpenFile1){
		std::vector<std::string> ExistingFile;
		ExistingFile.push_back("input_existing_file");
		FileContainer fileContainer(ExistingFile);
		std::fstream *openFile = fileContainer.getOpenFile(0);
		int invalidIndex = 1;
		BOOST_REQUIRE_THROW(fileContainer.closeOpenFile(invalidIndex), std::out_of_range);
	}
	BOOST_AUTO_TEST_CASE(closeOpenFile2){
		std::vector<std::string> ExistingFile;
		ExistingFile.push_back("input_existing_file");
		FileContainer fileContainer(ExistingFile);
		std::fstream *openFile = fileContainer.getOpenFile(0);
		BOOST_REQUIRE_NO_THROW(fileContainer.closeOpenFile(0));
		BOOST_REQUIRE(openFile->is_open() == false);
	}
	BOOST_AUTO_TEST_CASE(closeOpenFile3){
		std::vector<std::string> OutputFile;
		OutputFile.push_back("FileOutputForGetOpenFileTest");
		FileContainer fileContainer(OutputFile, false);
		std::fstream *openFile = fileContainer.getOpenFile(0);
		fileContainer.closeOpenFile(0);
		BOOST_REQUIRE(openFile->is_open() == false);
		system("rm FileOutputForGetOpenFileTest");
	}

BOOST_AUTO_TEST_SUITE_END()
