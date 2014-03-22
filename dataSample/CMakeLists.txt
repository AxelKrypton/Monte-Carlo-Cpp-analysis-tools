add_library(dataSample 
	dataSample.cpp
	jackknifeEstimators.cpp
)

configure_file(datafile.example datafile.example COPYONLY)
configure_file(datafile2.example datafile2.example COPYONLY)
configure_file(datafileWithTwoColumns.example datafileWithTwoColumns.example)

add_unit_test(dataSample dataSample_test)

# todo: the macro does not work here, jackknifeEstimators is not a library
#add_unit_test(jackknifeEstimators jackknifeEstimators_test)
add_executable(jackknifeEstimators_test jackknifeEstimators_test.cpp)
target_link_libraries(jackknifeEstimators_test dataSample ${Boost_LIBRARIES})
add_test(jackknifeEstimators jackknifeEstimators_test)