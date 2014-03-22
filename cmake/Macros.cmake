# Utility functions for specifying the build of OpTiMaL
#
# Copyright (c) 2012 Matthias Bach <bach@compeng.uni-frankfurt.de>
#
# This file is part of CL2QCD.
#
# CL2QCD is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CL2QCD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CL2QCD.  If not, see <http://www.gnu.org/licenses/>.

# Add a unit test
# NAME name for the test to use in ctest
# EXE name for the executable to use
# The test is expectet to have only one source
# file called EXE.cpp and will be linked automatically
macro(add_unit_test NAME EXE)
	add_executable("${EXE}" "${EXE}.cpp")
	target_link_libraries("${EXE}" ${NAME} ${Boost_LIBRARIES})
	add_test("${NAME}" "${EXE}")
endmacro()
