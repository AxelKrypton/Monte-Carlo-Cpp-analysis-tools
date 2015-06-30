add_library(dataAnalysisUtilities
	DataSampleBasic.cpp
	DataSample.cpp
	binnedDataSample.cpp
	jackknifeEstimators.cpp
	dataAnalysisUtilities.cpp
	DataSampleAnalyzer.cpp
	jackknifeAnalysis.cpp
	bootstrapAnalysis.cpp
	binning.cpp
	Observables.cpp
)
target_link_libraries(dataAnalysisUtilities IO Parameters)

if( USE_REW_MULTI_PRECISION )

	add_library(supportSourceForMultiprecisionReweighting
		DataSampleBasic.cpp
		DataSample.cpp
		binnedDataSample.cpp
		jackknifeEstimators.cpp
		jackknifeAnalysis.cpp
		bootstrapAnalysis.cpp
		binning.cpp
		Observables.cpp
	)
	target_compile_definitions(supportSourceForMultiprecisionReweighting PUBLIC -D_USE_BIG_FLOAT_)
	target_link_libraries(supportSourceForMultiprecisionReweighting IO Parameters ${MPFR_LIBRARIES})
		
endif( USE_REW_MULTI_PRECISION )


configure_file(datafile.example datafile.example COPYONLY)
configure_file(datafile2.example datafile2.example COPYONLY)
configure_file(datafile3.example datafile3.example COPYONLY)
configure_file(datafileWithTwoColumns.example datafileWithTwoColumns.example)

configure_file(gaussianNumbers_0_1_0_3.dat gaussianNumbers_0_1_0_3.dat COPYONLY)
configure_file(gaussianNumbers_0_1_1_3.dat gaussianNumbers_0_1_1_3.dat COPYONLY)

configure_file(emptyFile.example emptyFile.example COPYONLY)
configure_file(fileWithBrokenLine.example fileWithBrokenLine.example COPYONLY)
configure_file(fileWithComment.example fileWithComment.example COPYONLY)
configure_file(fileWithEmptyLine.example fileWithEmptyLine.example COPYONLY)
configure_file(../sampleDatafiles/gaussianNumbers_Berg.dat gaussianNumbers_Berg.dat COPYONLY)
configure_file(../sampleDatafiles/intTauBergRefResult32bins.dat intTauBergRefResult32bins.dat COPYONLY)
configure_file(../sampleDatafiles/autFuncBergRefResult32bins.dat autFuncBergRefResult32bins.dat COPYONLY)

add_unit_test(dataSampleBasic DataSampleBasic_test dataAnalysisUtilities)
add_unit_test(dataSample DataSample_test dataAnalysisUtilities)
add_unit_test(binnedDataSample binnedDataSample_test dataAnalysisUtilities)
add_unit_test(jackknifeEstimators jackknifeEstimators_test dataAnalysisUtilities)
add_unit_test(dataAnalysisUtilities dataAnalysisUtilities_test dataAnalysisUtilities Parameters)
add_unit_test(observables Observables_test dataAnalysisUtilities)
