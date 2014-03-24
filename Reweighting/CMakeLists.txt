add_library(Reweighting 
	SimulationData.cpp
	SimulationDataContainer.cpp
)
target_link_libraries(Reweighting dataAnalysisUtilities)

#This is to copy a file to the build directory
configure_file(input_existing_file input_existing_file COPYONLY)
configure_file(simulationData.example simulationData.example COPYONLY)

add_unit_test(SimulationData SimulationData_test)
add_unit_test(SimulationDataContainer SimulationDataContainer_test)
