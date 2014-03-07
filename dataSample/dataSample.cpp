#include<iostream>

#include "dataSample.hpp"

dataSample::dataSample(std::valarray<double> valuesIn)
{
	values = valuesIn;
}

void dataSample::printValuesToScreen()
{
	for (int iteration = 0; iteration < values.size(); iteration ++)
	{
		std::cout << values[iteration] << "\t";
	}
	std::cout << std::endl;
}

double dataSample::getMean()
{
	return mean;
}

