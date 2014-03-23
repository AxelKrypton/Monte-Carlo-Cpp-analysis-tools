cmake_minimum_required (VERSION 2.8.3)
project(StATools)

# we have some custom CMake stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Macros)

# Have the user asked for tests
enable_testing()
# Enable Dashboard
include(CTest)

find_package(Boost REQUIRED COMPONENTS regex filesystem system program_options unit_test_framework)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_subdirectory(dataSample)
include_directories(${StATools_SOURCE_DIR}/dataSample)
link_directories(${StATools_BINARY_DIR}/dataSample)

add_subdirectory(FileContainer)
include_directories(${StATools_SOURCE_DIR}/FileContainer)
link_directories(${StATools_BINARY_DIR}/FileContainer)

add_executable(dataAnalysis dataAnalysis.cpp)
target_link_libraries(dataAnalysis dataAnalysisUtilities)

# add_library(StATools dataSample)
