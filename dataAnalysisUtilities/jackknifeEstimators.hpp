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

/**
 * Jackknife estimators.
 * Following BA Berg,
 * "Markov Chain Monte Carlo Simulations and Their Statistical Analysis",
 * equation (2.160).
 */

#ifndef JACKKNIFEESTIMATORS_HPP_
#define JACKKNIFEESTIMATORS_HPP_

#include "DataSample.hpp"

class JackknifeEstimators: public DataSample
{
public:
	JackknifeEstimators(int numberOfElementsIn);
	JackknifeEstimators(DataSample sampleIn);

protected:
	void checkIfJackknifeCanBePerformed(int n);
	DataSampleBasic createJackknifeEstimatorsWithBinning(int numberOfBins, int binsize);
};


//TODO: The following two classes are not used at all in the Project. Remove them!?
class JackknifeEstimatorsFromBinningWithNumberOfBins: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithNumberOfBins(DataSample sampleIn, int numberOfBins);
};

class JackknifeEstimatorsFromBinningWithBinsize: public JackknifeEstimators
{
public:
	JackknifeEstimatorsFromBinningWithBinsize(DataSample sampleIn, int numberOfBins);
private:
	void checkIfJackknifeCanBePerformedWithBinsize(int binsize);
};



#endif /* JACKKNIFEESTIMATORS_HPP_ */
