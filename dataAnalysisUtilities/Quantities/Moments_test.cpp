/*
 *
 *  Copyright (c) 2016,2018,2020 Alessandro Sciarra
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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Moments
#include "Moments.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(moments)

    BOOST_AUTO_TEST_CASE(build) { BOOST_REQUIRE_NO_THROW(Moments moments); }

    BOOST_AUTO_TEST_CASE(setter_getter1)
    {
        unsigned int referenceIndex = 12;
        realFloat referenceMoment = 3.14;
        Moments moment;
        moment.insert(referenceIndex, referenceMoment);
        BOOST_REQUIRE_THROW(moment[3], std::out_of_range);
        BOOST_REQUIRE_THROW(moment(referenceIndex), std::invalid_argument);
        BOOST_REQUIRE_EQUAL(referenceMoment, moment[referenceIndex]);
    }

    BOOST_AUTO_TEST_CASE(setter_getter2)
    {
        unsigned int referenceIndex = 3;
        std::vector<realFloat> referenceMoment{3.14, 6.28, 9.42};
        Moments moment;
        for (size_t i = 0; i < referenceMoment.size(); i++)
            moment.insert(referenceIndex, referenceMoment[i]);
        BOOST_REQUIRE_THROW(moment(4), std::out_of_range);
        BOOST_REQUIRE_THROW(moment[3], std::invalid_argument);
        BOOST_REQUIRE(moment(3) == referenceMoment);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(momentsEstimators)

    BOOST_AUTO_TEST_CASE(build) { BOOST_REQUIRE_NO_THROW(MomentsEstimators momentsEst); }

    BOOST_AUTO_TEST_CASE(setter_getter1)
    {
        unsigned int referenceIndex = 12;
        DataSample referenceMomentEst(std::valarray<realFloat>(3.14, 100));
        MomentsEstimators momentsEst;
        momentsEst.insert(referenceIndex, referenceMomentEst);
        BOOST_REQUIRE_THROW(momentsEst[3], std::out_of_range);
        BOOST_REQUIRE_THROW(momentsEst(referenceIndex), std::invalid_argument);
        for (int i = 0; i < referenceMomentEst.getNumberOfElements(); i++)
            BOOST_REQUIRE_EQUAL(referenceMomentEst[i], momentsEst[referenceIndex][i]);
    }

    BOOST_AUTO_TEST_CASE(setter_getter2)
    {
        std::vector<int> referenceIndex{12, 7, 2};
        std::vector<DataSample> referenceMomentEst(3, DataSample(std::valarray<realFloat>(6.28, 100)));
        MomentsEstimators momentsEst;
        for (size_t i = 0; i < referenceIndex.size(); i++)
            momentsEst.insert(referenceIndex[i], referenceMomentEst[i]);
        std::vector<DataSample> momentsEstSelected = momentsEst[{12, 2}];
        std::initializer_list<unsigned int> tmp = {12, 7, 2, 1};
        BOOST_REQUIRE_THROW(std::vector<DataSample> momentsEstSelectedWrong = momentsEst[tmp], std::out_of_range);
        for (int i = 0; i < referenceMomentEst[0].getNumberOfElements(); i++) {
            BOOST_REQUIRE_EQUAL(referenceMomentEst[0][i], momentsEstSelected[0][i]);
            BOOST_REQUIRE_EQUAL(referenceMomentEst[2][i], momentsEstSelected[1][i]);
        }
    }

    BOOST_AUTO_TEST_CASE(setter_getter3)
    {
        unsigned int referenceIndex = 3;
        std::vector<DataSample> referenceMomentEst{
            DataSample(std::valarray<realFloat>(3.14, 100)), DataSample(std::valarray<realFloat>(6.28, 100))};
        MomentsEstimators momentsEst;
        for (size_t i = 0; i < referenceMomentEst.size(); i++)
            momentsEst.insert(referenceIndex, referenceMomentEst[i]);
        BOOST_REQUIRE_THROW(momentsEst(4), std::out_of_range);
        BOOST_REQUIRE_THROW(momentsEst[3], std::invalid_argument);
        for (size_t i = 0; i < referenceMomentEst.size(); i++) {
            for (int j = 0; j < referenceMomentEst[i].getNumberOfElements(); j++)
                BOOST_REQUIRE_EQUAL(momentsEst(referenceIndex)[i][j], referenceMomentEst[i][j]);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
