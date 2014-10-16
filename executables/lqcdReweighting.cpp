#include "exceptions.hpp"
#include "exitCodes.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../Reweighting/Reweighter.hpp"
#include "../IO/io_utilities.hpp"

int main(int argc, const char ** argv)
{
	try
	{
		LqcdReweightingParameters parameters(argc, argv);
		
		std::vector<std::pair<double, double> > newRanges;
		std::vector< unsigned int> newNumPoints;
		newRanges.push_back(std::make_pair(parameters.getNewBetaRange_low(), parameters.getNewBetaRange_high()));
		newNumPoints.push_back(parameters.getNumberOfNewBetaPoints());
		
		Reweighter reweighter(parameters.getInputfile(), newRanges, newNumPoints);
		
		std::vector<std::vector<EstimateAndError> > reweightedObservables = reweighter.calculateAndGetReweightedObservables();
		std::vector<std::vector<double> > newBetaValues = reweighter.getValuesOfNewParameters();
		
		writeReweightingResultsToFile(newBetaValues, reweightedObservables );
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
