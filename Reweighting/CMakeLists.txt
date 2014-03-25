add_library(Reweighting 
	SimulationData.cpp
	SimulationDataContainer.cpp
)

#This is to copy a file to the build directory
configure_file(simulationDataContainer.configfile simulationDataContainer.configfile COPYONLY)
configure_file(input_test_file_1 input_test_file_1 COPYONLY)
configure_file(input_test_file_2 input_test_file_2 COPYONLY)
configure_file(input_test_file_3 input_test_file_3 COPYONLY)

add_unit_test(SimulationData SimulationData_test StAT)
add_unit_test(SimulationDataContainer SimulationDataContainer_test Reweighting dataAnalysisUtilities)
