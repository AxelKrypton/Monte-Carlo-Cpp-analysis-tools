#
#  Copyright (c)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#

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