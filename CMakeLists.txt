cmake_minimum_required (VERSION 2.6)
project(StATools)

add_subdirectory(dataSample)

include_directories(${StATools_SOURCE_DIR}/dataSample)

link_directories(${StATools_BINARY_DIR}/dataSample)

add_executable(dataAnalysis dataAnalysis.cpp)

target_link_libraries(dataAnalysis dataSample)
