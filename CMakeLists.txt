cmake_minimum_required (VERSION 2.8.3)
project(StATools)

## we have some custom CMake stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Macros)

## Have the user asked for tests
enable_testing()

## Enable Dashboard
include(CTest)

# compiler settings
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wdisabled-optimization")
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

## These two variables are needed to create the main library StAT
## NOTE: So far they are empty, but they can be useful in future
set_abs_paths(StAT_FILES 
)
set(StAT_USED_LIBS ${Boost_LIBRARIES}
)

## Create the main library StAT
add_library(StAT ${StAT_FILES})

## Add to StAT each sub-directory linking it to the StAT library (see Macros.cmake in cmake folder)
add_modules(StAT
	dataAnalysisUtilities
	Reweighting
)

## Link used libraries to StAT
target_link_libraries(StAT ${StAT_USED_LIBS})
#set_target_properties(StAT PROPERTIES LINK_INTERFACE_MULTIPLICITY 4)

## The actual executables (frontends)
add_executable(dataAnalysis executables/dataAnalysis.cpp)
target_link_libraries(dataAnalysis StAT)