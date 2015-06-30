add_library(IO
	io_utilities.cpp
)

add_unit_test(io_utilities io_utilities_test IO Parameters)

add_library(IO_Reweighting
	io_utilities_reweighting.cpp
)

if( USE_REW_MULTI_PRECISION )

	add_library(IO_MultiprecisionReweighting
		io_utilities_reweighting.cpp
	)
	target_compile_definitions(IO_MultiprecisionReweighting PUBLIC -D_USE_BIG_FLOAT_)
	
endif( USE_REW_MULTI_PRECISION )
