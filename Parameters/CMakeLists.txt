add_library(Parameters
	Parameters.cpp
	ReweightingParameters.cpp
)

add_unit_test(Parameters Parameters_test Parameters)
add_unit_test(ReweightingParameters ReweightingParameters_test Parameters)