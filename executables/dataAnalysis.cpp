#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/datafileAnalyzer.hpp"
#include "exceptions.hpp"
#include "exitCodes.hpp"

int main(int argc, const char ** argv)
{
	try
	{
	    std::cout << std::endl << "#------------------------------------------------------------------------------------" << std::endl;
		Parameters parameters(argc, argv);
		DatafileAnalyzer analyzer(parameters);
		std::cout << "#------------------------------------------------------------------------------------" << std::endl << std::endl;
	}
	//todo: move catch block into own function?
	catch ( wrongBinningParameter &e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_BINNING_ERROR);
	}
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_ERROR);
	}
	catch(Parameters::parse_aborted&)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
		exit(EXIT_ERROR);
	}

    return EXIT_NORMAL;
}
