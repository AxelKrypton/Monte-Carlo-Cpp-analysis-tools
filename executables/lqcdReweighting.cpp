/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2015-2016,2020 Alessandro Sciarra
 *  Copyright (c) 2019 David Leemueller
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

#include "../IO/io_utilities_reweighting.hpp"
#include "../Parameters/LqcdReweightingParameters.hpp"
#include "../Reweighting/Reweighter.hpp"
#include "exceptions.hpp"
#include "exitCodes.hpp"

int main(int argc, const char** argv)
{
    try {
        LqcdReweightingParameters parameters(argc, argv);
        Reweighter reweighter(parameters);
        std::vector<std::vector<Quantities>> reweightedObservables = reweighter.getReweightedObservables();
        std::vector<std::vector<realFloat>> newBetaValues = reweighter.getValuesOfNewParameters();
        // TODO: Think weather the following Output functions should be a responsibility of ReweighterIO
        writeLqcdReweightedObservablesToFile(newBetaValues, reweightedObservables, parameters.getOutputfilePrefix());
        if (parameters.getPrintEstimatorsToFile()) {
            std::vector<std::vector<std::map<std::string, DataSample>>> reweightedObservablesEstimators
                = reweighter.getReweightedObservablesEstimators();
            writeLqcdReweightedObservablesEstimatorsToFile(newBetaValues, reweightedObservablesEstimators, parameters.getOutputfilePrefix());
        }
        if (! parameters.getDeactivateReweightingForProbabilityDistribution()) {
            std::vector<std::vector<ProbabilityDistribution>> reweightedProbabilityDistributions
                = reweighter.getReweightedProbabilityDistributions();
            writeLqcdReweightedProbabilityDistributionsToFile(
                newBetaValues, reweightedProbabilityDistributions, parameters.getOutputfilePrefix());
        }
        std::cout << std::endl;
    }
    // todo: move catch block into own function?
    catch (wrongBinningParameter& e) {
        std::cout << e.what() << std::endl;
        exit(EXIT_BINNING_ERROR);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(EXIT_ERROR);
    } catch (LqcdReweightingParameters::parse_aborted&) {
        // do nothing in this case...
    } catch (...) {
        std::cout << "Caught non-standard exception!\n\n";
        exit(EXIT_ERROR);
    }

    return EXIT_NORMAL;
}
