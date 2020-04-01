/*
 *
 *  Copyright (c) 2020 Alessandro Sciarra
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

#include "Moments.hpp"

Moments::Moments() {}

void Moments::insert(const unsigned int& whichMoment, const realFloat& momentValue)
{
    moments.insert(std::pair<unsigned int, realFloat>(whichMoment, momentValue));
}

realFloat Moments::operator[](const unsigned int& whichMoment)
{
    std::multimap<unsigned int, realFloat>::iterator itWhichMoment = moments.find(whichMoment);
    if (itWhichMoment == moments.end())
        throw std::out_of_range("Moments::[] accessed an invalid moment! Aborting...");
    else if (moments.count(itWhichMoment->first) > 1)
        throw std::invalid_argument("Moments::[] accessed a moment for which several values are set, NOT ALLOWED! Aborting...");
    else
        return itWhichMoment->second;
}

std::vector<realFloat> Moments::operator()(const unsigned int& whichMoment)
{
    std::multimap<unsigned int, realFloat>::iterator itWhichMoment = moments.find(whichMoment);
    if (itWhichMoment == moments.end())
        throw std::out_of_range("Moments::() accessed an invalid moment! Aborting...");
    else if (moments.count(itWhichMoment->first) == 1)
        throw std::invalid_argument("Moments::() accessed a moment for which only one value is set, NOT ALLOWED! Aborting...");
    else {
        std::vector<realFloat> returnVec;
        std::multimap<unsigned int, realFloat>::iterator itRangeWhichMoment;
        for (itRangeWhichMoment = moments.equal_range(itWhichMoment->first).first;
             itRangeWhichMoment != moments.equal_range(itWhichMoment->first).second; ++itRangeWhichMoment)
            returnVec.push_back(itRangeWhichMoment->second);
        return returnVec;
    }
}

/**********************************************************************************/

MomentsEstimators::MomentsEstimators(){};

void MomentsEstimators::insert(const unsigned int& whichMoment, const DataSample& momentEstValues)
{
    momentsEstimators.insert(std::pair<unsigned int, DataSample>(whichMoment, momentEstValues));
}

DataSample MomentsEstimators::operator[](const unsigned int& whichMoment)
{
    std::multimap<unsigned int, DataSample>::iterator itWhichMoment = momentsEstimators.find(whichMoment);
    if (itWhichMoment == momentsEstimators.end())
        throw std::out_of_range("MomentsEstimators::[] accessed an invalid moment! Aborting...");
    else if (momentsEstimators.count(itWhichMoment->first) > 1)
        throw std::invalid_argument("MomentsEstimators::[] accessed a moment for which several values are set, NOT ALLOWED! Aborting...");
    else
        return itWhichMoment->second;
}

std::vector<DataSample> MomentsEstimators::operator[](const std::initializer_list<unsigned int>& whichMoments)
{
    std::vector<DataSample> selectedMoments;
    for (unsigned int i : whichMoments) {
        DataSample tmp = (*this)[i];
        selectedMoments.push_back(tmp);
    }
    return selectedMoments;
}

std::vector<DataSample> MomentsEstimators::operator()(const unsigned int& whichMoment)
{
    std::multimap<unsigned int, DataSample>::iterator itWhichMoment = momentsEstimators.find(whichMoment);
    if (itWhichMoment == momentsEstimators.end())
        throw std::out_of_range("MomentsEstimators::() accessed an invalid moment! Aborting...");
    else if (momentsEstimators.count(itWhichMoment->first) == 1)
        throw std::invalid_argument("MomentsEstimators::() accessed a moment for which only one value is set, NOT ALLOWED! Aborting...");
    else {
        std::vector<DataSample> returnVec;
        std::multimap<unsigned int, DataSample>::iterator itRangeWhichMoment;
        for (itRangeWhichMoment = momentsEstimators.equal_range(itWhichMoment->first).first;
             itRangeWhichMoment != momentsEstimators.equal_range(itWhichMoment->first).second; ++itRangeWhichMoment)
            returnVec.push_back(itRangeWhichMoment->second);
        return returnVec;
    }
}
