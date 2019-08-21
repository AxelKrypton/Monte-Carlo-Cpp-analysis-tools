#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Histogram
#include <boost/test/unit_test.hpp>

#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "Histogram.hpp"

BOOST_AUTO_TEST_SUITE(build)

    BOOST_AUTO_TEST_CASE(build1)
    {
        BOOST_REQUIRE_NO_THROW(Histogram hist{1.5});
    }

    BOOST_AUTO_TEST_CASE(build2)
    {
        double wrongbinsize=0.0;
        BOOST_REQUIRE_THROW(Histogram hist(wrongbinsize), std::logic_error);
        wrongbinsize=-2.6;
        BOOST_REQUIRE_THROW(Histogram hist(wrongbinsize), std::logic_error);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(getters)

    BOOST_AUTO_TEST_CASE(getters1)
    {
        const double binsize=2.0;
        bool dontIncludeZeroBins=false;
        bool includeZeroBins=true;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=1.0;
        hist[6.3]=1.0;
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(dontIncludeZeroBins), 3);
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(includeZeroBins), 4);
    }

    BOOST_AUTO_TEST_CASE(getters2)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        BOOST_REQUIRE_CLOSE(hist.getBinsize(), 2.0, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getters3)
    {
        const double binsize=2.0;
        std::vector<double> manualHeights, heights;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        heights=hist.getHeightsOfBins();
        manualHeights.push_back(1.0);
        manualHeights.push_back(3.0);
        manualHeights.push_back(4.0);
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++)
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getters4)
    {
        const double binsize=2.0;
        double upperedge;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        upperedge=1.0;
        BOOST_REQUIRE_CLOSE(hist.getMaxXvalue(), upperedge, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getters5)
    {
        const double binsize=2.0;
        double loweredge;
        Histogram hist(binsize);
        hist[0.3]=1;
        loweredge=-1;
        BOOST_REQUIRE_EQUAL(hist.getMinXvalue(), loweredge);
    }

    BOOST_AUTO_TEST_CASE(getters6)
    {
        const double binsize=2.0;
        std::vector<std::pair<double, double> > bins, manualBins;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        bins=hist.getBins();
        manualBins.push_back(std::pair<double,double> (-1.0,1.0));
        manualBins.push_back(std::pair<double,double> ( 1.0,3.0));
        manualBins.push_back(std::pair<double,double> ( 5.0,7.0));
        BOOST_REQUIRE_EQUAL(bins.size(), manualBins.size());
        for(unsigned int i=0; i<bins.size(); i++){
            BOOST_REQUIRE_CLOSE(bins[i].first, manualBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(bins[i].second, manualBins[i].second, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AccessOperator)

    BOOST_AUTO_TEST_CASE(AccessOperator1)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        double arbitraryDouble=4.3;
        BOOST_REQUIRE_NO_THROW(hist[arbitraryDouble]);
    }

    BOOST_AUTO_TEST_CASE(AccessOperator2)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        double arbitraryDouble=4.3, arbitraryObservable=1.7;
        hist[arbitraryDouble]=arbitraryObservable;
        BOOST_REQUIRE_CLOSE(hist[arbitraryDouble], arbitraryObservable, realFloatPrecisionInPercent);
        hist[arbitraryDouble]+=arbitraryObservable;
        BOOST_REQUIRE_CLOSE(hist[arbitraryDouble], 2*arbitraryObservable, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()
