#include<iostream>

#include "dataSample.hpp"

void dataSample::printValuesToScreen()
{
	for (int iteration = 0; iteration < values.size(); iteration ++)
	{
		std::cout << values[iteration] << "\t";
	}
	std::cout << std::endl;
}

