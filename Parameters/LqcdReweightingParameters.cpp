#include "LqcdReweightingParameters.hpp"
#include "HelperTools.hpp"

static std::string getHelpDescription(std::string);

LqcdReweightingParameters::LqcdReweightingParameters(int argc, const char ** argv) 
{
    po::options_description desc("\nOptions for reweighting data from LQCD simulations.\nNOTE: "
                                 "Reweighting is currently implemented in beta only!\nUsage: \"--<optionName>=<value>\" "
                                 "(or \"-<shortOptionName><value>\")\nNote that boolean options can be changed from their default value implicitly, "
                                 "ie without giving explicitly true or false in the command line.\nFor example, \"--doNotUseBinning\" equals "
                                 "\"--doNotUseBinning true\" (as the default value is false)", getTerminalWidth()/10*9);
    po::variables_map vm;
    po::positional_options_description positionalOptions;

    desc.add_options()
        ("help,h", "Produce this help message")
        ("file,f", po::value<std::string>(&inputfile), "Inputfile containing metainformation for reweighting procedure.")
        ("outputfilePrefix", po::value<std::string>(&outputfilePrefix)->default_value("reweightedData"), "Prefix for output file.")
        ("numberOfNewBetaPoints", po::value<unsigned int>(&numberOfNewBetaPoints)->default_value(2), "Number of new points to produce with reweighting.")
        ("newBetaRange_high", po::value<realFloat>(&newBetaRange_high)->default_value(2), "Upper limit of new beta range of to cover with reweighting.")
        ("newBetaRange_low", po::value<realFloat>(&newBetaRange_low)->default_value(1), "Lower limit of new beta range of to cover with reweighting.")
        ("deactivateReweightingForMean", po::value<bool>(&deactivateReweightingForMean)->default_value(false)->implicit_value(true), "Do not perform reweighting for the mean of the data (this implies --deactivateReweightingForProbabilityDistribution).")
        ("deactivateReweightingForVariance", po::value<bool>(&deactivateReweightingForVariance)->default_value(false)->implicit_value(true), "Do not perform reweighting for the variance of the data.")
        ("deactivateReweightingForSkewness", po::value<bool>(&deactivateReweightingForSkewness)->default_value(false)->implicit_value(true), "Do not perform reweighting for the skewness of the data.")
        ("deactivateReweightingForKurtosis", po::value<bool>(&deactivateReweightingForKurtosis)->default_value(false)->implicit_value(true), "Do not perform reweighting for the kurtosis of the data.")
        ("obsMultipleColumns", po::value<std::vector<unsigned int> >(&columnsToBeReweightedUsingMultipleColumns)->multitoken(), getHelpDescription("obsMultipleColumns").c_str())
        ("numberOfMultipleColumnsForSingleObservable", po::value<unsigned int>(&numberOfMultipleColumnsForSingleObservable)->default_value(0), "Number of columns to be considered referred to the same observable.")
        ("isMeanKnownToBeZero", po::value<bool>(&isMeanKnownToBeZero)->default_value(false)->implicit_value(true), "ALL observables are known a priori to have zero mean.")
        ("useJackknifeAsErrorMethod", po::value<bool>(&useJackknifeAsErrorMethod)->default_value(false)->implicit_value(true), "Evaluate error in reweighting using Jackknife. ATTENTION: This method implies to use the biggest binsize for all raw data points. Unless the statistics is such that the number of uncorrelated data points is not affected, this method will in general overestimate the errors!!!")
        ("useBootstrapAsErrorMethod", po::value<bool>(&useBootstrapAsErrorMethod)->default_value(false)->implicit_value(true), "Evaluate error in reweighting using Bootstrap.")
        ("numberOfBootstrapResample", po::value<int>(&numberOfBootstrapResample)->default_value(100), "Number of resamples to be done in the bootstrap.")
        ("printEstimatorsToFile", po::value<bool>(&printEstimatorsToFile)->default_value(false)->implicit_value(true), "Print the observables estimators out of which the error is calculated to a file.")
        ("useSimulatedPointsAsNewPoints", po::value<bool>(&useSimulatedPointsAsNewPoints)->default_value(false)->implicit_value(true), "Perform reweighting evaluating the observables at the simulated points (given in the configuration file).")
        ("weightPrecision", po::value<realFloat>(&weightPrecision)->default_value(1.e-7), "Precision for iterative finding of optimal weights.")
        ("deactivateReweightingForProbabilityDistribution", po::value<bool>(&deactivateReweightingForProbabilityDistribution)->default_value(false)->implicit_value(true), "Do not perform reweighting for the probability distribution of the observables.")
        ("binsizeProbabilityDistribution", po::value<realFloat>(&binsizeProbabilityDistribution)->default_value(1.e-3), "Size of the bins of the probability distribution.");

    //option "file" can be given without option description
    positionalOptions.add("file", 1);
    po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);
    po::notify(vm);

    checkParsedArguments(vm, desc);
    printParameters();
}

void LqcdReweightingParameters::checkParsedArguments(po::variables_map & vm, po::options_description & desc)
{
    if(vm.count("help")) { // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options as to why this is done before po::notifiy(vm)
        std::cout << desc << '\n';
        throw LqcdReweightingParameters::parse_aborted();
    }

    if (! vm.count("file"))
    {
        throw std::invalid_argument("No datafile given. Aborting!");
    }

    /*
     * The following two checks should be transformed in variadic templates or functions that take
     * a std::initializer_list<bool> in case the error method are more than 2.
     */

    if ( vm["useJackknifeAsErrorMethod"].defaulted() && vm["useBootstrapAsErrorMethod"].defaulted() )
    {
        throw std::invalid_argument("No error method specified. Aborting!");
    }

    if ( !vm["useJackknifeAsErrorMethod"].defaulted() && !vm["useBootstrapAsErrorMethod"].defaulted() )
    {
        throw std::invalid_argument("More than one error method specified. Aborting!");
    }

    if( !vm["deactivateReweightingForMean"].defaulted() )
    {
        deactivateReweightingForProbabilityDistribution=deactivateReweightingForMean;
    }
}

void LqcdReweightingParameters::printParameters()
{
    std::string separator = "###########################################################";

    std::cout << separator << std::endl;
    std::cout << "# Options:" << std::endl;
    std::cout << separator << std::endl;
    std::cout << "# Inputfile:\t\"" << inputfile << "\"" << std::endl;
    std::cout << separator << std::endl;
    std::cout << "# Reweighting parameters:" << std::endl;
    if(useSimulatedPointsAsNewPoints)
        std::cout << "#   Reweight at simulated points" << std::endl;
    else{
        std::cout << "#   New beta range:\t[" << newBetaRange_low << ":" << newBetaRange_high << "]" << std::endl;
        std::cout << "#   New beta points:\t  " << numberOfNewBetaPoints << std::endl;
    }
    if(isMeanKnownToBeZero)
        std::cout << "#   Mean of the observables known to be ZERO" << std::endl;
    std::cout << "#   Columns of obs. to be rew. with multiple columns:  ";
    for(size_t i=0; i<columnsToBeReweightedUsingMultipleColumns.size(); i++)
        std::cout << columnsToBeReweightedUsingMultipleColumns[i] << " ";
    std::cout << std::endl;
    std::cout << "#   Precision to determine reweighting weights:\t" << weightPrecision << std::endl;
    std::cout << "#   Error method used:  ";
    if(useJackknifeAsErrorMethod) std::cout << "Jackknife\n";
    if(useBootstrapAsErrorMethod) std::cout << "Bootstrap (" << numberOfBootstrapResample << " resample)\n";
    std::cout << separator << std::endl;
    std::cout << "# Observables:" << std::endl;
    if ( deactivateReweightingForMean )
    {
        std::cout << "#\tDo NOT reweight mean of data" << std::endl;
    }
    else
    {
        std::cout << "#\tMean of data" << std::endl;
    }
    if ( deactivateReweightingForVariance )
    {
        std::cout << "#\tDo NOT reweight variance of data" << std::endl;
    }
    else
    {
        std::cout << "#\tVariance of data" << std::endl;
    }
    if ( deactivateReweightingForSkewness )
    {
        std::cout << "#\tDo NOT reweight skewness of data" << std::endl;
    }
    else
    {
        std::cout << "#\tSkewness of data" << std::endl;
    }
    if ( deactivateReweightingForKurtosis )
    {
        std::cout << "#\tDo NOT reweight kurtosis of data" << std::endl;
    }
    else
    {
        std::cout << "#\tKurtosis of data" << std::endl;
    }
    std::cout << separator << std::endl;
    if( deactivateReweightingForProbabilityDistribution )
    {
        std::cout << "#\tDo NOT reweight probability distribution of observables" << std::endl;
    }
    else
    {
        std::cout << "#\tProbability distribution of observables" << std::endl;
    }
    std::cout << "#   Binsize for reweighting probability distribution:\t" << binsizeProbabilityDistribution << std::endl;
}

unsigned int LqcdReweightingParameters::getNumberOfNewBetaPoints()
{
    return numberOfNewBetaPoints;
}

std::string LqcdReweightingParameters::getInputfile()
{
    return inputfile;
}

std::string LqcdReweightingParameters::getOutputfilePrefix()
{
    return outputfilePrefix;
}

realFloat LqcdReweightingParameters::getNewBetaRange_low()
{
    return newBetaRange_low;
}

realFloat LqcdReweightingParameters::getNewBetaRange_high()
{
    return newBetaRange_high;
}

bool LqcdReweightingParameters::getDeactivateReweightingForMean()
{
    return deactivateReweightingForMean;
}

bool LqcdReweightingParameters::getDeactivateReweightingForVariance()
{
    return deactivateReweightingForVariance;
}

bool LqcdReweightingParameters::getDeactivateReweightingForSkewness()
{
    return deactivateReweightingForSkewness;
}

bool LqcdReweightingParameters::getDeactivateReweightingForKurtosis()
{
    return deactivateReweightingForKurtosis;
}

std::vector<unsigned int> LqcdReweightingParameters::getColumnsToBeReweightedUsingMultipleColumns(){
    return columnsToBeReweightedUsingMultipleColumns;
}

bool LqcdReweightingParameters::getUseJackknifeAsErrorMethod()
{
    return useJackknifeAsErrorMethod;
}

bool LqcdReweightingParameters::getUseBootstrapAsErrorMethod()
{
    return useBootstrapAsErrorMethod;
}

bool LqcdReweightingParameters::getUseSimulatedPointsAsNewPoints()
{
    return useSimulatedPointsAsNewPoints;
}

bool LqcdReweightingParameters::getPrintEstimatorsToFile()
{
    return printEstimatorsToFile;
}

int LqcdReweightingParameters::getNumberOfBootstrapResample()
{
    return numberOfBootstrapResample;
}

realFloat LqcdReweightingParameters::getWeightPrecision()
{
    return weightPrecision;
}

bool LqcdReweightingParameters::getIsMeanKnownToBeZero()
{
    return isMeanKnownToBeZero;
}

unsigned int LqcdReweightingParameters::getNumberOfMultipleColumnsForSingleObservable()
{
    return numberOfMultipleColumnsForSingleObservable;
}

bool LqcdReweightingParameters::getDeactivateReweightingForProbabilityDistribution()
{
    return deactivateReweightingForProbabilityDistribution;
}

realFloat LqcdReweightingParameters::getBinsizeProbabilityDistribution()
{
    return binsizeProbabilityDistribution;
}

/***************************************************************************/

static std::string getHelpDescription(std::string option){
    std::string description="";
    if(option == "obsMultipleColumns"){
        description += "Number of FIRST column in the file containing observable to be reweighted using several columns for higher moments. ";
        description += "Do not forget that the first column (column 0) is reserved for the gauge action. In general the expected ";
        description += "number of columns for the same observable is equal to the maximum moment needed in the reweighting of the ";
        description += "required quantities (e.g. 3 if only mean and skewness are asked to be reweighted). Use the option ";
        description += "--numberOfMultipleColumnsForSingleObservable whenever you want to force the program to consider a different ";
        description += "number of columns for single observable. ATTENTION: Column ranges start from ZERO!";
    }else
        throw std::invalid_argument("Unknown option in \"getHelpDescription\" function!");
    return description;
}
