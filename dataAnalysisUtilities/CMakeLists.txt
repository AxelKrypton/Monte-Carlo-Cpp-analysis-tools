#
#  Copyright (c) 2014 Christopher Pinke
#  Copyright (c) 2014-2015,2020 Alessandro Sciarra
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

#This is to copy all test files to the correct build directory
add_custom_target(SampleDataFiles ALL
				  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/dataAnalysisUtilities/SampleDatafiles $<TARGET_FILE_DIR:dataAnalysisUtilities>/SampleDatafiles)

add_unit_test(dataSampleBasic DataSampleBasic_test dataAnalysisUtilities)
add_unit_test(dataSample DataSample_test dataAnalysisUtilities)
add_unit_test(binnedDataSample binnedDataSample_test dataAnalysisUtilities)
add_unit_test(jackknifeEstimators jackknifeEstimators_test dataAnalysisUtilities)
add_unit_test(dataAnalysisUtilities dataAnalysisUtilities_test dataAnalysisUtilities Parameters)
add_unit_test(observables Observables_test dataAnalysisUtilities)
