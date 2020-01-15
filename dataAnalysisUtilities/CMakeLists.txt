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

#Extract test files directly to the build correct folder
set(TEST_DATAFILES_ARCHIVE_PATH ${CMAKE_SOURCE_DIR}/dataAnalysisUtilities/SampleDatafiles/testAuxiliaryDataFiles.tar.xz)
set(TEST_DATAFILES_BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/SampleDatafiles)
file(MAKE_DIRECTORY ${TEST_DATAFILES_BUILD_PATH})
execute_process(COMMAND tar -xf ${TEST_DATAFILES_ARCHIVE_PATH}
                WORKING_DIRECTORY ${TEST_DATAFILES_BUILD_PATH}
                RESULT_VARIABLE EXTRACTION_EXIT_CODE)
if(EXTRACTION_EXIT_CODE EQUAL 0)
  message(STATUS "Extraction of tests auxiliary files successfully completed.")
else()
  message(FATAL_ERROR "Failed extracting files from ${CMAKE_SOURCE_DIR}/sampleDatafiles/testAuxiliaryDataFiles.tar.xz archive!")
endif(EXTRACTION_EXIT_CODE EQUAL 0)

add_unit_test(dataSampleBasic DataSampleBasic_test dataAnalysisUtilities)
add_unit_test(dataSample DataSample_test dataAnalysisUtilities)
add_unit_test(binnedDataSample binnedDataSample_test dataAnalysisUtilities)
add_unit_test(jackknifeEstimators jackknifeEstimators_test dataAnalysisUtilities)
add_unit_test(dataAnalysisUtilities dataAnalysisUtilities_test dataAnalysisUtilities Parameters)
add_unit_test(observables Observables_test dataAnalysisUtilities)
