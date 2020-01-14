/*
 *
 *  Copyright (c)
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

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "../types.hpp"

#include "iostream"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
namespace po = boost::program_options;

class Parameters
{
public:
	Parameters(int argc, const char ** argv);

	struct parse_aborted {};
	std::string file;
	std::string analysisOutputFilePrefix;
	std::string analysisOutputFilePostfix;
	int binsize;
	std::vector<int> binsizeMoments;
	std::vector<int> binsizeCentralMoments;
	int numberOfBins;
	std::vector<int> numberOfBinsMoments;
	std::vector<int> numberOfBinsCentralMoments;
	int numberOfBinsForAutocorrelation;
	int timeMaxAutocorrelationFunction;
	int offset;
	int column;
	bool isMeanKnownToBeZero;
	bool doNotUseBinning;
	bool useNumberOfBinsForBinning;
	bool calcAutocorrelation;
	bool doNotAnalyzeMean;
	bool doNotAnalyzeVariance;
	bool doNotAnalyzeSkewness;
	bool doNotAnalyzeKurtosis;
	bool binningMustFitDataSampleSize;
	bool adjustDataSampleSizeToBinning;

private:
	void printParameters();
	void setBinningTypeParameter(bool valueIn);
	void checkParsedArguments(po::variables_map & vm, po::options_description & desc);
	void parseBinningInformationForMoments(std::vector<int>& vectorWithBinningInformations, const int defaultValue);
};

#endif /* PARAMETERS_HPP_ */
