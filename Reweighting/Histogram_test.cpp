#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Histogram
#include <boost/test/unit_test.hpp>

#include "Histogram.hpp"

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        BOOST_REQUIRE_THROW(Histogram hist(), std::invalid_argument);
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        double wrongbinsize=0;
        BOOST_REQUIRE_THROW(Histogram hist(wrongbinsize), std::logic_error);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getters)

    BOOST_AUTO_TEST_CASE(getters1)
    {
        double binsize=2;
        bool dontIncludeZeroBins=false;
        bool includeZeroBins=true;
        Histogram hist(binsize);
        hist[0.3]=1;
        hist[1.3]=1;
        hist[6.3]=1;
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(dontIncludeZeroBins), 3);
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(includeZeroBins), 4);
    }

    BOOST_AUTO_TEST_CASE(getters2)
    {
        double binsize=2;
        Histogram hist(binsize);
        BOOST_REQUIRE_EQUAL(hist.getBinsize, 2);
    }

    BOOST_AUTO_TEST_CASE(getters3)
    {
        double binsize=2;
        std::vector<double> manualHeights, heights;
        Histogram hist(binsize);
        hist[0.3]=1;
        hist[1.3]=3;
        hist[6.3]=4;
        heights=hist.getHeightsOfBins;
        manualHeights.push_back(1);
        manualHeights.push_back(3);
        manualHeights.push_back(4);
        BOOST_REQUIRE((heights.size() == manualHeights.size()) && std::equal(heights.begin(), heights.end(), manualHeights.begin()));
    }

    BOOST_AUTO_TEST_CASE(getters4)
    {
        double binsize=2;
        double upperedge;
        Histogram hist(binsize);
        hist[0.3]=1;
        upperedge=1;
        BOOST_REQUIRE_EQUAL(hist.getMaxXvalue, upperedge);
    }

    BOOST_AUTO_TEST_CASE(getters5)
    {
        double binsize=2;
        double loweredge;
        Histogram hist(binsize);
        hist[0.3]=1;
        loweredge=-1;
        BOOST_REQUIRE_EQUAL(hist.getMinXvalue, loweredge);
    }

    BOOST_AUTO_TEST_CASE(getters6)
    {
        double binsize=2;
        std::vector<std::pair<double, double> > bins, manualBins;
        Histogram hist(binsize);
        hist[0.3]=1;
        hist[1.3]=3;
        hist[6.3]=4;
        bins=hist.getBins;
        manualBins.push_back(std::pair<double,double> (-1,1));
        manualBins.push_back(std::pair<double,double> (1,3));
        manualBins.push_back(std::pair<double,double> (5,7));
        BOOST_REQUIRE((bins.size() == manualBins.size()) && std::equal(bins.begin(), bins.end(), manualBins.begin()));
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AccessOperator)

    BOOST_AUTO_TEST_CASE(AccessOperator1)
    {
        double binsize=2;
        Histogram hist(2);
        double arbitraryDouble=4.3;
        BOOST_REQUIRE_NO_THROW(double doubleNumber=hist[arbitraryDouble]);
    }

BOOST_AUTO_TEST_SUITE_END()