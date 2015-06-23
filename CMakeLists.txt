cmake_minimum_required (VERSION 2.8.3)
project(StATools)

#Check that the user deleted in the cache the precision variable
if(EXISTS CMakeCache.txt)
	message(WARNING "Before running cmake, delete the CMakeCache.txt file to be prompted about multiprecision option in the correct order.")
endif(EXISTS CMakeCache.txt)

## we have some custom CMake stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Macros)

## Have the user asked for tests
enable_testing()

## Enable Dashboard
include(CTest)

# compiler settings
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -Wdisabled-optimization")
	set(CMAKE_CXX_FLAGS_DEBUG     "-g -O0 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
	set(CMAKE_CXX_FLAGS_DEBUGFULL "-g3 -fno-linline")
endif(CMAKE_COMPILER_IS_GNUCXX)
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wc++-compat -Wdisabled-optimization -std=c99")
	set(CMAKE_C_FLAGS_DEBUG       "-g -O0 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
	set(CMAKE_C_FLAGS_DEBUGFULL   "-g3 -fno-linline")
endif(CMAKE_COMPILER_IS_GNUCC)

find_package(Boost REQUIRED COMPONENTS regex filesystem system program_options unit_test_framework)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package( MPFR REQUIRED )
include_directories( SYSTEM ${MPFR_INCLUDE_DIR} )

# Allow to choose whether to use multi-precision and in case the digits precision for reweighting
option( USE_REW_MULTI_PRECISION "Use arbitrary precision arithmetics in Reweighting" OFF )
if( USE_REW_MULTI_PRECISION )
	set(DIGITS_PREC "34" CACHE STRING "The number of decimal digits used in the Reweighting executable.")
	string(REGEX MATCH "^[0-9]+$" TMP "${DIGITS_PREC}")
	if(TMP STREQUAL "")
		message(SEND_ERROR "DIGITS_PREC must be an integer positive number. \"${DIGITS_PREC}\" is not a valid value.")
	endif(TMP STREQUAL "")
	add_definitions( -DNUM_DECIMAL_DIGITS=${DIGITS_PREC} )
else( USE_REW_MULTI_PRECISION )
	add_definitions( -DNUM_DECIMAL_DIGITS=15 )
endif( USE_REW_MULTI_PRECISION )

#Gather in a list all the external used libraries
set(StAT_USED_LIBS ${Boost_LIBRARIES} ${MPFR_LIBRARIES}
)

# Add to the project each sub-directory as it is, no linking to nothing (see Macros.cmake in cmake folder)
add_set_of_subdirectories(
	Parameters
	IO
	dataAnalysisUtilities
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
endif( USE_REW_MULTI_PRECISION )	
target_link_libraries(lqcdReweighting Reweighting Parameters_Reweighting IO_Reweighting ${StAT_USED_LIBS})

#include scripts-directory
add_subdirectory("${CMAKE_SOURCE_DIR}/Scripts")