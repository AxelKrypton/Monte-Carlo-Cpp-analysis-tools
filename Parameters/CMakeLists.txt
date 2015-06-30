add_library(Parameters
	Parameters.cpp
)
add_unit_test(Parameters Parameters_test Parameters)

add_library(Parameters_Reweighting
	LqcdReweightingParameters.cpp
)

if( USE_REW_MULTI_PRECISION )

	add_library(Parameters_MultiprecisionReweighting
		LqcdReweightingParameters.cpp
	)
	target_compile_definitions(Parameters_MultiprecisionReweighting PUBLIC -D_USE_BIG_FLOAT_)
	
endif( USE_REW_MULTI_PRECISION )

add_unit_test(LqcdReweightingParameters LqcdReweightingParameters_test Parameters_Reweighting ${MPFR_LIBRARIES})