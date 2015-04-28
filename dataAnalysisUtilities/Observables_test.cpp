#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Observables
#include <boost/test/unit_test.hpp>

#include "Observables.hpp"

BOOST_AUTO_TEST_SUITE(moments)

	BOOST_AUTO_TEST_CASE(build)
    {
        BOOST_REQUIRE_NO_THROW(Moments moments);
    }

	BOOST_AUTO_TEST_CASE(setter_getter1)
	{
		int referenceIndex=12;
		double referenceMoment = 3.14;
		Moments moment;
		moment[referenceIndex] = referenceMoment;
		BOOST_REQUIRE_EQUAL(referenceMoment, moment[referenceIndex]);
	}

	BOOST_AUTO_TEST_CASE(setter_getter2)
	{
		std::vector<int> referenceIndex{12, 7, 2};
		std::vector<double> referenceMoment{3.14, 6.28, 9.42};
		Moments moment;
		for(size_t i=0; i<referenceIndex.size(); i++){
			moment[referenceIndex[i]] = referenceMoment[i];
		}
		Moments moment2 = moment[ {12, 2} ];
		std::initializer_list<int> tmp = {12, 7, 2, 1};
		BOOST_REQUIRE_THROW(Moments moment3 = moment[tmp], std::out_of_range);
		BOOST_REQUIRE_EQUAL(referenceMoment[0], moment2[referenceIndex[0]]);
		BOOST_REQUIRE_EQUAL(referenceMoment[2], moment2[referenceIndex[2]]);
	}

	BOOST_AUTO_TEST_CASE(at)
	{
		Moments moment;
		moment[0] = 123.456;
		BOOST_REQUIRE_THROW(moment.at(3), std::out_of_range);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(momentsEstimators)

	BOOST_AUTO_TEST_CASE(build)
    {
        BOOST_REQUIRE_NO_THROW(MomentsEstimators momentsEst);
    }

	BOOST_AUTO_TEST_CASE(setter_getter1)
	{
		int referenceIndex=12;
		DataSample referenceMomentEst(std::valarray<double>(3.14, 100));
		MomentsEstimators momentsEst;
		momentsEst[referenceIndex] = referenceMomentEst;
		for(int i=0; i<referenceMomentEst.getNumberOfElements(); i++)
			BOOST_REQUIRE_EQUAL(referenceMomentEst[i], momentsEst[referenceIndex][i]);
	}

	BOOST_AUTO_TEST_CASE(setter_getter2)
	{
		std::vector<int> referenceIndex{12, 7, 2};
		std::vector<DataSample> referenceMomentEst(3, DataSample(std::valarray<double>(6.28, 100)));
		MomentsEstimators momentsEst;
		for(size_t i=0; i<referenceIndex.size(); i++){
			momentsEst[referenceIndex[i]] = referenceMomentEst[i];
		}
		MomentsEstimators momentsEst2 = momentsEst[ {12, 2} ];
		std::initializer_list<int> tmp = {12, 7, 2, 1};
		BOOST_REQUIRE_THROW(MomentsEstimators moment3 = momentsEst[tmp], std::out_of_range);
		for(int i=0; i<referenceMomentEst[0].getNumberOfElements(); i++){
			BOOST_REQUIRE_EQUAL(referenceMomentEst[0][i], momentsEst2[referenceIndex[0]][i]);
			BOOST_REQUIRE_EQUAL(referenceMomentEst[2][i], momentsEst2[referenceIndex[2]][i]);
		}
	}

	BOOST_AUTO_TEST_CASE(setter_getter3)
	{
		std::vector<int> referenceIndex{12, 7, 2};
		std::vector<DataSample> referenceMomentEst(3, DataSample(std::valarray<double>(6.28, 100)));
		MomentsEstimators momentsEst;
		for(size_t i=0; i<referenceIndex.size(); i++){
			momentsEst[referenceIndex[i]] = referenceMomentEst[i];
		}
		std::vector<DataSample> momentsEstSelected = momentsEst( {12, 2} );
		std::initializer_list<int> tmp = {12, 7, 2, 1};
		BOOST_REQUIRE_THROW(MomentsEstimators moment3 = momentsEst[tmp], std::out_of_range);
		for(int i=0; i<referenceMomentEst[0].getNumberOfElements(); i++){
			BOOST_REQUIRE_EQUAL(referenceMomentEst[0][i], momentsEstSelected[0][i]);
			BOOST_REQUIRE_EQUAL(referenceMomentEst[2][i], momentsEstSelected[1][i]);
		}
	}

	BOOST_AUTO_TEST_CASE(at)
	{
		MomentsEstimators momentsEst;
		momentsEst[0] = DataSample(std::valarray<double>(9.42, 100));
		BOOST_REQUIRE_THROW(momentsEst.at(3), std::out_of_range);
	}

BOOST_AUTO_TEST_SUITE_END()


//TODO: Develop tests for Mean, Variance, Skewness, BinderCumulant classes
