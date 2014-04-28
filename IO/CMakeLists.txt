add_library(IO
	io_utilities.cpp
)

add_unit_test(io_utilities io_utilities_test IO Parameters)