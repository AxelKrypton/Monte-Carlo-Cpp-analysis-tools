#include "../dataAnalysisUtilities/datafileAnalyzer.hpp"
#include "exceptions.hpp"

int main(int argc, const char ** argv)
{
	try
	{
		Parameters parameters(argc, argv);
	    DatafileAnalyzer analyzer(parameters.file, parameters);
	}
	//todo: move catch block into own function?
	catch ( wrongBinningParameter &e)
	{
		std::cout << e.what() << std::endl;
		exit(2);
	}
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(1);
	}
	catch(Parameters::parse_aborted)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
		exit(1);
	}

    return 0;
}
