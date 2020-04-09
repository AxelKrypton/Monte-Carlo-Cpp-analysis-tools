/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2014-2015,2018,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "Parameters.hpp"

#include "../IO/io_utilities.hpp"
#include "../Quantities/Constants.hpp"
#include "HelperTools.hpp"

Parameters::Parameters(std::vector<std::string> argv)
{
    int argc = argv.size();
    std::vector<const char*> Ptrs;
    std::transform(std::begin(argv), std::end(argv), std::back_inserter(Ptrs), [](std::string& str) { return str.c_str(); });
    *this = Parameters{argc, Ptrs.data()};
}

Parameters::Parameters(int argc, const char** argv)
{
    po::options_description desc("\nProgram to analyze (correlated) data evaluating mean, variance, skewness and kurtosis.\n\n"
                                 "Usage: \"--<optionName>=<value>\" (or \"-<shortOptionName><value>\")\n\n"
                                 "Note that boolean options can be changed from their default value implicitly, i.e. without giving "
                                 "explicitly true or false in the command line.\n"
                                 "For example, \"--doNotUseBinning\" equals \"--doNotUseBinning=true\" (as the default value is false)",
                                 getTerminalWidth() * 10 / 9);
    po::variables_map vm;
    po::positional_options_description positionalOptions;

    // todo: Maybe it would be nicer to put the observables into a vector
    /**
     * Apparently, when using short options with int one has to do "-n99"
     * because otherwise the empty space causes an error.
     */
    // clang-format off
    desc.add_options()
        ("help,h", "Produce this help message")
        ("file,f", po::value<std::string>(&file), "File containing data")
        ("offset,o", po::value<int>(&offset)->default_value(0), "Discard first <offset> values of data")
        ("column,c", po::value<int>(&column)->default_value(1), "Read data from column number <column> (must be >= 1)")
        ("numberOfColumns", po::value<int>(&numberOfColumnsToBeConsidered)->default_value(1), "Consider multiple columns for estimators of the same observable (must be >= 1)")
        ("isMeanKnownToBeZero,m", po::value<bool>(&isMeanKnownToBeZero)->default_value(false)->implicit_value(true), "The mean for given column is set to zero in the observables' calculation")
        ("binsize,b", po::value<int>(&binsize), "Binsize used for all moments (default: 100)")
        ("binsizeMoments", po::value<std::vector<int> >(&binsizeMoments)->multitoken(), "Binsize for Nth moment (use it giving: N1 binsize N2 binsize ...)")
        ("binsizeCentralMoments", po::value<std::vector<int> >(&binsizeCentralMoments)->multitoken(), "Binsize for Nth central moment (use it giving: N1 binsize N2 binsize ...)")
        ("numberOfBins,n", po::value<int>(&numberOfBins), "Number of bins used for all observables (default: 10)")
        ("numberOfBinsMoments", po::value<std::vector<int> >(&numberOfBinsMoments)->multitoken(), "Number of bins for Nth moment (use it giving: N1 numberOfBins N2 numberOfBins ...)")
        ("numberOfBinsCentralMoments", po::value<std::vector<int> >(&numberOfBinsCentralMoments)->multitoken(), "Number of bins for Nth central moment (use it giving: N1 numberOfBins N2 numberOfBins ...)")
        ("analysisOutputFilePrefix", po::value<std::string>(&analysisOutputFilePrefix)->default_value(""), "Prefix for filename of analysis results")
        ("analysisOutputFilePostfix", po::value<std::string>(&analysisOutputFilePostfix)->default_value("_stat"), "Postfix for filename of analysis results")
        ("doNotAnalyzeMean", po::value<bool>(&doNotAnalyzeMean)->default_value(false)->implicit_value(true), "Do NOT analyze data for mean")
        ("doNotAnalyzeVariance", po::value<bool>(&doNotAnalyzeVariance)->default_value(false)->implicit_value(true), "Do NOT analyze data for variance")
        ("doNotAnalyzeSkewness", po::value<bool>(&doNotAnalyzeSkewness)->default_value(false)->implicit_value(true), "Do NOT analyze data for skewness")
        ("doNotAnalyzeKurtosis", po::value<bool>(&doNotAnalyzeKurtosis)->default_value(false)->implicit_value(true), "Do NOT analyze data for kurtosis")
        ("doNotUseBinning", po::value<bool>(&doNotUseBinning)->default_value(false)->implicit_value(true), "Do NOT perform binning on data")
        ("binningMustFitDataSampleSize", po::value<bool>(&binningMustFitDataSampleSize)->default_value(false)->implicit_value(true), "Require that no element of the data sample is discarded during binning")
        ("adjustDataSampleSizeToBinning", po::value<bool>(&adjustDataSampleSizeToBinning)->default_value(true)->implicit_value(true), "Adjust number of elements of the data sample if elements are discarded during binning")
        ("calcAutocorrelation,a", po::value<bool>(&calcAutocorrelation)->default_value(false)->implicit_value(true), "Estimate autocorrelation of data. In this case no other observable is evaluated!")
        ("numberOfBinsForAutocorrelation", po::value<int>(&numberOfBinsForAutocorrelation), "Number of bins for the estimate of the autocorrelation time (default: 10)")
        ("timeMaxAutocorrelationFunction", po::value<int>(&timeMaxAutocorrelationFunction), "Maximum data distance for the estimate of the autocorrelation function (needed parameter).")
        ;
    // clang-format on

    // option "file" can be given without option description
    positionalOptions.add("file", 1);
    po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);
    po::notify(vm);

    checkParsedArguments(vm, desc);
    printParameters();
}

void Parameters::checkParsedArguments(po::variables_map& vm, po::options_description& desc)
{
    if (vm.count("help")) {  // see http://stackoverflow.com/questions/5395503/required-and-optional-arguments-using-boost-library-program-options
                             // as to why this is done before po::notifiy(vm)
        std::cout << desc << '\n';
        throw Parameters::parse_aborted();
    }

    if (! vm.count("file")) {
        throw std::invalid_argument("No datafile given. Aborting!");
    }

    if (calcAutocorrelation && (! vm.count("timeMaxAutocorrelationFunction"))) {
        throw std::invalid_argument(
            "If calcAutocorrelation==true then the option --timeMaxAutocorrelationFunction=... must be given. Aborting!");
    }

    if (calcAutocorrelation && numberOfColumnsToBeConsidered != 1) {
        throw std::invalid_argument("No multiple estimators supported in autocorrelation analysis. Aborting!");
    }

    /**
     * For the binning one has to know if it should be
     * performed with numberOfBins or with binsize parameter.
     * That's what the variable useNumberOfBinsForBinning is for.
     * Its value should be changed automatically if a binsize or
     * a numberOfBins is given in the command line. If a default
     * value is assigned to an option, the option is always regarded
     * as set, even if the user didn't specify it. This makes
     * difficult to see what was given in the command line,
     * since "count" is always true.
     * Hence, no default value is given in the definition of
     * the options but it is assigned as done below.
     * Maybe this can be done better with the "notifier"
     * functionality of boost.
     *
     * TODO: Actually one can use the defaulted() method, i.e.
     *       check if an option was explicitly given or not via
     *       vm["optionName"].defaulted(). See the following link
     *       http://www.boost.org/doc/libs/1_58_0/doc/html/boost/program_options/variable_value.html#id1132213-bb
     *       for more information.
     */
    useNumberOfBinsForBinning = false;
    if ((vm.count("numberOfBins") || vm.count("numberOfBinsMoments") || vm.count("numberOfBinsCentralMoments"))) {
        if ((vm.count("binsize") || vm.count("binsizeMoments") || vm.count("binsizeCentralMoments"))) {
            throw std::invalid_argument(
                "Not clear what binning parameter to use, both \"numberOfBins\" and \"binsize\" have been set. Aborting!");
        }
        useNumberOfBinsForBinning = true;
    }

    /*
     * TODO: Think whether the following two blocks of code on binsize
     *       and numberOfBins should always both be executed or whether
     *       only the one according to "useNumberOfBinsForBinning" should be done.
     */
    // check if numberOfBins has been set, otherwise set them.
    if (! vm.count("numberOfBins"))
        numberOfBins = 10;
    // check structure of numberOfBins(Central)Moment and parse it
    if ((numberOfBinsMoments.size() % 2) != 0)
        throw std::invalid_argument("Option --numberOfBinsMoments incomplete!");
    parseBinningInformationForMoments(numberOfBinsMoments, numberOfBins);
    if ((numberOfBinsCentralMoments.size() % 2) != 0)
        throw std::invalid_argument("Option --numberOfBinsCentralMoments incomplete!");
    parseBinningInformationForMoments(numberOfBinsCentralMoments, numberOfBins);

    // check if binsize has been set, otherwise set them.
    if (! vm.count("binsize"))
        binsize = 100;
    // check structure of binsize(Central)Moment and parse it
    if ((binsizeMoments.size() % 2) != 0)
        throw std::invalid_argument("Option --binsizeMoments incomplete!");
    parseBinningInformationForMoments(binsizeMoments, binsize);
    if ((binsizeCentralMoments.size() % 2) != 0)
        throw std::invalid_argument("Option --binsizeCentralMoments incomplete!");
    parseBinningInformationForMoments(binsizeCentralMoments, binsize);

    // check if numberOfBinsForAutocorrelation has been set, otherwise set them.
    if (! vm.count("numberOfBinsForAutocorrelation")) {
        numberOfBinsForAutocorrelation = 10;
    }
}

void Parameters::printParameters()
{
    std::cout << std::endl;
    PrintRepeatedSymbol();
    std::cout << "# Datafile:\t" << file << std::endl;
    if (numberOfColumnsToBeConsidered == 1)
        std::cout << "# Use column:\t" << column;
    else
        std::cout << "# Use columns:\t" << column << " to " << column + numberOfColumnsToBeConsidered - 1;
    if (isMeanKnownToBeZero)
        std::cout << "\t(mean is known to be zero)";
    std::cout << std::endl;
    std::cout << "# Offset:\t" << offset << std::endl;
    // todo: add output of observables which are analyzed
    PrintRepeatedSymbol();
    if (! doNotUseBinning) {
        std::cout << "# Perform binning with:" << std::endl;
        if (useNumberOfBinsForBinning) {
            std::cout << "# Number of bins:\n";
            for (size_t i = 1; i < numberOfBinsMoments.size(); i++)
                std::cout << "#   - moment " << i << ": " << numberOfBinsMoments[i] << std::endl;
            for (size_t i = 1; i < numberOfBinsCentralMoments.size(); i++)
                std::cout << "#   - central moment " << i << ": " << numberOfBinsCentralMoments[i] << std::endl;
        } else {
            std::cout << "# Binsize:\n";
            for (size_t i = 1; i < binsizeMoments.size(); i++)
                std::cout << "#   - moment " << i << ": " << binsizeMoments[i] << std::endl;
            for (size_t i = 1; i < binsizeCentralMoments.size(); i++)
                std::cout << "#   - central moment " << i << ": " << binsizeCentralMoments[i] << std::endl;
        }
        if (binningMustFitDataSampleSize) {
            PrintRepeatedSymbol();
            std::cout << "# Require binsize/numberOfBins\n#   to be multiple of number of\n#   data points" << std::endl;
        }
        if (adjustDataSampleSizeToBinning) {
            PrintRepeatedSymbol();
            std::cout << "# Resize raw data sample in case data points are discarded during binning" << std::endl;
        }
    } else {
        std::cout << "# Do not perform binning!" << std::endl;
    }
    if (calcAutocorrelation) {
        PrintRepeatedSymbol();
        std::cout << "# Calculate estimate of autocorrelation time:" << std::endl;
        std::cout << "#  - for \"t\" up to " << timeMaxAutocorrelationFunction << "," << std::endl;
        std::cout << "#  - using " << numberOfBinsForAutocorrelation << " bins to bin the data before" << std::endl;
        std::cout << "#    applying Jackknife." << std::endl;
    }
    PrintRepeatedSymbol();
    std::cout << "\n";
}

/*
 * The following function parses the input vector where there should be the information for binning as
 *   n_1 x_1 n_2 x_2 n_3 x_3 ...
 * where n_i are the moments and x_i the binning information (either binsize or number of bins).
 * In this function the input vector is used to create an output vector that will have as many entries
 * as the max{x_i} and that will have all the entries set to the default binning information (either
 * binsize or numberOfbins) except the n_i entries that will be set to x_i.
 */
void Parameters::parseBinningInformationForMoments(std::vector<int>& vectorWithBinningInformations, const int defaultValue)
{
    std::vector<std::vector<int>> auxVector(2);
    for (size_t i = 0; i < vectorWithBinningInformations.size(); i += 2) {
        auxVector[0].push_back(vectorWithBinningInformations[i]);
        auxVector[1].push_back(vectorWithBinningInformations[i + 1]);
    }
    vectorWithBinningInformations.clear();
    /*
     * Minimum 4 moments.
     * NOTE: Entry 0 is meaningless but we keep it to have the index
     *       referring exactly to the number of the moment without a
     *       a shift by one (make rest of code more readable).
     * TODO: Improve in the sense that this 4 should not be hard coded!!
     */
    int totalNumberOfMoments = (auxVector[0].size() == 0) ? 4 : std::max(4, *std::max_element(auxVector[0].begin(), auxVector[0].end()));
    for (int i = 0; i <= totalNumberOfMoments; i++) {
        std::vector<int>::iterator it;
        it = find(auxVector[0].begin(), auxVector[0].end(), i);
        if (it != auxVector[0].end())
            vectorWithBinningInformations.push_back(auxVector[1][it - auxVector[0].begin()]);
        else
            vectorWithBinningInformations.push_back(defaultValue);
    }
}

BinningParameters Parameters::getBinningParametersForObservablesAnalysis(std::string observable) const
{
    BinningParameters returnValue;
    returnValue.performBinning = ! doNotUseBinning;
    returnValue.binningMustFitDataSample = binningMustFitDataSampleSize;
    returnValue.adjustDataSample = adjustDataSampleSizeToBinning;
    returnValue.useNumberOfBins = useNumberOfBinsForBinning;

    if (observable == constants::observableName<Mean>) {
        returnValue.number = useNumberOfBinsForBinning ? numberOfBinsMoments[1] : binsizeMoments[1];
    } else if (observable == constants::observableName<Variance>) {
        returnValue.number = useNumberOfBinsForBinning ? numberOfBinsCentralMoments[2] : binsizeCentralMoments[2];
    } else if (observable == constants::observableName<Skewness>) {
        returnValue.number = (useNumberOfBinsForBinning) ? std::min(numberOfBinsCentralMoments[2], numberOfBinsCentralMoments[3])
                                                         : std::max(binsizeCentralMoments[2], binsizeCentralMoments[3]);
    } else if (observable == constants::observableName<Kurtosis>) {
        returnValue.number = (useNumberOfBinsForBinning) ? std::min(numberOfBinsCentralMoments[2], numberOfBinsCentralMoments[4])
                                                         : std::max(binsizeCentralMoments[2], binsizeCentralMoments[4]);
    } else {
        throw std::invalid_argument("Unknown observable in getBinningParametersForObservablesAnalysis function!");
    }
    return returnValue;
}
