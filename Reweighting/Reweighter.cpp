/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018-2021 Alessandro Sciarra
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

#include "Reweighter.hpp"

#include "../Quantities/Kurtosis.hpp"
#include "../Quantities/Mean.hpp"
#include "../Quantities/Moments.hpp"
#include "../Quantities/Skewness.hpp"
#include "../Quantities/Variance.hpp"
#include "MomentsReweighter.hpp"

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters);
static std::vector<int> getBinsizesToBeUsedBasedOnObservable(std::string, bool, std::vector<Binsizes>);
static std::initializer_list<unsigned int> getNeededMomentsBasedOnObservableName(std::string, bool);
static std::vector<ReweightingProcedure>
getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(std::map<std::string, std::vector<int>>, bool);
static std::vector<unsigned int> getUnionOfVectors(std::vector<unsigned int>, std::vector<unsigned int>);
static unsigned int getMaximumMomentToBeReweighted(const std::vector<ReweightingProcedure>&);
static void printInformationAboutReweightingProcedure(const ReweightingProcedure&);
static void
    checkSizesOfMomentsAndMomentsEstimators(std::vector<std::vector<Moments>>, std::vector<std::vector<MomentsEstimators>>, size_t, size_t);
static void setObservablesAtNewPointsFromMomentsAndMomentEstimators(std::vector<std::vector<Quantities>>&, bool, ErrorCalculationMethod,
                                                                    std::vector<std::vector<Moments>>,
                                                                    std::vector<std::vector<MomentsEstimators>>, std::vector<std::string>);
static void checkSizesOfHistogramsAndHistogramEstimators(std::vector<std::vector<Histogram>>, std::vector<std::vector<HistogramEstimator>>,
                                                         size_t, size_t);
static void setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(std::vector<std::vector<ProbabilityDistribution>>&,
                                                                                     ErrorCalculationMethod,
                                                                                     std::vector<std::vector<Histogram>>,
                                                                                     std::vector<std::vector<HistogramEstimator>>);
static void setObservablesEstimatorsAtNewPointsFromMomentsEstimators(std::vector<std::vector<std::map<std::string, DataSample>>>&, bool,
                                                                     std::vector<std::vector<MomentsEstimators>>, std::vector<std::string>);
static void setMeanToZeroAtNewPoints(std::vector<std::vector<Quantities>>&);

/*****************************************************************************************/

Reweighter::Reweighter(std::initializer_list<std::string> options) : reweighterIO(createLqcdParameters(options))
{
    LqcdReweightingParameters parameters = createLqcdParameters(options);
    quantitiesToBeReweighted = getQuantitiesToBeReweighted(parameters);
    precisionOfIterativeProcedureToCalculateLogZ = parameters.getWeightPrecision();
    newNumberOfPointsOfParameters
        = {parameters.getNumberOfNewBetaPoints()};  // TODO: Use method of class ReweightingParameters to be implemented!!
    newRangesOfParameters
        = {std::make_pair(parameters.getNewBetaRange_low(),
                          parameters.getNewBetaRange_high())};  // TODO: Use method of class ReweightingParameters to be implemented!!
    useSimulatedPointsAsNewPoints = parameters.getUseSimulatedPointsAsNewPoints();
    maximumMomentNeededOverall = 4;  // Hard coded for tests, TODO: think a better way
    std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
    for (auto i : quantitiesToBeReweighted)
        std::cout << i << " - ";
    std::cout << "\n";
    std::cout << "reweighterIO.valuesOfSpecifiedLogZ.size() = " << reweighterIO.valuesOfSpecifiedLogZ.size() << "  -> ";
    for (auto i : reweighterIO.valuesOfSpecifiedLogZ)
        std::cout << i << " ";
    std::cout << "\n";
}

Reweighter::Reweighter(LqcdReweightingParameters parameters)
    : reweighterIO(parameters)
    , quantitiesToBeReweighted(getQuantitiesToBeReweighted(parameters))
    , precisionOfIterativeProcedureToCalculateLogZ(parameters.getWeightPrecision())
{
    newNumberOfPointsOfParameters
        = {parameters.getNumberOfNewBetaPoints()};  // TODO: Use method of class ReweightingParameters to be implemented!!
    newRangesOfParameters
        = {std::make_pair(parameters.getNewBetaRange_low(),
                          parameters.getNewBetaRange_high())};  // TODO: Use method of class ReweightingParameters to be implemented!!
    useSimulatedPointsAsNewPoints = parameters.getUseSimulatedPointsAsNewPoints();
    std::vector<ReweightingProcedure> reweightingProceduresToBePerformed = getReweightingProceduresToBePerformed();
    if (reweightingProceduresToBePerformed.size() > 0) {
        maximumMomentNeededOverall = getMaximumMomentToBeReweighted(reweightingProceduresToBePerformed);
        // Before setting the observables, we have to reserve the correct amount of memory
        size_t numberOfNewPoints, numberOfObservablesInFiles;
        if (useSimulatedPointsAsNewPoints)
            numberOfNewPoints = reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters.size();
        else
            numberOfNewPoints = std::accumulate(
                newNumberOfPointsOfParameters.begin(), newNumberOfPointsOfParameters.end(), 1, std::multiplies<unsigned int>());
        numberOfObservablesInFiles = reweighterIO.readFromFileDataContainer[0].getNumberOfDataSample()
                                     - reweighterIO.namesOfParametersIgnoringMetaParameters.size()
                                     - reweighterIO.columnsToBeReweightedUsingMultipleColumns.size()
                                           * (reweighterIO.numberOfMultipleColumns - 1);  // neglect multiple columns (count one column only)
        observablesAtNewPoints
            = std::vector<std::vector<Quantities>>(numberOfNewPoints, std::vector<Quantities>(numberOfObservablesInFiles, Quantities()));
        if (parameters.getPrintEstimatorsToFile())
            observablesEstimatorsAtNewPoints = std::unique_ptr<std::vector<std::vector<std::map<std::string, DataSample>>>>(
                new std::vector<std::vector<std::map<std::string, DataSample>>>(
                    numberOfNewPoints,
                    std::vector<std::map<std::string, DataSample>>(numberOfObservablesInFiles, std::map<std::string, DataSample>())));
        else
            observablesEstimatorsAtNewPoints = nullptr;
        probabilityDistributionsAtNewPoints = std::vector<std::vector<ProbabilityDistribution>>(
            numberOfNewPoints, std::vector<ProbabilityDistribution>(numberOfObservablesInFiles, ProbabilityDistribution()));
        std::cout << "maximumMomentNeededOverall = " << maximumMomentNeededOverall << "\n";
        std::cout << "numberOfNewPoints = " << numberOfNewPoints << "\n";
        std::cout << "numberOfObservablesInFiles = " << numberOfObservablesInFiles << "\n";
        std::cout << "quantitiesToBeReweighted.size() = " << quantitiesToBeReweighted.size() << "  - ";
        for (auto i : quantitiesToBeReweighted)
            std::cout << i << " - ";
        std::cout << "\n";

        for (auto rewProc : reweightingProceduresToBePerformed) {
            if (reweighterIO.deactivateReweightingForProbabilityDistribution)
                rewProc.reweightProbabilityDistributions = false;
            printInformationAboutReweightingProcedure(rewProc);
            MomentsReweighter momentsReweighter(getRawDataForReweightingAndMetainformation(
                rewProc.momentsToBeReweighted, rewProc.binsizesToBeUsed, rewProc.reweightProbabilityDistributions));
            if (valuesOfNewParameters.empty())
                valuesOfNewParameters = momentsReweighter.getValuesOfNewParameters();
            std::vector<std::vector<Moments>> momentsAtNewPoints = momentsReweighter.getMomentsAtNewPoints();
            std::vector<std::vector<MomentsEstimators>> momentsEstimatorsAtNewPoints = momentsReweighter.getMomentsEstimatorsAtNewPoints();
            checkSizesOfMomentsAndMomentsEstimators(
                momentsAtNewPoints, momentsEstimatorsAtNewPoints, numberOfNewPoints, numberOfObservablesInFiles);
            setObservablesAtNewPointsFromMomentsAndMomentEstimators(observablesAtNewPoints, reweighterIO.isMeanKnownToBeZero,
                                                                    reweighterIO.errorMethod, momentsAtNewPoints,
                                                                    momentsEstimatorsAtNewPoints, rewProc.quantitiesConsidered);
            if (parameters.getPrintEstimatorsToFile())
                setObservablesEstimatorsAtNewPointsFromMomentsEstimators(*observablesEstimatorsAtNewPoints, reweighterIO.isMeanKnownToBeZero,
                                                                         momentsEstimatorsAtNewPoints, rewProc.quantitiesConsidered);
            if (rewProc.reweightProbabilityDistributions) {
                std::cout << "==========================================================" << std::endl;
                std::cout << " Set probability distributions from estimators..." << std::endl;
                std::vector<std::vector<Histogram>> histogramsAtNewPoints = momentsReweighter.getProbabilityDistributionsAtNewPoints();
                std::vector<std::vector<HistogramEstimator>> histogramEstimatorsAtNewPoints
                    = momentsReweighter.getProbabilityDistributionEstimatorsAtNewPoints();
                checkSizesOfHistogramsAndHistogramEstimators(
                    histogramsAtNewPoints, histogramEstimatorsAtNewPoints, numberOfNewPoints, numberOfObservablesInFiles);
                setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(
                    probabilityDistributionsAtNewPoints, reweighterIO.errorMethod, histogramsAtNewPoints, histogramEstimatorsAtNewPoints);
                std::cout << "  ...done!" << std::endl;
                std::cout << " Reweighting of probability distributions done!" << std::endl;
                std::cout << "==========================================================" << std::endl;
                std::cout << std::endl;
            }
        }

        // Set manually mean to zero if mean is known to be zero and MEAN is asked
        if (reweighterIO.isMeanKnownToBeZero
            && find(quantitiesToBeReweighted.begin(), quantitiesToBeReweighted.end(), constants::observableName<Mean>)
                   != quantitiesToBeReweighted.end())
            setMeanToZeroAtNewPoints(observablesAtNewPoints);
    }
}

std::vector<std::vector<realFloat>> Reweighter::getValuesOfNewParameters()
{
    return valuesOfNewParameters;
}

std::vector<std::vector<Quantities>> Reweighter::getReweightedObservables()
{
    return observablesAtNewPoints;
}

std::vector<std::vector<std::map<std::string, DataSample>>> Reweighter::getReweightedObservablesEstimators()
{
    if (observablesEstimatorsAtNewPoints == nullptr)
        throw std::logic_error(
            "getReweightedObservablesEstimators method of Reweighter called but ObservablesEstimators not asked to be set!");
    else
        return *observablesEstimatorsAtNewPoints;
}

std::vector<std::vector<ProbabilityDistribution>> Reweighter::getReweightedProbabilityDistributions()
{
    return probabilityDistributionsAtNewPoints;
}

/******************************************** PRIVATE METHODS *************************************************/

std::vector<ReweightingProcedure> Reweighter::getReweightingProceduresToBePerformed()
{
    std::map<std::string, std::vector<int>> binsizesToBeUsedPerQuantityToBeReweighted;
    for (auto quantity : quantitiesToBeReweighted) {
        std::vector<int> binsizesToBeUsedBasedOnObservable
            = getBinsizesToBeUsedBasedOnObservable(quantity, reweighterIO.isMeanKnownToBeZero, reweighterIO.valuesOfSpecifiedBinsizes);
        if (! binsizesToBeUsedBasedOnObservable.empty())  // it is empty for mean if the mean is known to be zero
            binsizesToBeUsedPerQuantityToBeReweighted[quantity] = binsizesToBeUsedBasedOnObservable;
    }
    return getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(
        binsizesToBeUsedPerQuantityToBeReweighted, reweighterIO.isMeanKnownToBeZero);
}

RawDataForReweightingAndMetainformation
Reweighter::getRawDataForReweightingAndMetainformation(std::vector<unsigned int> momentsToBeReweighted, std::vector<int> binsizesToBeUsed,
                                                       bool reweightProbabilityDistributions)
{
    return {reweighterIO.readFromFileDataContainer,
            reweighterIO.namesOfParametersIgnoringMetaParameters,
            reweighterIO.valuesOfSimulationParametersIgnoringMetaParameters,
            reweighterIO.valuesOfSpecifiedLogZ,
            newRangesOfParameters,
            newNumberOfPointsOfParameters,
            useSimulatedPointsAsNewPoints,
            reweighterIO.isMeanKnownToBeZero,
            precisionOfIterativeProcedureToCalculateLogZ,
            reweighterIO.columnsToBeReweightedUsingMultipleColumns,
            reweighterIO.numberOfMultipleColumns,
            reweighterIO.errorMethod,
            reweighterIO.bootstrapNumber,
            maximumMomentNeededOverall,
            momentsToBeReweighted,
            binsizesToBeUsed,
            reweightProbabilityDistributions,
            reweighterIO.binsizeProbabilityDistribution};
}

LqcdReweightingParameters Reweighter::createLqcdParameters(std::initializer_list<std::string> options)
{
    std::vector<std::string> optionsAsStrings(options);
    optionsAsStrings.insert(optionsAsStrings.begin(), "foo");
    std::vector<const char*> argv(optionsAsStrings.size());
    std::transform(optionsAsStrings.begin(), optionsAsStrings.end(), argv.begin(), [](std::string& str) { return str.c_str(); });
    LqcdReweightingParameters parameters((int)argv.size(), argv.data());
    return parameters;
}

/******************************************** STATIC FUNCTIONS *************************************************/

static std::vector<std::string> getQuantitiesToBeReweighted(LqcdReweightingParameters parameters)
{
    std::vector<std::string> quantities;
    if (! parameters.getDeactivateReweightingForMean())
        quantities.push_back(constants::observableName<Mean>);
    if (! parameters.getDeactivateReweightingForVariance())
        quantities.push_back(constants::observableName<Variance>);
    if (! parameters.getDeactivateReweightingForSkewness())
        quantities.push_back(constants::observableName<Skewness>);
    if (! parameters.getDeactivateReweightingForKurtosis())
        quantities.push_back(constants::observableName<Kurtosis>);
    return quantities;
}

static std::vector<int> getBinsizesToBeUsedBasedOnObservable(std::string obsName, bool isMeanZero, std::vector<Binsizes> binsizesFromConfigFile)
{
    std::vector<int> result;
    if (obsName == constants::observableName<Mean> && isMeanZero)
        return result;  // Particular case, return empty object, no reweighting to be done!!
    else {
        for (size_t i = 0; i < binsizesFromConfigFile.size(); i++) {
            std::vector<int> binsizes = binsizesFromConfigFile[i].at(getNeededMomentsBasedOnObservableName(obsName, isMeanZero));
            result.push_back(*std::max_element(binsizes.begin(), binsizes.end()));
        }
        return result;
    }
}

static std::initializer_list<unsigned int> getNeededMomentsBasedOnObservableName(std::string obsName, bool isMeanZero)
{
    if (obsName == constants::observableName<Mean>)
        return isMeanZero ? constants::neededMomentsUnexpanded<Mean> : constants::neededMomentsExpanded<Mean>;
    else if (obsName == constants::observableName<Variance>)
        return isMeanZero ? constants::neededMomentsUnexpanded<Variance> : constants::neededMomentsExpanded<Variance>;
    else if (obsName == constants::observableName<Skewness>)
        return isMeanZero ? constants::neededMomentsUnexpanded<Skewness> : constants::neededMomentsExpanded<Skewness>;
    else if (obsName == constants::observableName<Kurtosis>)
        return isMeanZero ? constants::neededMomentsUnexpanded<Kurtosis> : constants::neededMomentsExpanded<Kurtosis>;
    else
        throw std::invalid_argument("Unknown observable in \"getNeededMomentsBasedOnObservableName\" function!");
}

/*
 * ATTENTION: Here the map binsizesPerQuantity is modified inside the function! It is passed BY VALUE on purpose!!!
 */
static std::vector<ReweightingProcedure>
getReweightingProceduresToBePerformedBasedOnBinsizesPerQuantity(std::map<std::string, std::vector<int>> binsizesPerQuantity, bool isMeanZero)
{
    std::vector<ReweightingProcedure> reweightingProcedures;
    while (! binsizesPerQuantity.empty()) {
        ReweightingProcedure reweightingProcedure;
        reweightingProcedure.binsizesToBeUsed = binsizesPerQuantity.begin()->second;
        reweightingProcedure.momentsToBeReweighted = getNeededMomentsBasedOnObservableName(binsizesPerQuantity.begin()->first, isMeanZero);
        reweightingProcedure.quantitiesConsidered.push_back(binsizesPerQuantity.begin()->first);
        for (std::map<std::string, std::vector<int>>::iterator it = ++(binsizesPerQuantity.begin()); it != binsizesPerQuantity.end(); it++) {
            if (it->second == binsizesPerQuantity.begin()->second) {
                reweightingProcedure.momentsToBeReweighted = getUnionOfVectors(
                    reweightingProcedure.momentsToBeReweighted, getNeededMomentsBasedOnObservableName(it->first, isMeanZero));
                reweightingProcedure.quantitiesConsidered.push_back(it->first);
                it = binsizesPerQuantity.erase(it);
                it--;  // necessary since erase return an iterator to the following element in the map.
            }
        }
        bool isMeanCalculated = false;
        for (std::string quantity : reweightingProcedure.quantitiesConsidered) {
            if (quantity == constants::observableName<Mean>) {
                isMeanCalculated = true;
                break;
            }
        }
        reweightingProcedure.reweightProbabilityDistributions = isMeanCalculated;
        reweightingProcedures.push_back(reweightingProcedure);
        binsizesPerQuantity.erase(binsizesPerQuantity.begin());
    }
    return reweightingProcedures;
}

static std::vector<unsigned int> getUnionOfVectors(std::vector<unsigned int> firstVec, std::vector<unsigned int> secondVec)
{
    std::vector<unsigned int> result(firstVec.size() + secondVec.size());
    std::sort(firstVec.begin(), firstVec.end());
    std::sort(secondVec.begin(), secondVec.end());
    std::vector<unsigned int>::iterator it
        = std::set_union(firstVec.begin(), firstVec.end(), secondVec.begin(), secondVec.end(), result.begin());
    result.resize(it - result.begin());
    return result;
}

static unsigned int getMaximumMomentToBeReweighted(const std::vector<ReweightingProcedure>& rewProcedures)
{
    unsigned int result = 0;
    for (auto rewProc : rewProcedures) {
        unsigned int localMax = *std::max_element(rewProc.momentsToBeReweighted.begin(), rewProc.momentsToBeReweighted.end());
        if (localMax > result)
            result = localMax;
    }
    return result;
}

static void printInformationAboutReweightingProcedure(const ReweightingProcedure& rewProc)
{
    std::string info = "   Reweighting moments ";
    for (auto i : rewProc.momentsToBeReweighted)
        info += std::to_string(i) + " ";
    info += "for ";
    for (auto i : rewProc.quantitiesConsidered)
        info += i + " ";
    info += "  ";
    std::cout << "\n";
    for (size_t i = 0; i < info.length() + 4; i++)
        std::cout << "*";
    std::cout << "\n**";
    for (size_t i = 0; i < info.length(); i++)
        std::cout << " ";
    std::cout << "**\n";
    std::cout << "**" << info << "**\n**";
    for (size_t i = 0; i < info.length(); i++)
        std::cout << " ";
    std::cout << "**\n";
    for (size_t i = 0; i < info.length() + 4; i++)
        std::cout << "*";
    std::cout << "\n\n";
}

static void checkSizesOfMomentsAndMomentsEstimators(std::vector<std::vector<Moments>> moments,
                                                    std::vector<std::vector<MomentsEstimators>> momentsEstimators, size_t numberOfPoints,
                                                    size_t numberOfObservablesInFiles)
{
    if (moments.size() != numberOfPoints)
        throw std::runtime_error("Resulting size of moments after Reweighting is different from the expected one (wrong numberOfPoints)!");
    for (size_t i = 0; i < moments.size(); i++) {
        if (moments[i].size() != numberOfObservablesInFiles)
            throw std::runtime_error(
                "Resulting size of moments after Reweighting is different from the expected one (wrong numberOfObservablesInFiles)!");
    }

    if (momentsEstimators.size() != numberOfPoints)
        throw std::runtime_error(
            "Resulting size of momentsEstimators after Reweighting is different from the expected one (wrong numberOfPoints)!");
    for (size_t i = 0; i < momentsEstimators.size(); i++) {
        if (momentsEstimators[i].size() != numberOfObservablesInFiles)
            throw std::runtime_error("Resulting size of momentsEstimators after Reweighting is different from the expected one (wrong "
                                     "numberOfObservablesInFiles)!");
    }
}

static void setObservablesAtNewPointsFromMomentsAndMomentEstimators(std::vector<std::vector<Quantities>>& observables, bool isMeanZero,
                                                                    ErrorCalculationMethod errorMethod,
                                                                    std::vector<std::vector<Moments>> moments,
                                                                    std::vector<std::vector<MomentsEstimators>> momentsEstimators,
                                                                    std::vector<std::string> quantitiesToBeSet)
{
    for (size_t newPoint = 0; newPoint < observables.size(); newPoint++) {
        for (size_t obsInFile = 0; obsInFile < observables[newPoint].size(); obsInFile++) {
            for (auto quantity : quantitiesToBeSet) {
                if (quantity == constants::observableName<Mean>) {
                    observables[newPoint][obsInFile][quantity] = Mean(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile],
                                                                      QuantityAttributes{isMeanZero, false}, errorMethod);
                } else if (quantity == constants::observableName<Variance>) {
                    observables[newPoint][obsInFile][quantity] = Variance(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile],
                                                                          QuantityAttributes{isMeanZero, false}, errorMethod);
                } else if (quantity == constants::observableName<Skewness>) {
                    observables[newPoint][obsInFile][quantity] = Skewness(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile],
                                                                          QuantityAttributes{isMeanZero, false}, errorMethod);
                } else if (quantity == constants::observableName<Kurtosis>) {
                    observables[newPoint][obsInFile][quantity] = Kurtosis(moments[newPoint][obsInFile], momentsEstimators[newPoint][obsInFile],
                                                                          QuantityAttributes{isMeanZero, false}, errorMethod);
                } else
                    throw std::invalid_argument(
                        "Unknown observable in \"setObservablesAtNewPointsFromMomentsAndMomentEstimators\" function!");
            }
        }
    }
}

static void checkSizesOfHistogramsAndHistogramEstimators(std::vector<std::vector<Histogram>> histogram,
                                                         std::vector<std::vector<HistogramEstimator>> histogramEstimator,
                                                         size_t numberOfPoints, size_t numberOfObservablesInFiles)
{
    if (histogram.size() != numberOfPoints)
        throw std::runtime_error(
            "Resulting size of histograms after Reweighting is different from the expected one (wrong numberOfPoints)!");
    for (size_t i = 0; i < histogram.size(); i++) {
        if (histogram[i].size() != numberOfObservablesInFiles)
            throw std::runtime_error(
                "Resulting size of histograms after Reweighting is different from the expected one (wrong numberOfInputObservables)!");
    }

    if (histogramEstimator.size() != numberOfPoints)
        throw std::runtime_error(
            "Resulting size of histogramEstimator after Reweighting is different from the expected one (wrong numberOfPoints)!");
    for (size_t i = 0; i < histogramEstimator.size(); i++) {
        if (histogramEstimator[i].size() != numberOfObservablesInFiles)
            throw std::runtime_error("Resulting size of histogramEstimator after Reweighting is different from the expected one (wrong "
                                     "numberOfInputObservables)!");
    }
}

static void setProbabilityDistributionAtNewPointsFromHistogramAndHistogramEstimators(
    std::vector<std::vector<ProbabilityDistribution>>& probabilityDistributions, ErrorCalculationMethod errorMethod,
    std::vector<std::vector<Histogram>> histograms, std::vector<std::vector<HistogramEstimator>> histogramEstimators)
{
    for (size_t indexNewPoint = 0; indexNewPoint < histograms.size(); indexNewPoint++) {
        for (size_t indexInputObservable = 0; indexInputObservable < histograms[indexNewPoint].size(); indexInputObservable++) {
            ProbabilityDistribution tmpProbabilityDistribution(
                histograms[indexNewPoint][indexInputObservable], histogramEstimators[indexNewPoint][indexInputObservable], errorMethod);
            probabilityDistributions[indexNewPoint][indexInputObservable] = tmpProbabilityDistribution;
        }
    }
}

static void setMeanToZeroAtNewPoints(std::vector<std::vector<Quantities>>& observables)
{
    for (size_t newPoint = 0; newPoint < observables.size(); newPoint++) {
        for (size_t obsInFile = 0; obsInFile < observables[newPoint].size(); obsInFile++) {
            EstimateAndError& meanValue = observables[newPoint][obsInFile][constants::observableName<Mean>].value;
            if (meanValue.estimate != meanValue.estimate)
                meanValue.estimate = 0.0;
            else
                throw std::logic_error("Error setting mean.estimate to 0.0 since it should be NAN but it isn't!");
            if (meanValue.error != meanValue.error)
                meanValue.error = 0.0;
            else
                throw std::logic_error("Error setting mean.error to 0.0 since it should be NAN but it isn't!");
        }
    }
}

static void
setObservablesEstimatorsAtNewPointsFromMomentsEstimators(std::vector<std::vector<std::map<std::string, DataSample>>>& observablesEstimators,
                                                         bool isMeanZero, std::vector<std::vector<MomentsEstimators>> momentsEstimators,
                                                         std::vector<std::string> quantitiesToBeSet)
{
    for (size_t newPoint = 0; newPoint < observablesEstimators.size(); newPoint++) {
        for (size_t obsInFile = 0; obsInFile < observablesEstimators[newPoint].size(); obsInFile++) {
            for (auto quantity : quantitiesToBeSet) {
                if (quantity == constants::observableName<Mean>)
                    observablesEstimators[newPoint][obsInFile][quantity]
                        = Mean::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero);
                else if (quantity == constants::observableName<Variance>)
                    observablesEstimators[newPoint][obsInFile][quantity]
                        = Variance::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero);
                else if (quantity == constants::observableName<Skewness>)
                    observablesEstimators[newPoint][obsInFile][quantity]
                        = Skewness::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero);
                else if (quantity == constants::observableName<Kurtosis>)
                    observablesEstimators[newPoint][obsInFile][quantity]
                        = Kurtosis::evaluateObservableOnMomentEstimators(momentsEstimators[newPoint][obsInFile], isMeanZero);
                else
                    throw std::invalid_argument(
                        "Unknown observable in \"setObservablesAtNewPointsFromMomentsAndMomentEstimators\" function!");
            }
        }
    }
}
