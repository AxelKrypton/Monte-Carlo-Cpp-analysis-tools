add_library(FileContainer 
	FileContainer.cpp
	SimulationData.cpp
)

#This is to copy a file to the build directory
configure_file(input_existing_file input_existing_file COPYONLY)

add_unit_test(FileContainer FileContainer FileContainer_test)
#add_unit_test(SimulationData FileContainer SimulationData_test)
add_executable(SimulationData_test SimulationData_test.cpp)
target_link_libraries(SimulationData_test FileContainer dataAnalysisUtilities ${Boost_LIBRARIES})
add_test(SimulationData SimulationData_test)
