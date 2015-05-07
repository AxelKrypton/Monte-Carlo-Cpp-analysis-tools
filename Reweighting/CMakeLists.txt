add_library(Reweighting 
	SimulationData.cpp
	SimulationDataContainer.cpp
    MomentsReweighterHelper.cpp
	MomentsReweighter.cpp
	ReweighterIO.cpp
	Reweighter.cpp
)
target_link_libraries(Reweighting dataAnalysisUtilities)

#This is to copy a files to the build directory
add_custom_target(ReweightingFiles ALL
				  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/Reweighting/GeneralTestFiles $<TARGET_FILE_DIR:Reweighting>/GeneralTestFiles
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/Reweighting/RealTestData $<TARGET_FILE_DIR:Reweighting>/RealTestData)

add_unit_test(SimulationData SimulationData_test Reweighting dataAnalysisUtilities)
add_unit_test(SimulationDataContainer SimulationDataContainer_test Reweighting dataAnalysisUtilities)
add_unit_test(MomentsReweighterHelper MomentsReweighterHelper_test Reweighting dataAnalysisUtilities Parameters)
add_unit_test(MomentsReweighter MomentsReweighter_test Reweighting dataAnalysisUtilities Parameters)
add_unit_test(ReweighterIO ReweighterIO_test Reweighting dataAnalysisUtilities Parameters)
add_unit_test(Reweighter Reweighter_test Reweighting dataAnalysisUtilities Parameters)
