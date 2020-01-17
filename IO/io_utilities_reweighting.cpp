/*
 *
 *  Copyright (c) 2015-2016,2018-2020 Alessandro Sciarra
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

#include "io_utilities_reweighting.hpp"

#include <boost/lexical_cast.hpp>
#include <fstream>

class LqcdReweightedData {
  public:
    LqcdReweightedData(std::string quantityNameIn, std::string outputfilePrefix) : quantityName(quantityNameIn)
    {
        filename = outputfilePrefix + "_" + quantityName;
    }

    void append(realFloat betaValue, Observables observables)
    {
        std::pair<realFloat, Observables> tmpPair(betaValue, observables);
        values.push_back(tmpPair);
    }

    void printToFile(const unsigned int* estimatorNumber = nullptr)
    {
        if (estimatorNumber == nullptr) {
            std::cout << "# Writing reweighted data for \"" << quantityName << "\" to file \"" << filename << "\"" << std::endl;
            std::ofstream outputstream;
            outputstream.open(filename.c_str(), std::ios::app);
            if (outputstream.is_open()) {
                outputstream << "# beta\t\t" << values[0].second.getMetaInformation() << std::endl;

                for (unsigned int index = 0; index < values.size(); index++) {
                    outputstream << std::scientific << values[index].first << "\t" << values[index].second.getObservablesAsString()
                                 << std::endl;
                }
                outputstream.close();
            } else {
                throw std::invalid_argument("Could open file for output. Aborting!");
            }
        } else {
            static bool alreadyPrintedToFile = false;
            if (alreadyPrintedToFile == false)
                std::cout << "# Writing observable estimators for \"" << quantityName << "\" to file \"" << filename << "\"" << std::endl;
            std::ofstream outputstream;
            outputstream.open(filename.c_str(), std::ios::app);
            if (outputstream.is_open()) {
                if (alreadyPrintedToFile == false)
                    outputstream << "#estimator\tbeta\t\t" << values[0].second.getMetaInformation() << std::endl;
                for (unsigned int index = 0; index < values.size(); index++)
                    outputstream << *estimatorNumber << "\t\t" << std::scientific << values[index].first << "\t"
                                 << values[index].second.getObservablesAsString() << std::endl;
                outputstream.close();
            } else {
                throw std::invalid_argument("Could open file for output. Aborting!");
            }
            if (alreadyPrintedToFile == false)
                alreadyPrintedToFile = true;
            values.clear();
        }
    }

  private:
    std::string quantityName;
    std::string filename;
    std::vector<std::pair<realFloat, Observables>> values;
};

template<typename T>
static void checkInputSizes(const std::vector<std::vector<realFloat>>& newBetaValues, const std::vector<std::vector<T>>& reweightedData)
{
    if (reweightedData.size() == 0) {
        throw std::exception();
    }

    unsigned int numberOfNewPoints = reweightedData.size();
    unsigned int numberOfQuantities = reweightedData[0].size();

    if (numberOfQuantities <= 0) {
        throw std::invalid_argument("Number of reweighted quantities must be greater than 0! Aborting!");
    }
    if (newBetaValues.size() != numberOfNewPoints) {
        throw std::invalid_argument("Number of new beta points and reweighted observables does not match! Aborting!");
    }
    if (newBetaValues[0].size() != 1) {
        throw std::invalid_argument("Found new values for more than one parameter! Aborting!");
    }
    std::cout << "# Found " << numberOfQuantities << " reweighted quantities." << std::endl;
}

void writeLqcdReweightedObservablesToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                          const std::vector<std::vector<Observables>>& reweightedData, std::string outputfilePrefix)
{
    try {
        checkInputSizes(newBetaValues, reweightedData);
    } catch (std::invalid_argument& e) {
        throw e;
    } catch (std::exception& e) {
        std::cout << "\n   No reweighting procedure has been performed, probably because none was asked. No file will be created.\n"
                  << std::endl;
        return;
    }

    std::vector<LqcdReweightedData> reweightedQuantities;
    unsigned int numberOfNewPoints = reweightedData.size();
    unsigned int numberOfQuantities = reweightedData[0].size();

    for (unsigned int quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++) {
        LqcdReweightedData reweightedQuantity("quantity" + boost::lexical_cast<std::string>(quantityIndex + 1), outputfilePrefix);

        for (unsigned int iteration = 0; iteration < numberOfNewPoints; iteration++) {
            reweightedQuantity.append(newBetaValues[iteration][0], reweightedData[iteration][quantityIndex]);
        }

        reweightedQuantities.push_back(reweightedQuantity);
    }

    for (unsigned int quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++) {
        reweightedQuantities[quantityIndex].printToFile();
    }
}

static std::vector<Observables> convertMapOfObservableNameAndDataSampleToVectorOfObservables(std::map<std::string, DataSample> inputMap)
{
    if (inputMap.empty())
        throw std::logic_error("Called \"convertMapOfObservableNameAndDataSampleToVectorOfObservables\" function with empty map!");

    const int numberOfEstimatorsPerQuantity = inputMap.begin()->second.getNumberOfElements();
    std::vector<Observables> returnValue(numberOfEstimatorsPerQuantity, Observables());

    for (auto& mapElement : inputMap) {
        if (mapElement.second.getNumberOfElements() != numberOfEstimatorsPerQuantity)
            throw std::logic_error("In \"convertMapOfObservableNameAndDataSampleToVectorOfObservables\" map seems to contain different "
                                   "DataSample sizes, not allowed!");
        for (int dataIndex = 0; dataIndex < mapElement.second.getNumberOfElements(); dataIndex++) {
            if (mapElement.first == Mean::observableName)
                returnValue[dataIndex].mean.estimate = mapElement.second[dataIndex];
            else if (mapElement.first == Variance::observableName)
                returnValue[dataIndex].susceptibility.estimate = mapElement.second[dataIndex];
            else if (mapElement.first == Skewness::observableName)
                returnValue[dataIndex].skewness.estimate = mapElement.second[dataIndex];
            else if (mapElement.first == Kurtosis::observableName)
                returnValue[dataIndex].kurtosis.estimate = mapElement.second[dataIndex];
            else
                throw std::invalid_argument(
                    "In \"convertMapOfObservableNameAndDataSampleToVectorOfObservables\" map seems to contain unknown observable!");
        }
    }

    return returnValue;
}

void writeLqcdReweightedObservablesEstimatorsToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                                    const std::vector<std::vector<std::map<std::string, DataSample>>>& reweightedEstimators,
                                                    std::string outputfilePrefix)
{
    try {
        std::cout << std::endl;
        checkInputSizes(newBetaValues, reweightedEstimators);
    } catch (std::invalid_argument& e) {
        throw e;
    } catch (std::exception& e) {
        std::cout << "\n   No reweighting procedure has been performed, probably because none was asked. No file will be created.\n"
                  << std::endl;
        return;
    }

    unsigned int numberOfNewPoints = reweightedEstimators.size();
    unsigned int numberOfQuantities = reweightedEstimators[0].size();
    std::vector<std::vector<std::vector<Observables>>> reweightedEstimatorsNew(
        numberOfNewPoints, std::vector<std::vector<Observables>>(numberOfQuantities, std::vector<Observables>()));

    for (unsigned int newPointIndex = 0; newPointIndex < numberOfNewPoints; newPointIndex++) {
        for (unsigned int quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++)
            reweightedEstimatorsNew[newPointIndex][quantityIndex]
                = convertMapOfObservableNameAndDataSampleToVectorOfObservables(reweightedEstimators[newPointIndex][quantityIndex]);
    }

    unsigned int numberOfEstimators = reweightedEstimatorsNew[0][0].size();

    for (unsigned int quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++) {
        LqcdReweightedData reweightedQuantity("estimatorsQuantity" + std::to_string(quantityIndex + 1), outputfilePrefix);
        for (unsigned int estimatorIndex = 0; estimatorIndex < numberOfEstimators; estimatorIndex++) {
            for (unsigned int newPointIndex = 0; newPointIndex < numberOfNewPoints; newPointIndex++) {
                reweightedQuantity.append(
                    newBetaValues[newPointIndex][0], reweightedEstimatorsNew[newPointIndex][quantityIndex][estimatorIndex]);
            }
            reweightedQuantity.printToFile(&estimatorIndex);
        }
    }
}

static void printHistoToFile(realFloat betaValue, int numberOfQuantity, const ProbabilityDistribution& reweightedProbabilityDistribution,
                             std::string outputfilePrefix)
{
    std::vector<double> middleOfBins = reweightedProbabilityDistribution.getMiddleOfBins();
    std::ofstream outputstream;
    std::string filename = outputfilePrefix + "_" + "ProbabilityDistribution" + boost::lexical_cast<std::string>(numberOfQuantity + 1) + "_"
                           + std::to_string(betaValue);
    outputstream.open(filename.c_str(), std::ios::app);
    if (outputstream.is_open()) {
        double binsize = reweightedProbabilityDistribution.getBinsize();
        outputstream << "#numOfBin"
                     << "  "
                     << "LowerEdgeOfBin   MiddleOfBin\tUpperEdgeOfBin"
                     << "\t\t"
                     << "Estimate\tError" << std::endl;
        int i = 0;
        for (double middleOfBin : middleOfBins) {
            realFloat lowerBinEdge = middleOfBin - 0.5 * binsize;
            realFloat upperBinEdge = middleOfBin + 0.5 * binsize;
            EstimateAndError estErr = reweightedProbabilityDistribution.getHeightOfSpecificBin(middleOfBin);
            outputstream << std::scientific << i << "\t   " << lowerBinEdge << "\t   " << middleOfBin << "\t " << upperBinEdge << "\t\t"
                         << estErr.estimate << "\t" << estErr.error << std::endl;
            i++;
        }
        outputstream.close();
    }
}

void writeLqcdReweightedProbabilityDistributionsToFile(const std::vector<std::vector<realFloat>>& newBetaValues,
                                                       const std::vector<std::vector<ProbabilityDistribution>>& reweightedProbabilityDistributions,
                                                       std::string outputfilePrefix)
{
    try {
        checkInputSizes(newBetaValues, reweightedProbabilityDistributions);
    } catch (std::invalid_argument& e) {
        throw e;
    } catch (std::exception& e) {
        std::cout << "\n   No reweighting procedure has been performed, probably because none was asked. No file will be created.\n"
                  << std::endl;
        return;
    }

    unsigned int numberOfNewPoints = reweightedProbabilityDistributions.size();
    unsigned int numberOfQuantities = reweightedProbabilityDistributions[0].size();

    for (unsigned int quantityIndex = 0; quantityIndex < numberOfQuantities; quantityIndex++) {
        for (unsigned int iteration = 0; iteration < numberOfNewPoints; iteration++) {
            printHistoToFile(
                newBetaValues[iteration][0], quantityIndex, reweightedProbabilityDistributions[iteration][quantityIndex], outputfilePrefix);
        }
    }
}
