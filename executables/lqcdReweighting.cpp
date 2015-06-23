#include "exceptions.hpp"
#include "exitCodes.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../Reweighting/Reweighter.hpp"
#include "../IO/io_utilities_reweighting.hpp"


int main(int argc, const char ** argv)
{
	try
	{
		LqcdReweightingParameters parameters(argc, argv);
		Reweighter reweighter(parameters);
		std::vector<std::vector<Observables> > reweightedObservables = reweighter.getReweightedObservables();
		std::vector<std::vector<realFloat> > newBetaValues = reweighter.getValuesOfNewParameters();
		writeLqcdReweightingResultsToFile(newBetaValues, reweightedObservables, parameters.getOutputfilePrefix() );
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
	catch(LqcdReweightingParameters::parse_aborted&)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!\n\n";
		exit(EXIT_ERROR);
	}

    return EXIT_NORMAL;
}
