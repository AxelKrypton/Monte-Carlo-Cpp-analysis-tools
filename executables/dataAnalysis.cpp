#include "../dataAnalysisUtilities/dataAnalyzer.hpp"

int main(int argc, char ** argv)
{
	try
	{
		parameters params(argc, argv);
		//todo: move binning here?
		DataSampleAnalyzer dataSample(params.file);
	    dataAnalyzer analyzer(dataSample, params);
	}
	//todo: move catch block into own function?
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(parameters::parse_aborted)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
	}

    return 0;
}
