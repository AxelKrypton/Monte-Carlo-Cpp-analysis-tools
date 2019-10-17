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

    BOOST_AUTO_TEST_CASE(getHeightOfSpecificBin1)
    {
        const double binsize=2.0;
        double manualHeight, height;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        height=hist.getHeightOfSpecificBin(2.0);
        manualHeight=3.0;
        BOOST_REQUIRE_CLOSE(height, manualHeight, realFloatPrecisionInPercent);
        height=hist.getHeightOfSpecificBin(4.0);
        manualHeight=0.0;
        BOOST_REQUIRE_CLOSE(height, manualHeight, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getHeightOfSpecificBin2)
    {
        const double binsize=2.0, anchor=-3.0;
        double manualHeight, height;
        Histogram hist(binsize, anchor);
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        height=hist.getHeightOfSpecificBin(4.0);
        manualHeight=0.0;
        BOOST_REQUIRE_CLOSE(height, manualHeight, realFloatPrecisionInPercent);
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

    BOOST_AUTO_TEST_CASE(getMiddleOfBins)
    {
        const double binsize=2.0;
        std::vector<double> middleOfBins, manualMiddleOfBins;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[1.3]=3.0;
        hist[6.3]=4.0;
        middleOfBins=hist.getMiddleOfBins();
        manualMiddleOfBins.push_back(0);
        manualMiddleOfBins.push_back(2);
        manualMiddleOfBins.push_back(6);
        BOOST_REQUIRE_EQUAL(middleOfBins.size(), manualMiddleOfBins.size());
        for(unsigned int i=0; i<middleOfBins.size(); i++){
            BOOST_REQUIRE_CLOSE(middleOfBins[i], manualMiddleOfBins[i], realFloatPrecisionInPercent);
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

    BOOST_AUTO_TEST_CASE(Exponentiate)
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

    BOOST_AUTO_TEST_CASE(Normalize1)
    {
        const double binsize=4.0;
        std::vector<double> heights, manualHeights;
        double areaOfHistogram;
        Histogram hist(binsize);
        hist[0.3]=1.0;
        hist[2.3]=3.0;
        hist[6.3]=2.0;
        hist.normalize();
        heights=hist.getHeightsOfBins();
        areaOfHistogram=1*4 + 3*4 + 2*4;
        manualHeights.push_back(1.0/areaOfHistogram);
        manualHeights.push_back(3.0/areaOfHistogram);
        manualHeights.push_back(2.0/areaOfHistogram);
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++)
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(Normalize2)
    {
        const double binsize=4.0;
        std::vector<double> heights;
        double areaOfHistogram=0.0;
        Histogram hist(binsize);
        hist[0.3]=-1.0;
        hist[2.3]=1.0;
        hist[6.3]=2.0;
        hist.normalize();
        heights=hist.getHeightsOfBins();
        for(unsigned int i=0; i<heights.size(); i++)
            areaOfHistogram+=heights[i]*binsize;
        BOOST_REQUIRE_CLOSE(areaOfHistogram, 1, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()


/******************************************************
***********Tests For histogramEstimators***************
*******************************************************/


BOOST_AUTO_TEST_SUITE(buildEstimator)

    BOOST_AUTO_TEST_CASE(buildEstimator1)
    {
        BOOST_REQUIRE_NO_THROW(HistogramEstimator histEstimator{1.5});
    }

    BOOST_AUTO_TEST_CASE(buildEstimator2)
    {
        double wrongbinsize=0.0;
        BOOST_REQUIRE_THROW(HistogramEstimator histEstimator(wrongbinsize), std::logic_error);
        wrongbinsize=-2.6;
        BOOST_REQUIRE_THROW(HistogramEstimator histEstimator(wrongbinsize), std::logic_error);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(gettersEstimator)

    BOOST_AUTO_TEST_CASE(getMultipleHeightsOfBins)
    {
        double binsize=2.0;
        HistogramEstimator histEstimator(binsize);
        std::vector<std::vector<double> > heights, manualHeights, manualHeightsWithZeroBins;
        histEstimator.insert(8.0,{15,27});
        histEstimator.insert(2.0,{23,13});
        histEstimator.insert(10.0,{33});
        histEstimator.insert(4.0,{24});
        heights=histEstimator.getMultipleHeightsOfBins();
        manualHeights.push_back({23,13});
        manualHeights.push_back({24});
        manualHeights.push_back({15,27});
        manualHeights.push_back({33});
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++){
            BOOST_REQUIRE_EQUAL(heights[i].size(), manualHeights[i].size());
            for(unsigned int j=0; j<heights[i].size(); j++)
                BOOST_REQUIRE_CLOSE(heights[i][j], manualHeights[i][j], realFloatPrecisionInPercent);
        }
        heights=histEstimator.getMultipleHeightsOfBins(true);
        manualHeightsWithZeroBins.push_back({23,13});
        manualHeightsWithZeroBins.push_back({24});
        manualHeightsWithZeroBins.push_back({0});
        manualHeightsWithZeroBins.push_back({15,27});
        manualHeightsWithZeroBins.push_back({33});
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeightsWithZeroBins.size());
        for(unsigned int i=0; i<heights.size(); i++){
            BOOST_REQUIRE_EQUAL(heights[i].size(), manualHeightsWithZeroBins[i].size());
            for(unsigned int j=0; j<heights[i].size(); j++)
                BOOST_REQUIRE_CLOSE(heights[i][j], manualHeightsWithZeroBins[i][j], realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(getHeightsOfBin)
    {
        double binsize=2.0;
        HistogramEstimator histEstimator(binsize);
        std::vector<double> heights, manualHeights;
        histEstimator.insert(8.0,{15.0,27.0});
        histEstimator.insert(2.0,{23.0,13.0});
        histEstimator.insert(10.0,{33.0});
        histEstimator.insert(4.0,{24.0});
        heights=histEstimator.getHeightsOfBin(8.0);
        manualHeights.push_back(15.0);
        manualHeights.push_back(27.0);
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++){
            BOOST_REQUIRE_CLOSE(heights[i], manualHeights[i], realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

/******************************************************
**********Tests For Probability Distribution***********
*******************************************************/

BOOST_AUTO_TEST_SUITE(buildProbDist)

    BOOST_AUTO_TEST_CASE(buildProbDist1)
    {
        Histogram hist(2);
        HistogramEstimator histEst(2);
        ErrorCalculationMethod errormethod{};
        BOOST_REQUIRE_NO_THROW(ProbabilityDistribution probDist(hist, histEst, errormethod));
    }

    BOOST_AUTO_TEST_CASE(buildProbDist2)//Only temporarily to see if it works
    {
        double binsize=2;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        hist[0.3]=3.0;
        hist[2.3]=4.2;
        hist[6.3]=1.8;
        histEst.insert(0.0,{2.9, 3.1, 2.8});
        histEst.insert(2.0,{4.1, 4.0, 4.4});
        histEst.insert(6.0,{1.7, 1.5, 2.1});
        ErrorCalculationMethod errormethod{bootstrap};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        for(int i=0; i<probDist.getNumberOfBins(); i++){
            double middleOfBin=probDist.getBins().at(i).first + 0.5*binsize;
            EstimateAndError estErr=probDist.getHeightsOfBins().at(i);
            std::cout << middleOfBin << " : " << estErr.estimate << " +/-" << estErr.error << std::endl;
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(gettersProbabilityDistribution)

    BOOST_AUTO_TEST_CASE(getNumberOfBinsProbDist1)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        bool dontIncludeZeroBins=false;
        bool includeZeroBins=true;
        BOOST_REQUIRE_EQUAL(probDist.getNumberOfBins(dontIncludeZeroBins), 0);
        BOOST_REQUIRE_EQUAL(probDist.getNumberOfBins(includeZeroBins), 0);
    }

    BOOST_AUTO_TEST_CASE(getNumberOfBinsProbDist2)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        bool dontIncludeZeroBins=false;
        bool includeZeroBins=true;
        probDist[0.3]=EstimateAndError(1.0, 0.1);
        probDist[1.3]=EstimateAndError(1.0, 0.1);
        probDist[6.3]=EstimateAndError(1.0, 0.1);
        BOOST_REQUIRE_EQUAL(probDist.getNumberOfBins(dontIncludeZeroBins), 3);
        BOOST_REQUIRE_EQUAL(probDist.getNumberOfBins(includeZeroBins), 4);
    }

    BOOST_AUTO_TEST_CASE(getBinsizeProbDist)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        BOOST_REQUIRE_CLOSE(probDist.getBinsize(), 2.0, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getHeightsOfBinsProbDist)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        std::vector<EstimateAndError> manualHeights, heights;
        std::vector<EstimateAndError> manualHeightsWithZeroBins, heightsWithZeroBins;
        bool includeZeroBins=true;
        probDist[0.3]=EstimateAndError(1.0, 0.1);
        probDist[1.3]=EstimateAndError(3.0, 0.1);
        probDist[6.3]=EstimateAndError(4.0, 0.1);
        heights=probDist.getHeightsOfBins();
        heightsWithZeroBins=probDist.getHeightsOfBins(includeZeroBins);
        manualHeights.push_back(EstimateAndError(1.0, 0.1));
        manualHeights.push_back(EstimateAndError(3.0, 0.1));
        manualHeights.push_back(EstimateAndError(4.0, 0.1));
        BOOST_REQUIRE_EQUAL(heights.size(), manualHeights.size());
        for(unsigned int i=0; i<heights.size(); i++){
            BOOST_REQUIRE_CLOSE(heights[i].estimate, manualHeights[i].estimate, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(heights[i].error, manualHeights[i].error, realFloatPrecisionInPercent);
        }
        manualHeightsWithZeroBins.push_back(EstimateAndError(1.0, 0.1));
        manualHeightsWithZeroBins.push_back(EstimateAndError(3.0, 0.1));
        manualHeightsWithZeroBins.push_back(EstimateAndError(0.0, 0.0));
        manualHeightsWithZeroBins.push_back(EstimateAndError(4.0, 0.1));
        BOOST_REQUIRE_EQUAL(heightsWithZeroBins.size(), manualHeightsWithZeroBins.size());
        for(unsigned int i=0; i<heightsWithZeroBins.size(); i++){
            BOOST_REQUIRE_CLOSE(heightsWithZeroBins[i].estimate, manualHeightsWithZeroBins[i].estimate, realFloatPrecisionInPercent);
            BOOST_REQUIRE_CLOSE(heightsWithZeroBins[i].error, manualHeightsWithZeroBins[i].error, realFloatPrecisionInPercent);
        }
    }

    BOOST_AUTO_TEST_CASE(getHeightOfSpecificBinProbDist1)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        EstimateAndError estAndErr, height;
        probDist[0.3]=EstimateAndError(2.2, 0.1);
        probDist[1.3]=EstimateAndError(1.2, 0.1);
        probDist[6.3]=EstimateAndError(4.2, 0.1);
        height=probDist.getHeightOfSpecificBin(0);
        estAndErr=EstimateAndError(2.2, 0.1);
        BOOST_REQUIRE_CLOSE(height.estimate, estAndErr.estimate, realFloatPrecisionInPercent);
        BOOST_REQUIRE_CLOSE(height.error, estAndErr.error, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getHeightOfSpecificBinProbDist2)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        EstimateAndError estAndErr, height;
        probDist[0.3]=EstimateAndError(2.2, 0.1);
        probDist[1.3]=EstimateAndError(1.2, 0.1);
        probDist[6.3]=EstimateAndError(4.2, 0.1);
        height=probDist.getHeightOfSpecificBin(4);
        estAndErr=EstimateAndError(0.0, 0.0);
        BOOST_REQUIRE_CLOSE(height.estimate, estAndErr.estimate, realFloatPrecisionInPercent);
        BOOST_REQUIRE_CLOSE(height.error, estAndErr.error, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(getMaxXvalueProbDist1)
        {
            const double binsize=2.0;
            Histogram hist(binsize);
            HistogramEstimator histEst(binsize);
            ErrorCalculationMethod errormethod{};
            ProbabilityDistribution probDist(hist, histEst, errormethod);
            double upperedge;
            probDist[0.3]=EstimateAndError(1.0,0.1);
            upperedge=1.0;
            BOOST_REQUIRE_CLOSE(probDist.getMaxXvalue(), upperedge, realFloatPrecisionInPercent);
        }

    BOOST_AUTO_TEST_CASE(getMaxXvalueProbDist2)
        {
            const double binsize=2.0;
            const double anchor=-3.5;
            Histogram hist(binsize, anchor);
            HistogramEstimator histEst(binsize);
            ErrorCalculationMethod errormethod{};
            ProbabilityDistribution probDist(hist, histEst, errormethod);
            const double upperedge=1.5;
            probDist[0.3]=EstimateAndError(1.0,0.1);
            BOOST_REQUIRE_CLOSE(probDist.getMaxXvalue(), upperedge, realFloatPrecisionInPercent);
        }

    BOOST_AUTO_TEST_CASE(getMinXvalueProbDist1)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        double loweredge;
        probDist[0.3]=EstimateAndError(1.0,0.1);
        loweredge=-1;
        BOOST_REQUIRE_EQUAL(probDist.getMinXvalue(), loweredge);
    }

    BOOST_AUTO_TEST_CASE(getMinXvalueProbDist2)
    {
        const double binsize=2.0;
        const double anchor=13.5;
        Histogram hist(binsize, anchor);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        const double loweredge=-1.5;
        probDist[0.3]=EstimateAndError(1.0,0.1);
        BOOST_REQUIRE_EQUAL(probDist.getMinXvalue(), loweredge);
    }

    BOOST_AUTO_TEST_CASE(getBinsProbDist1)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        std::vector<std::pair<double, double> > bins, manualBins;
        std::vector<std::pair<double, double> > binsWithZeroBins, manualBinsWithZeroBins;
        bool includeZeroBins=true;
        probDist[0.3]=EstimateAndError(1.0,0.1);
        probDist[1.3]=EstimateAndError(3.0,0.1);
        probDist[6.3]=EstimateAndError(4.0,0.1);
        bins=probDist.getBins();
        binsWithZeroBins=probDist.getBins(includeZeroBins);
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

    BOOST_AUTO_TEST_CASE(getBinsProbDist2)
    {
        const double binsize=2.0, anchor=5.5;
        Histogram hist(binsize, anchor);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        std::vector<std::pair<double, double> > bins, manualBins;
        std::vector<std::pair<double, double> > binsWithZeroBins, manualBinsWithZeroBins;
        bool includeZeroBins=true;
        probDist[0.3]=EstimateAndError(1.0,0.1);
        probDist[1.3]=EstimateAndError(3.0,0.1);
        probDist[6.3]=EstimateAndError(4.0,0.1);
        bins=probDist.getBins();
        binsWithZeroBins=probDist.getBins(includeZeroBins);
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

    BOOST_AUTO_TEST_CASE(getMiddleOfBinsProbDist)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        std::vector<double> middleOfBins, manualMiddleOfBins;
        probDist[0.3]=EstimateAndError(1.0,0.1);
        probDist[1.3]=EstimateAndError(3.0,0.1);
        probDist[6.3]=EstimateAndError(4.0,0.1);
        middleOfBins=probDist.getMiddleOfBins();
        manualMiddleOfBins.push_back(0);
        manualMiddleOfBins.push_back(2);
        manualMiddleOfBins.push_back(6);
        BOOST_REQUIRE_EQUAL(middleOfBins.size(), manualMiddleOfBins.size());
        for(unsigned int i=0; i<middleOfBins.size(); i++){
            BOOST_REQUIRE_CLOSE(middleOfBins[i], manualMiddleOfBins[i], realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OperatorProbDist)

    BOOST_AUTO_TEST_CASE(AccessOperatorProbDist1)
    {
        const double binsize=2.0, anchor=1.23;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        double arbitraryDouble=4.3;
        BOOST_REQUIRE_NO_THROW(probDist[arbitraryDouble]);
        Histogram hist2(binsize, anchor);
        ProbabilityDistribution probDist2(hist2, histEst, errormethod);
        BOOST_REQUIRE_NO_THROW(probDist2[arbitraryDouble]);
    }

    BOOST_AUTO_TEST_CASE(AccessOperatorProbDist2)
    {
        const double binsize=2.0;
        Histogram hist(binsize);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        double arbitraryDouble=4.3;
        EstimateAndError estAndErr=EstimateAndError(2.0, 0.2);
        probDist[arbitraryDouble]=estAndErr;
        BOOST_REQUIRE_CLOSE(probDist[arbitraryDouble].estimate, estAndErr.estimate, realFloatPrecisionInPercent);
        BOOST_REQUIRE_CLOSE(probDist[arbitraryDouble].error, estAndErr.error, realFloatPrecisionInPercent);
    }

    BOOST_AUTO_TEST_CASE(AccessOperatorProbDist3)
    {
        const double binsize=1.0, anchor=-5.5;
        Histogram hist(binsize, anchor);
        HistogramEstimator histEst(binsize);
        ErrorCalculationMethod errormethod{};
        ProbabilityDistribution probDist(hist, histEst, errormethod);
        double arbitraryDouble=4.3;
        EstimateAndError estAndErr=EstimateAndError(2.0, 0.2);
        probDist[arbitraryDouble]=estAndErr;
        BOOST_REQUIRE_CLOSE(probDist[arbitraryDouble].estimate, estAndErr.estimate, realFloatPrecisionInPercent);
        BOOST_REQUIRE_CLOSE(probDist[arbitraryDouble].error, estAndErr.error, realFloatPrecisionInPercent);
    }

BOOST_AUTO_TEST_SUITE_END()
