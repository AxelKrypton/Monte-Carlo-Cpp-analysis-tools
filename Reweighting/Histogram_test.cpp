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

    BOOST_AUTO_TEST_CASE(getNumberOfBins1)
    {
        const double binsize=2.0;
        bool dontIncludeZeroBins=false;
        bool includeZeroBins=true;
        Histogram hist(binsize);
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(dontIncludeZeroBins), 0);
        BOOST_REQUIRE_EQUAL(hist.getNumberOfBins(includeZeroBins), 0);
    }

    BOOST_AUTO_TEST_CASE(getNumberOfBins2)
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

    BOOST_AUTO_TEST_CASE(getBinsize)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        BOOST_REQUIRE_CLOSE(hist.getBinsize(), 2.0, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getHeightsOfBins)
    {
        const double binsize=2.0;
        std::vector<double> manualHeights, heights;
        std::vector<double> manualHeightsWithZeroBins, heightsWithZeroBins;
        bool includeZeroBins=true;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        heights=hist.getHeightsOfBins();
        heightsWithZeroBins=hist.getHeightsOfBins(includeZeroBins);
        manualHeights.push_back(1.0);
        manualHeights.push_back(3.0);
        manualHeights.push_back(4.0);
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++)
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
        
        manualHeightsWithZeroBins.push_back(1.0);
        manualHeightsWithZeroBins.push_back(3.0);
        manualHeightsWithZeroBins.push_back(0.0);
        manualHeightsWithZeroBins.push_back(4.0);
        BOOST_REQUIRE_EQUAL(heightsWithZeroBins.size(), manualHeightsWithZeroBins.size());
        for(unsigned int i=0; i<heightsWithZeroBins.size(); i++)
            BOOST_REQUIRE_CLOSE(heightsWithZeroBins[i], manualHeightsWithZeroBins[i], realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getMaxXvalue1)
        {
            const double binsize=2.0;
            double upperedge;
            Histogram hist(binsize);
            hist[0.3]=1.0;
            upperedge=1.0;
            BOOST_REQUIRE_CLOSE(hist.getMaxXvalue(), upperedge, realFloatPrecisionInPercent);
        }

    BOOST_AUTO_TEST_CASE(getMaxXvalue2)
        {
            const double binsize=2.0;
            const double anchor=-3.5;
            const double upperedge=1.5;
            Histogram hist(binsize, anchor);
            hist[0.3]=1.0;
            BOOST_REQUIRE_CLOSE(hist.getMaxXvalue(), upperedge, realFloatPrecisionInPercent);
        }

    BOOST_AUTO_TEST_CASE(getMinXvalue1)
    {
        const double binsize=2.0;
        double loweredge;
        Histogram hist(binsize);
        hist[0.3]=1;
        loweredge=-1;
        BOOST_REQUIRE_EQUAL(hist.getMinXvalue(), loweredge);
    }

    BOOST_AUTO_TEST_CASE(getMinXvalue2)
    {
        const double binsize=2.0;
        const double anchor=13.5;
        const double loweredge=-1.5;
        Histogram hist(binsize, anchor);
        hist[0.3]=1;
        BOOST_REQUIRE_EQUAL(hist.getMinXvalue(), loweredge);
    }

    BOOST_AUTO_TEST_CASE(getBins1)
    {
        const double binsize=2.0;
        std::vector<std::pair<double, double> > bins, manualBins;
        std::vector<std::pair<double, double> > binsWithZeroBins, manualBinsWithZeroBins;
        bool includeZeroBins=true;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        bins=hist.getBins();
        binsWithZeroBins=hist.getBins(includeZeroBins);
        manualBins.push_back(std::pair<double,double> (-1.0,1.0));
        manualBins.push_back(std::pair<double,double> ( 1.0,3.0));
        manualBins.push_back(std::pair<double,double> ( 5.0,7.0));
        BOOST_REQUIRE_EQUAL(bins.size(), manualBins.size());
        for(unsigned int i=0; i<bins.size(); i++){
            BOOST_REQUIRE_CLOSE(bins[i].first, manualBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(bins[i].second, manualBins[i].second, realFloatPrecisionInPercent);
        }

        manualBinsWithZeroBins.push_back(std::pair<double,double> (-1.0,1.0));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 1.0,3.0));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 3.0,5.0));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 5.0,7.0));
        BOOST_REQUIRE_EQUAL(binsWithZeroBins.size(), manualBinsWithZeroBins.size());
        for(unsigned int i=0; i<binsWithZeroBins.size(); i++){
            BOOST_REQUIRE_CLOSE(binsWithZeroBins[i].first, manualBinsWithZeroBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(binsWithZeroBins[i].second, manualBinsWithZeroBins[i].second, realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(getBins2)
    {
        const double binsize=2.0, anchor=5.5;
        std::vector<std::pair<double, double> > bins, manualBins;
        std::vector<std::pair<double, double> > binsWithZeroBins, manualBinsWithZeroBins;
        bool includeZeroBins=true;
        Histogram hist(binsize, anchor);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        bins=hist.getBins();
        binsWithZeroBins=hist.getBins(includeZeroBins);
        manualBins.push_back(std::pair<double,double> (-1.5,0.5));
        manualBins.push_back(std::pair<double,double> ( 0.5,2.5));
        manualBins.push_back(std::pair<double,double> ( 4.5,6.5));
        BOOST_REQUIRE_EQUAL(bins.size(), manualBins.size());
        for(unsigned int i=0; i<bins.size(); i++){
            BOOST_REQUIRE_CLOSE(bins[i].first, manualBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(bins[i].second, manualBins[i].second, realFloatPrecisionInPercent);
        }

        manualBinsWithZeroBins.push_back(std::pair<double,double> (-1.5,0.5));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 0.5,2.5));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 2.5,4.5));
        manualBinsWithZeroBins.push_back(std::pair<double,double> ( 4.5,6.5));
        BOOST_REQUIRE_EQUAL(binsWithZeroBins.size(), manualBinsWithZeroBins.size());
        for(unsigned int i=0; i<binsWithZeroBins.size(); i++){
            BOOST_REQUIRE_CLOSE(binsWithZeroBins[i].first, manualBinsWithZeroBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(binsWithZeroBins[i].second, manualBinsWithZeroBins[i].second, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operator)

    BOOST_AUTO_TEST_CASE(AccessOperator1)
    {
        const double binsize=2.0, anchor=1.23;
        Histogram hist(binsize);
        double arbitraryDouble=4.3;
        BOOST_REQUIRE_NO_THROW(hist[arbitraryDouble]);
        Histogram hist2(binsize, anchor);
        BOOST_REQUIRE_NO_THROW(hist2[arbitraryDouble]);
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

    BOOST_AUTO_TEST_CASE(AccessOperator3)
    {
        const double binsize=1.0, anchor=-5.5;
        Histogram hist(binsize, anchor);
        double arbitraryDouble=4.3, arbitraryObservable=1.7;
        hist[arbitraryDouble]=arbitraryObservable;
        BOOST_REQUIRE_CLOSE(hist[arbitraryDouble], arbitraryObservable, realFloatPrecisionInPercent);
        hist[arbitraryDouble]+=arbitraryObservable;
        BOOST_REQUIRE_CLOSE(hist[arbitraryDouble], 2*arbitraryObservable, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(SubtractionOperator)
    {
        const double binsize=4.0;
        std::vector<double> heights, manualHeights;
        double arbitraryDouble=3.0;
        Histogram hist(binsize);
        hist[0.3]=4.0;
        hist[2.3]=12.0;
        hist[6.3]=16.0;
        hist-=arbitraryDouble;
        heights=hist.getHeightsOfBins();
        manualHeights.push_back(1.0);
        manualHeights.push_back(9.0);
        manualHeights.push_back(13.0);
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++)
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(Exponetiate)
    {
        const double binsize=4.0;
        std::vector<double> heights, manualHeights;
        Histogram hist(binsize);
        hist[0.3]=-1.0;
        hist[2.3]=1.0;
        hist[6.3]=2.0;
        hist.exponentiateHeights();
        heights=hist.getHeightsOfBins();
        manualHeights.push_back(std::exp(-1.0));
        manualHeights.push_back(std::exp(+1.0));
        manualHeights.push_back(std::exp(+2.0));
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++)
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(Shift1)
    {
        const double binsize=0.5, delta=3.0;
        std::vector<std::pair<double, double> > binsBefore, binsAfter;
        Histogram hist(binsize);
        hist[-0.9]=1.5;
        hist[2.3]=1.8;
        hist[6.3]=2.7;
        binsBefore=hist.getBins();
        hist.shift(delta);
        hist.shift(-delta);
        binsAfter=hist.getBins();
        //Bins should not have changed
        BOOST_REQUIRE_EQUAL(binsBefore.size(), binsAfter.size());
        for(unsigned int i=0; i<binsBefore.size(); i++)
        {
            BOOST_REQUIRE_CLOSE(binsBefore[i].first, binsAfter[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(binsBefore[i].second, binsAfter[i].second, realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(Shift2)
    {
        const double binsize=0.5, delta=3.0;
        std::vector<std::pair<double, double> > bins, manualShiftedBins;
        Histogram hist(binsize);
        hist[-0.9]=1.5;
        hist[2.3]=1.8;
        hist[6.3]=2.7;
        hist.shift(delta);
        bins=hist.getBins();
        manualShiftedBins.push_back(std::pair<double,double>(-1.25,-0.75));
        manualShiftedBins.push_back(std::pair<double,double>(2.25,2.75));
        manualShiftedBins.push_back(std::pair<double,double>(6.25,6.75));
        for(auto& bin : manualShiftedBins)
        {
            bin.first+=delta;
            bin.second+=delta;
        }
        BOOST_REQUIRE_EQUAL(bins.size(), manualShiftedBins.size());
        for(unsigned int i=0; i<bins.size(); i++)
        {
            BOOST_REQUIRE_CLOSE(bins[i].first, manualShiftedBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(bins[i].second, manualShiftedBins[i].second, realFloatPrecisionInPercent);
        }
    }


    BOOST_AUTO_TEST_CASE(Shift3)
    {
        const double binsize=2, delta=3.0, anchor=-13;
        std::vector<std::pair<double, double> > bins, manualShiftedBins;
        Histogram hist(binsize, anchor);
        hist[1.0]=1.5;
        hist[2.0]=1.8;
        hist[6.0]=2.7;
        hist.shift(delta);
        bins=hist.getBins();
        manualShiftedBins.push_back(std::pair<double,double>(0.0,2.0));
        manualShiftedBins.push_back(std::pair<double,double>(4.0,6.0));
        for(auto& bin : manualShiftedBins)
        {
            bin.first+=delta;
            bin.second+=delta;
        }
        BOOST_REQUIRE_EQUAL(bins.size(), manualShiftedBins.size());
        for(unsigned int i=0; i<bins.size(); i++)
        {
            BOOST_REQUIRE_CLOSE(bins[i].first, manualShiftedBins[i].first, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(bins[i].second, manualShiftedBins[i].second, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
