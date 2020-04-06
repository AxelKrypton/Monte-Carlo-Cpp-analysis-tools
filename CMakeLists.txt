#
#  Copyright (c) 2014 Christopher Pinke
#  Copyright (c) 2014-2015,2019-2020 Alessandro Sciarra
#  Copyright (c) 2019 David Leemueller
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

# CMakeLists.txt
project(StATools)
cmake_minimum_required (VERSION 3.1) # -> CMAKE_CXX_STANDARD

## we have some custom CMake stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Macros)

# if a user doesn't specify anything he probably isn't interested in debug information
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

## Have the user asked for tests
enable_testing()

## Enable Dashboard
include(CTest)

# Compiler settings (https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html)
set(CMAKE_CXX_STANDARD 11)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Compiling with ${CMAKE_CXX_COMPILER_ID}")
    add_compile_options(
        "$<$<CONFIG:RELEASE>:-Wall;-Wextra;-pedantic>"
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Compiling with ${CMAKE_CXX_COMPILER_ID}")
    add_compile_options(
        "$<$<CONFIG:RELEASE>:-Wall;-Wextra;-pedantic;-Wdisabled-optimization>"
        "$<$<CONFIG:DEBUG>:-g3;-O0;-fno-reorder-blocks;-fno-schedule-insns;-fno-inline>"
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(FATAL_ERROR "Compiling with Intel Compiler is not supported")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(FATAL_ERROR "Compiling with Microsoft Visual Studio is not supported")
else()
    message(FATAL_ERROR "Compiling with ${CMAKE_CXX_COMPILER_ID} is not supported")
endif()

find_package(Boost REQUIRED COMPONENTS regex filesystem system program_options unit_test_framework)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package( MPFR REQUIRED )
include_directories( SYSTEM ${MPFR_INCLUDE_DIR} )

# Allow to choose whether to use multi-precision and in case the digits precision for reweighting
option( USE_REW_MULTI_PRECISION "Use arbitrary precision arithmetics in Reweighting" OFF )
if( USE_REW_MULTI_PRECISION )
	set(USE_REW_NUM_DECIMAL_DIGITS "34" CACHE STRING "The number of decimal digits used in the Reweighting executable (ignored if USE_REW_MULTI_PRECISION==OFF).")
	string(REGEX MATCH "^[0-9]+$" TMP "${USE_REW_NUM_DECIMAL_DIGITS}")
	if(TMP STREQUAL "")
		message(SEND_ERROR "USE_REW_NUM_DECIMAL_DIGITS must be an integer positive number. \"${USE_REW_NUM_DECIMAL_DIGITS}\" is not a valid value.")
	endif(TMP STREQUAL "")
	add_definitions( -DNUM_DECIMAL_DIGITS=${USE_REW_NUM_DECIMAL_DIGITS} )
else( USE_REW_MULTI_PRECISION )
	add_definitions( -DNUM_DECIMAL_DIGITS=15 )
endif( USE_REW_MULTI_PRECISION )

#Gather in a list all the external used libraries
set(StAT_USED_LIBS ${Boost_LIBRARIES} ${MPFR_LIBRARIES}
)

# Add to the project each sub-directory as it is, no linking to nothing (see Macros.cmake in cmake folder)
add_set_of_subdirectories(
    dataAnalysisUtilities
    IO
    Parameters
    Quantities
    Reweighting
)

# The actual executables (frontends)
#
# NOTE: Here, the linking of the executable is done manually since some
#       sub-libraries are compiled with the definition _USE_BIG_FLOAT_ and
#       some others no.
#
add_executable(dataAnalysis executables/dataAnalysis.cpp)
target_link_libraries(dataAnalysis dataAnalysisUtilities Parameters IO ${StAT_USED_LIBS})

add_executable(lqcdReweighting executables/lqcdReweighting.cpp)
if( USE_REW_MULTI_PRECISION )
	target_compile_definitions(lqcdReweighting PUBLIC -D_USE_BIG_FLOAT_)
	target_link_libraries(lqcdReweighting MultiprecisionReweighting Parameters_MultiprecisionReweighting IO_MultiprecisionReweighting ${StAT_USED_LIBS})
else( USE_REW_MULTI_PRECISION )
	target_link_libraries(lqcdReweighting Reweighting Parameters_Reweighting IO_Reweighting ${StAT_USED_LIBS})
endif( USE_REW_MULTI_PRECISION )


#include scripts-directory
add_subdirectory("${CMAKE_SOURCE_DIR}/Scripts")
