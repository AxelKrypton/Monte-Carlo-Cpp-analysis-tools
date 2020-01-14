#
#  Copyright (c)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#

add_library(dataAnalysisUtilities
	DataSampleBasic.cpp
	DataSample.cpp
	binnedDataSample.cpp
	jackknifeEstimators.cpp
	dataAnalysisUtilities.cpp
	DataSampleAnalyzer.cpp
	jackknifeAnalysis.cpp
	bootstrapAnalysis.cpp
	binning.cpp
	Observables.cpp
)
target_link_libraries(dataAnalysisUtilities IO Parameters)

if( USE_REW_MULTI_PRECISION )

	add_library(supportSourceForMultiprecisionReweighting
		DataSampleBasic.cpp
		DataSample.cpp
		binnedDataSample.cpp
		jackknifeEstimators.cpp
		jackknifeAnalysis.cpp
		bootstrapAnalysis.cpp
		binning.cpp
		Observables.cpp
	)
	target_compile_definitions(supportSourceForMultiprecisionReweighting PUBLIC -D_USE_BIG_FLOAT_)
	target_link_libraries(supportSourceForMultiprecisionReweighting IO Parameters ${MPFR_LIBRARIES})
		
endif( USE_REW_MULTI_PRECISION )


configure_file(datafile.example datafile.example COPYONLY)
configure_file(datafile2.example datafile2.example COPYONLY)
configure_file(datafile3.example datafile3.example COPYONLY)
configure_file(datafileWithTwoColumns.example datafileWithTwoColumns.example)

configure_file(gaussianNumbers_0_1_0_3.dat gaussianNumbers_0_1_0_3.dat COPYONLY)
configure_file(gaussianNumbers_0_1_1_3.dat gaussianNumbers_0_1_1_3.dat COPYONLY)

configure_file(emptyFile.example emptyFile.example COPYONLY)
configure_file(fileWithBrokenLine.example fileWithBrokenLine.example COPYONLY)
configure_file(fileWithComment.example fileWithComment.example COPYONLY)
configure_file(fileWithEmptyLine.example fileWithEmptyLine.example COPYONLY)
configure_file(../sampleDatafiles/gaussianNumbers_Berg.dat gaussianNumbers_Berg.dat COPYONLY)
configure_file(../sampleDatafiles/intTauBergRefResult32bins.dat intTauBergRefResult32bins.dat COPYONLY)
configure_file(../sampleDatafiles/autFuncBergRefResult32bins.dat autFuncBergRefResult32bins.dat COPYONLY)

add_unit_test(dataSampleBasic DataSampleBasic_test dataAnalysisUtilities)
add_unit_test(dataSample DataSample_test dataAnalysisUtilities)
add_unit_test(binnedDataSample binnedDataSample_test dataAnalysisUtilities)
add_unit_test(jackknifeEstimators jackknifeEstimators_test dataAnalysisUtilities)
add_unit_test(dataAnalysisUtilities dataAnalysisUtilities_test dataAnalysisUtilities Parameters)
add_unit_test(observables Observables_test dataAnalysisUtilities)
