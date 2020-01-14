/*
 *
 *  Copyright (c) 2014 Christopher Pinke
 *  Copyright (c) 2016,2018,2020 Alessandro Sciarra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../Parameters/Parameters.hpp"
#include "../dataAnalysisUtilities/datafileAnalyzer.hpp"
#include "exceptions.hpp"
#include "exitCodes.hpp"

int main(int argc, const char ** argv)
{
	try
	{
		Parameters parameters(argc, argv);
		std::cout << std::endl << "#------------------------------------------------------------------------------------" << std::endl;
		DatafileAnalyzer analyzer(parameters);
		std::cout << "#------------------------------------------------------------------------------------" << std::endl << std::endl;
	}
	//todo: move catch block into own function?
	catch ( wrongBinningParameter &e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_BINNING_ERROR);
	}
	catch ( const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_ERROR);
	}
	catch(Parameters::parse_aborted&)
	{
		//do nothing in this case...
	}
	catch (...)
	{
		std::cout << "Caught non-standard exception!";
		exit(EXIT_ERROR);
	}

    return EXIT_NORMAL;
}
