#
#  Copyright (c) 2014-2015,2019-2020 Alessandro Sciarra
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

#if( USE_REW_MULTI_PRECISION )
#	add_definitions( -D_USE_BIG_FLOAT_ )
#endif( USE_REW_MULTI_PRECISION )

if( USE_REW_MULTI_PRECISION )
	
	add_library(MultiprecisionReweighting 
		SimulationData.cpp
		SimulationDataContainer.cpp
	    MomentsReweighterHelper.cpp
		MomentsReweighter.cpp
		ReweighterIO.cpp
		Reweighter.cpp
		Histogram.cpp
	)
	target_compile_definitions(MultiprecisionReweighting PUBLIC -D_USE_BIG_FLOAT_)
	target_link_libraries(MultiprecisionReweighting supportSourceForMultiprecisionReweighting)

endif( USE_REW_MULTI_PRECISION )

add_library(Reweighting 
	SimulationData.cpp
	SimulationDataContainer.cpp
    MomentsReweighterHelper.cpp
	MomentsReweighter.cpp
	ReweighterIO.cpp
	Reweighter.cpp
	Histogram.cpp
)
target_link_libraries(Reweighting dataAnalysisUtilities)

#This is to copy all test files to the correct build directory
add_custom_target(ReweightingFiles ALL
				  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/Reweighting/GeneralTestFiles $<TARGET_FILE_DIR:Reweighting>/GeneralTestFiles
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/Reweighting/RealTestData $<TARGET_FILE_DIR:Reweighting>/RealTestData)

add_unit_test(SimulationData SimulationData_test Reweighting ${MPFR_LIBRARIES})
add_unit_test(SimulationDataContainer SimulationDataContainer_test Reweighting ${MPFR_LIBRARIES})
add_unit_test(MomentsReweighterHelper MomentsReweighterHelper_test Reweighting Parameters_Reweighting ${MPFR_LIBRARIES})
add_unit_test(MomentsReweighter MomentsReweighter_test Reweighting Parameters_Reweighting ${MPFR_LIBRARIES})
add_unit_test(ReweighterIO ReweighterIO_test Reweighting Parameters_Reweighting ${MPFR_LIBRARIES})
add_unit_test(Reweighter Reweighter_test Reweighting Parameters_Reweighting ${MPFR_LIBRARIES})
add_unit_test(Histogram Histogram_test Reweighting ${MPFR_LIBRARIES})
