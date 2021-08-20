/*
 *
 *  Copyright (c) 2020-2021 Alessandro Sciarra
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

#pragma once

#include "../dataAnalysisUtilities/DataSample.hpp"
#include "../dataAnalysisUtilities/MultipleDataSample.hpp"
#include "../dataAnalysisUtilities/dataSampleTestUtilities.hpp"
#include "EstimateAndError.hpp"
#include "Moments.hpp"

#include <boost/test/unit_test.hpp>

void checkEstimateAndError(EstimateAndError expectedEstimateAndError, EstimateAndError calculatedEstimateAndError, realFloat testPrecision)
{
    if (expectedEstimateAndError.estimate == 0.0)
        BOOST_CHECK_SMALL(calculatedEstimateAndError.estimate, testPrecision);
    else
        BOOST_CHECK_CLOSE(expectedEstimateAndError.estimate, calculatedEstimateAndError.estimate, testPrecision);
    if (expectedEstimateAndError.error == 0.0)
        BOOST_CHECK_SMALL(calculatedEstimateAndError.error, testPrecision);
    else
        BOOST_CHECK_CLOSE(expectedEstimateAndError.error, calculatedEstimateAndError.error, testPrecision);
}

MultipleDataSample buildMultipleDataSampleForTest(bool differentSamples)
{
    const int numberOfEstimates = 8;
    const int numberOfElements = 10;
    DataSample positionData(makeValarrayWithArrayPosition(numberOfElements) + 1.0);
    std::vector<DataSample> data(numberOfEstimates, positionData);
    if (differentSamples) {
        data[0] = 1.0 / positionData;
        data[1] = DataSample(makeValarrayWithOnesAndMinusOnes(numberOfElements));
        data[2] = -0.05 * positionData;
        data[3] = 0.1 * DataSample(makeValarrayWithOnes(numberOfElements));
        data[4] = data[0];
        data[5] = data[1];
        data[6] = data[2];
        data[7] = data[3];
    }
    return MultipleDataSample(data);
    /*
     * differentSamples == TRUE
     *
     * Calculating (e.g. in Mathematica) the first four unbiased moments
     * per trajectory and averaging out over numberOfElements, one obtains
     *
     *   <UNB_1> = 0.0294742063492063492
     *   <UNB_2> = 0.0509514096974768743
     *   <UNB_3> = 0.00807768728853975453
     *   <UNB_4> = 0.00119897375418421337
     *
     * For the mean it follows that (<UNB_1> as central value and standard error)
     *
     *   Mean = 0.0294742063492063492 +- 0.0961058972090868503
     *
     * However, for the quantities beyond the mean, a Jackknife analysis is needed
     * and this can also quickly be implemented to get reference values.
     *
     *   Variance = 0.0490564204710764376 +- 0.0224903186229649300
     *   Skewness = 0.308128594671967048  +- 1.03523001446472049
     *   Kurtosis = 0.190964606824740572  +- 1.08281855793286145
     *
     * NOTE: Having a small(er) number of estimates per trajectory can lead to
     *       negative variance/kurtosis and hence to imaginary skewness. The guess
     *       here is that with "toy-numbers" the bias is basically larger then the
     *       quantity to be estimated.
     *
     *
     * differentSamples == FALSE
     *
     *       Mean = 5.5                 +- 0.957427107756338110
     *   Variance = 8.14814814814814815 +- 2.69124475919777037
     *   Skewness = 0.0                 +- 0.452121680616564996
     *   Kurtosis = 1.79234043214632025 +- 0.403117909936601779
     */
}

Moments buildMomentsForTest()
{
    Moments moments;
    moments.insert(1, 5.120788163699608e-01);
    moments.insert(2, 2.622374015645983e-01);
    moments.insert(3, 1.342992378238976e-01);
    moments.insert(4, 6.878181572513453e-02);
    return moments;
    /*
     *     mean = 0.5120788163699608        -> zero mean: 0.0
     * variance = 1.268738973830841e-05     -> zero mean: 0.2622374015645983
     * skewness = 0.5694793357428045        -> zero mean: 1.000072760979389
     * kurtosis = 3.7478114121524830        -> zero mean: 1.000194288875983
     */
}

MomentsEstimators buildMomentsEstimatorsForTest()
{
    MomentsEstimators momentsEst;
    momentsEst.insert(
        1, DataSample(std::valarray<realFloat>(
               {5.1235601107091922e-01, 5.1150824489887503e-01, 5.1172734937075659e-01, 5.1178725503695854e-01, 5.0979370987783490e-01,
                5.1255624637967778e-01, 5.1202230784545366e-01, 5.1171955370408695e-01, 5.1187488596000053e-01, 5.0979885326724550e-01,
                5.1346010441702183e-01, 5.0972776041563195e-01, 5.1024877919415301e-01, 5.1106580352847342e-01, 5.1195532496227270e-01,
                5.1390447355869662e-01, 5.1202183781458366e-01, 5.1326137707280928e-01, 5.1267731347761258e-01, 5.1142797137376972e-01})));
    momentsEst.insert(
        2, DataSample(std::valarray<realFloat>(
               {2.6252465475720638e-01, 2.6165170089094386e-01, 2.6187569175770908e-01, 2.6193691234537581e-01, 2.5990842550370130e-01,
                2.6272340690594786e-01, 2.6217662123277541e-01, 2.6186580343559934e-01, 2.6202834168518563e-01, 2.5990976236280711e-01,
                2.6365306166921659e-01, 2.5983595391311276e-01, 2.6037344532842710e-01, 2.6120041302190311e-01, 2.6210894186848277e-01,
                2.6410567248106614e-01, 2.6217766022621947e-01, 2.6344694349572489e-01, 2.6284970759104292e-01, 2.6156899778943893e-01})));
    momentsEst.insert(
        3, DataSample(std::valarray<realFloat>(
               {1.3452246987538111e-01, 1.3384830173963591e-01, 1.3402005031388509e-01, 1.3406698527826047e-01, 1.3251889318442522e-01,
                1.3467028782742235e-01, 1.3425032974734333e-01, 1.3401099022082003e-01, 1.3413850486189055e-01, 1.3251691344901928e-01,
                1.3538744578451081e-01, 1.3245946231125300e-01, 1.3287530606686845e-01, 1.3350305989651526e-01, 1.3419903990843887e-01,
                1.3573319225558023e-01, 1.3425229075709791e-01, 1.3522713133192771e-01, 1.3476908916630512e-01, 1.3378440751335702e-01})));
    momentsEst.insert(
        4, DataSample(std::valarray<realFloat>(
               {6.8936002646886763e-02, 6.8473202726632348e-02, 6.8590267467890556e-02, 6.8622260196942120e-02, 6.7572025855569748e-02,
                6.9033623934637786e-02, 6.8746913222247338e-02, 6.8583080004194330e-02, 6.8671972940326412e-02, 6.7568629641372460e-02,
                6.9525398204902958e-02, 6.7528892599278342e-02, 6.7814859824966087e-02, 6.8238427978485269e-02, 6.8712360871493086e-02,
                6.9760160621270675e-02, 6.8749042841277780e-02, 6.9414579186828071e-02, 6.9102312901675886e-02, 6.8429331538084379e-02})));
    return momentsEst;
    /*
     * bootstrap     mean = 1.1520239122276158e-03
     * bootstrap variance = 3.0259317682406541e-06
     * bootstrap skewness = not present in the reference code (Sanfilippo)
     * bootstrap kurtosis = 7.2132403917648602e-01
     */
}

MomentsEstimators buildMomentsEstimatorsSameEntryForTest()
{
    MomentsEstimators momentsEst;
    momentsEst.insert(1, DataSample(std::valarray<realFloat>(5.120788163699608e-01, 100)));
    momentsEst.insert(2, DataSample(std::valarray<realFloat>(2.622374015645983e-01, 100)));
    momentsEst.insert(3, DataSample(std::valarray<realFloat>(1.342992378238976e-01, 100)));
    momentsEst.insert(4, DataSample(std::valarray<realFloat>(6.878181572513453e-02, 100)));
    return momentsEst;
    /*
     * Use here the same (repeated) value as in buildMomentsForTest, so that reference values are the same
     */
}
