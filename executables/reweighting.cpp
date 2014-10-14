#include "../Parameters/Parameters.hpp"
#include "exceptions.hpp"
#include "exitCodes.hpp"
#include "../Reweighting/Reweighter.hpp"

int main(int argc, const char ** argv)
{
	try
	{
		Parameters parameters(argc, argv);
		Reweighter reweighter(parameters.file);
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
	catch(Parameters::parse_aborted)
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
