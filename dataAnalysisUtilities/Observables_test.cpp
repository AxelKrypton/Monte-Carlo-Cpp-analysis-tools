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


//TODO: Develop tests for Mean class
