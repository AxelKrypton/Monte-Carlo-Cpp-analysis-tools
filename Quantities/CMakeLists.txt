#
#  Copyright (c) 2014 Christopher Pinke
#  Copyright (c) 2014-2015,2020-2021 Alessandro Sciarra
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

add_library(Quantities
	Quantities.cpp
	AbstractQuantity.cpp
	Mean.cpp
	Variance.cpp
	Skewness.cpp
	Kurtosis.cpp
	Tools.cpp
)
target_link_libraries(Quantities dataAnalysisUtilities IO Parameters)

#Quantities needs the same test files needed in dataAnalysisUtilities -> make symbolic link
set(TEST_DATAFILES_BUILD_PATH ${PROJECT_BINARY_DIR}/dataAnalysisUtilities/SampleDatafiles)
set(TEST_DATAFILES_LINK_DEST ${CMAKE_CURRENT_BINARY_DIR}/SampleDatafiles)
execute_process(COMMAND ln -s -f ${TEST_DATAFILES_BUILD_PATH} ${TEST_DATAFILES_LINK_DEST}
                RESULT_VARIABLE LINKING_EXIT_CODE)
if(LINKING_EXIT_CODE EQUAL 0)
  message(STATUS "Linking of tests auxiliary files for Quantities folder successfully completed.")
else()
  message(FATAL_ERROR "Failed linking tests auxiliary files for Quantities folder!")
endif(LINKING_EXIT_CODE EQUAL 0)

add_unit_test(moments    Moments_test    dataAnalysisUtilities)
add_unit_test(quantities Quantities_test Parameters dataAnalysisUtilities)
add_unit_test(mean       Mean_test       Parameters dataAnalysisUtilities)
add_unit_test(variance   Variance_test   Parameters dataAnalysisUtilities)
add_unit_test(skewness   Skewness_test   Parameters dataAnalysisUtilities)
add_unit_test(kurtosis   Kurtosis_test   Parameters dataAnalysisUtilities)
