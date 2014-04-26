#include "../dataAnalysisUtilities/datafileAnalyzer.hpp"

int main(int argc, char ** argv)
{
	try
	{
		Parameters parameters(argc, argv);
	    DatafileAnalyzer analyzer(parameters.file, parameters);
	}
	//todo: move catch block into own function?
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(Parameters::parse_aborted)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
	}

    return 0;
}
