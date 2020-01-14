/*
 *
 *  Copyright (c) 2015 Christopher Pinke
 *  Copyright (c) 2015-2016,2018,2020 Alessandro Sciarra
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
#define BOOST_TEST_MODULE Observables
#include <boost/test/unit_test.hpp>

#include "Observables.hpp"
#include "dataSampleTestUtilities.hpp" // for realFloatPrecisionInPercent

BOOST_AUTO_TEST_SUITE(moments)

	BOOST_AUTO_TEST_CASE(build)
    {
        BOOST_REQUIRE_NO_THROW(Moments moments);
    }

	BOOST_AUTO_TEST_CASE(setter_getter1)
	{
		unsigned int referenceIndex=12;
		realFloat referenceMoment = 3.14;
		Moments moment;
		moment.insert(referenceIndex, referenceMoment);
		BOOST_REQUIRE_THROW(moment[3], std::out_of_range);
		BOOST_REQUIRE_THROW(moment(referenceIndex), std::invalid_argument);
		BOOST_REQUIRE_EQUAL(referenceMoment, moment[referenceIndex]);
	}

	BOOST_AUTO_TEST_CASE(setter_getter2)
	{
		unsigned int referenceIndex=3;
		std::vector<realFloat> referenceMoment{3.14, 6.28, 9.42};
		Moments moment;
		for(size_t i=0; i<referenceMoment.size(); i++)
			moment.insert(referenceIndex, referenceMoment[i]);
		BOOST_REQUIRE_THROW(moment(4), std::out_of_range);
		BOOST_REQUIRE_THROW(moment[3], std::invalid_argument);
		BOOST_REQUIRE(moment(3) == referenceMoment);
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(momentsEstimators)

	BOOST_AUTO_TEST_CASE(build)
    {
        BOOST_REQUIRE_NO_THROW(MomentsEstimators momentsEst);
    }

	BOOST_AUTO_TEST_CASE(setter_getter1)
	{
		unsigned int referenceIndex=12;
		DataSample referenceMomentEst(std::valarray<realFloat>(3.14, 100));
		MomentsEstimators momentsEst;
		momentsEst.insert(referenceIndex, referenceMomentEst);
		BOOST_REQUIRE_THROW(momentsEst[3], std::out_of_range);
		BOOST_REQUIRE_THROW(momentsEst(referenceIndex), std::invalid_argument);
		for(int i=0; i<referenceMomentEst.getNumberOfElements(); i++)
			BOOST_REQUIRE_EQUAL(referenceMomentEst[i], momentsEst[referenceIndex][i]);
	}

	BOOST_AUTO_TEST_CASE(setter_getter2)
	{
		std::vector<int> referenceIndex{12, 7, 2};
		std::vector<DataSample> referenceMomentEst(3, DataSample(std::valarray<realFloat>(6.28, 100)));
		MomentsEstimators momentsEst;
		for(size_t i=0; i<referenceIndex.size(); i++)
			momentsEst.insert(referenceIndex[i], referenceMomentEst[i]);
		std::vector<DataSample> momentsEstSelected = momentsEst[ {12, 2} ];
		std::initializer_list<unsigned int> tmp = {12, 7, 2, 1};
		BOOST_REQUIRE_THROW(std::vector<DataSample> momentsEstSelectedWrong = momentsEst[tmp], std::out_of_range);
		for(int i=0; i<referenceMomentEst[0].getNumberOfElements(); i++){
			BOOST_REQUIRE_EQUAL(referenceMomentEst[0][i], momentsEstSelected[0][i]);
			BOOST_REQUIRE_EQUAL(referenceMomentEst[2][i], momentsEstSelected[1][i]);
		}
	}

	BOOST_AUTO_TEST_CASE(setter_getter3)
	{
		unsigned int referenceIndex=3;
		std::vector<DataSample> referenceMomentEst{DataSample(std::valarray<realFloat>(3.14, 100)), DataSample(std::valarray<realFloat>(6.28, 100))};
		MomentsEstimators momentsEst;
		for(size_t i=0; i<referenceMomentEst.size(); i++)
			momentsEst.insert(referenceIndex, referenceMomentEst[i]);
		BOOST_REQUIRE_THROW(momentsEst(4), std::out_of_range);
		BOOST_REQUIRE_THROW(momentsEst[3], std::invalid_argument);
		for(size_t i=0; i<referenceMomentEst.size(); i++){
			for(int j=0; j<referenceMomentEst[i].getNumberOfElements(); j++)
				BOOST_REQUIRE_EQUAL(momentsEst(referenceIndex)[i][j], referenceMomentEst[i][j]);
		}
	}

BOOST_AUTO_TEST_SUITE_END()


static Moments buildMomentsForTest(){
	Moments moments;
	moments.insert(1, 5.120788163699608e-01);
	moments.insert(2, 2.622374015645983e-01);
	moments.insert(3, 1.342992378238976e-01);
	moments.insert(4, 6.878181572513453e-02);
	return moments;
	/*
	 *     mean = 0.5120788163699608		-> zero mean: 0.0
	 * variance = 1.268738973830841e-05		-> zero mean: 0.2622374015645983
	 * skewness = 0.5694793357428045		-> zero mean: 1.000072760979389
	 * kurtosis = 3.7478114121524830		-> zero mean: 1.000194288875983
	 */
}

static Moments buildMomentsSeveralEstimateForTest(){
	Moments moments;
	for(int i=0; i<4; i++)
		moments.insert(1, 5.120788163699608e-01);
	moments.insert(2, 2.622374015645983e-01);
	moments.insert(3, 1.342992378238976e-01);
	moments.insert(4, 6.878181572513453e-02);
	return moments;
}

static MomentsEstimators buildMomentsEstimatorsForTest(){
	MomentsEstimators momentsEst;
	momentsEst.insert(1, DataSample(std::valarray<realFloat>({5.1235601107091922e-01, 5.1150824489887503e-01, 5.1172734937075659e-01, 5.1178725503695854e-01,
														   5.0979370987783490e-01, 5.1255624637967778e-01, 5.1202230784545366e-01, 5.1171955370408695e-01,
														   5.1187488596000053e-01, 5.0979885326724550e-01, 5.1346010441702183e-01, 5.0972776041563195e-01,
														   5.1024877919415301e-01, 5.1106580352847342e-01, 5.1195532496227270e-01, 5.1390447355869662e-01,
														   5.1202183781458366e-01, 5.1326137707280928e-01, 5.1267731347761258e-01, 5.1142797137376972e-01})));
	momentsEst.insert(2, DataSample(std::valarray<realFloat>({2.6252465475720638e-01, 2.6165170089094386e-01, 2.6187569175770908e-01, 2.6193691234537581e-01,
														   2.5990842550370130e-01, 2.6272340690594786e-01, 2.6217662123277541e-01, 2.6186580343559934e-01,
														   2.6202834168518563e-01, 2.5990976236280711e-01, 2.6365306166921659e-01, 2.5983595391311276e-01,
														   2.6037344532842710e-01, 2.6120041302190311e-01, 2.6210894186848277e-01, 2.6410567248106614e-01,
														   2.6217766022621947e-01, 2.6344694349572489e-01, 2.6284970759104292e-01, 2.6156899778943893e-01})));
	momentsEst.insert(3, DataSample(std::valarray<realFloat>({1.3452246987538111e-01, 1.3384830173963591e-01, 1.3402005031388509e-01, 1.3406698527826047e-01,
														   1.3251889318442522e-01, 1.3467028782742235e-01, 1.3425032974734333e-01, 1.3401099022082003e-01,
														   1.3413850486189055e-01, 1.3251691344901928e-01, 1.3538744578451081e-01, 1.3245946231125300e-01,
														   1.3287530606686845e-01, 1.3350305989651526e-01, 1.3419903990843887e-01, 1.3573319225558023e-01,
														   1.3425229075709791e-01, 1.3522713133192771e-01, 1.3476908916630512e-01, 1.3378440751335702e-01})));
	momentsEst.insert(4, DataSample(std::valarray<realFloat>({6.8936002646886763e-02, 6.8473202726632348e-02, 6.8590267467890556e-02, 6.8622260196942120e-02,
														   6.7572025855569748e-02, 6.9033623934637786e-02, 6.8746913222247338e-02, 6.8583080004194330e-02,
														   6.8671972940326412e-02, 6.7568629641372460e-02, 6.9525398204902958e-02, 6.7528892599278342e-02,
														   6.7814859824966087e-02, 6.8238427978485269e-02, 6.8712360871493086e-02, 6.9760160621270675e-02,
														   6.8749042841277780e-02, 6.9414579186828071e-02, 6.9102312901675886e-02, 6.8429331538084379e-02})));
	return momentsEst;
	/*
	 * bootstrap     mean = 1.1520239122276158e-03
	 * bootstrap variance = 3.0259317682406541e-06
	 * bootstrap skewness = not present in the reference code
	 * bootstrap kurtosis = 7.2132403917648602e-01
	 */
}

static MomentsEstimators buildMomentsEstimatorsSameEntryForTest(){
	MomentsEstimators momentsEst;
	momentsEst.insert(1, DataSample(std::valarray<realFloat>(5.126236900933244e-01, 100)));
	momentsEst.insert(2, DataSample(std::valarray<realFloat>(2.627932923896761e-01, 100)));
	momentsEst.insert(3, DataSample(std::valarray<realFloat>(1.347245894580630e-01, 100)));
	momentsEst.insert(4, DataSample(std::valarray<realFloat>(6.907112186983248e-02, 100)));
	return momentsEst;
}

static MomentsEstimators buildMomentsEstimatorsSameEntrySeveralEstimateForTest(){
	MomentsEstimators momentsEst;
	for(int i=0; i<4; i++)
		momentsEst.insert(1, DataSample(std::valarray<realFloat>(5.126236900933244e-01, 100)));
	momentsEst.insert(2, DataSample(std::valarray<realFloat>(2.627932923896761e-01, 100)));
	momentsEst.insert(3, DataSample(std::valarray<realFloat>(1.347245894580630e-01, 100)));
	momentsEst.insert(4, DataSample(std::valarray<realFloat>(6.907112186983248e-02, 100)));
	return momentsEst;
}


BOOST_AUTO_TEST_SUITE(MeanTest)

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
	{
		EstimateAndError referenceValue(0.0, 0.0);
		Mean mean(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
		BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(mean.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
	{
		EstimateAndError referenceValue(0.5120788163699608, 0.0);
		Mean mean(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(mean.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
	{
		EstimateAndError referenceValue(0.5120788163699608, 1.1520239122276158e-03);
		Mean mean(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
		//BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_CLOSE(mean.getValueAndError().error, referenceValue.error, realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
	{
		EstimateAndError referenceValue(0.0, 0.0);
		Mean mean(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);
		BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_CLOSE(mean.getValueAndError().error, referenceValue.error, realFloatPrecisionInPercent);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
	{
		EstimateAndError referenceValue(0.0, 0.0);
		Mean mean(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
		BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(mean.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator6)
	{
		EstimateAndError referenceValue(0.5120788163699608, 0.0);
		Mean mean(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
		BOOST_CHECK_CLOSE(mean.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(mean.getValueAndError().error, 1.e-7);
	}

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueNonZeroMean = 5.126236900933244e-01;
        DataSample resultNonZeroMean = Mean::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for(int i=0; i<resultNonZeroMean.getNumberOfElements(); i++){
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(VarianceTest)

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
	{
		EstimateAndError referenceValue(0.2622374015645983, 0.0);
		Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
		BOOST_CHECK_CLOSE(variance.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(variance.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
	{
		EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
		Variance variance(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(variance.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(variance.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
	{
		EstimateAndError referenceValue(1.268738973830841e-05, 3.0259317682406541e-06);
		Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(variance.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_CLOSE(variance.getValueAndError().error, referenceValue.error, realFloatPrecisionInPercent);
	}

	//TODO: Test for Variance variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
	{
		EstimateAndError referenceValue(0.2622374015645983, 0.0);
		Variance variance(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
		BOOST_CHECK_CLOSE(variance.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(variance.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
	{
		EstimateAndError referenceValue(1.268738973830841e-05, 0.0);
		Variance variance(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
		BOOST_CHECK_CLOSE(variance.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(variance	.getValueAndError().error, 1.e-7);
	}

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueZeroMean = 2.627932923896761e-01;
        double referenceValueNonZeroMean = 1.024474477934190e-05;
        DataSample resultZeroMean = Variance::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Variance::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for(int i=0; i<resultZeroMean.getNumberOfElements(); i++){
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(SkewnessTest)

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
	{
		EstimateAndError referenceValue(1.000072760979389, 0.0);
		Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
		BOOST_CHECK_CLOSE(skewness.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(skewness.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
	{
		EstimateAndError referenceValue(0.5694793357428045, 0.0);
		Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(skewness.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(skewness.getValueAndError().error, 1.e-7);
	}

	//TODO: Test for Skewness skewness(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
	//TODO: Test for Skewness variance(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
	{
		EstimateAndError referenceValue(1.000072760979389, 0.0);
		Skewness skewness(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
		BOOST_CHECK_CLOSE(skewness.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(skewness.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
	{
		EstimateAndError referenceValue(0.5694793357428045, 0.0);
		Skewness skewness(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
		BOOST_CHECK_CLOSE(skewness.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(skewness.getValueAndError().error, 1.e-7);
	}

    BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
        double referenceValueZeroMean = 1.0000586137849100;
        double referenceValueNonZeroMean = 0.5728362972699335;
        DataSample resultZeroMean = Skewness::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Skewness::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for(int i=0; i<resultZeroMean.getNumberOfElements(); i++){
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(KurtosisTest)

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator1)
	{
		EstimateAndError referenceValue(1.000194288875983, 0.0);
		Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), true, bootstrap);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(kurtosis.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator2)
	{
		EstimateAndError referenceValue(3.7478114121524830, 0.0);
		Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsSameEntryForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(kurtosis.getValueAndError().error, 3.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator3)
	{
		EstimateAndError referenceValue(3.7478114121524830, 7.2132403917648602e-01);
		Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsForTest(), false, bootstrap);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().error, referenceValue.error, realFloatPrecisionInPercent);
	}

	//TODO: Test for Kurtosis kurtosis(buildMomentsForTest(), buildMomentsEstimatorsForTest(), true, bootstrap);

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator4)
	{
		EstimateAndError referenceValue(1.000194288875983, 0.0);
		Kurtosis kurtosis(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), true, bootstrap, true);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(kurtosis.getValueAndError().error, 1.e-7);
	}

	BOOST_AUTO_TEST_CASE(fromMomentsAndEstimator5)
	{
		EstimateAndError referenceValue(3.7478114121524830, 0.0);
		Kurtosis kurtosis(buildMomentsSeveralEstimateForTest(), buildMomentsEstimatorsSameEntrySeveralEstimateForTest(), false, bootstrap, true);
		BOOST_CHECK_CLOSE(kurtosis.getValueAndError().estimate, referenceValue.estimate, realFloatPrecisionInPercent);
		BOOST_CHECK_SMALL(kurtosis.getValueAndError().error, 3.e-7);
	}

	BOOST_AUTO_TEST_CASE(observableFromMomentsEstimators)
    {
	    double referenceValueZeroMean = 1.00015649139308;
	    double referenceValueNonZeroMean = 3.36129098137853;
        DataSample resultZeroMean = Kurtosis::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), true);
        DataSample resultNonZeroMean = Kurtosis::evaluateObservableOnMomentEstimators(buildMomentsEstimatorsSameEntryForTest(), false);
        for(int i=0; i<resultZeroMean.getNumberOfElements(); i++){
            BOOST_CHECK_CLOSE(resultZeroMean[i], referenceValueZeroMean, realFloatPrecisionInPercent);
            BOOST_CHECK_CLOSE(resultNonZeroMean[i], referenceValueNonZeroMean, realFloatPrecisionInPercent);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

//TODO: Develop tests for Mean, Variance, Skewness, Kurtosis classes with Jackknife from estimators!























