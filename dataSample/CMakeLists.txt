add_library(dataAnalysisUtilities
	dataSample.cpp
	dataSampleAnalyzer.cpp
	jackknifeEstimators.cpp
)

configure_file(datafile.example datafile.example COPYONLY)
configure_file(datafile2.example datafile2.example COPYONLY)
configure_file(datafile3.example datafile3.example COPYONLY)
configure_file(datafileWithTwoColumns.example datafileWithTwoColumns.example)

#add_unit_test(dataSample dataSample_test)
add_executable(dataSample_test dataSample_test.cpp)
target_link_libraries(dataSample_test dataAnalysisUtilities ${Boost_LIBRARIES})
add_test(dataSample dataSample_test)

add_executable(dataSampleAnalyzer_test dataSampleAnalyzer_test.cpp)
target_link_libraries(dataSampleAnalyzer_test dataAnalysisUtilities ${Boost_LIBRARIES})
add_test(dataSampleAnalyzer dataSampleAnalyzer_test)

# todo: the macro does not work here, jackknifeEstimators is not a library
#add_unit_test(jackknifeEstimators jackknifeEstimators_test)
add_executable(jackknifeEstimators_test jackknifeEstimators_test.cpp)
target_link_libraries(jackknifeEstimators_test dataAnalysisUtilities ${Boost_LIBRARIES})
add_test(jackknifeEstimators jackknifeEstimators_test)