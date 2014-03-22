#include "jackknifeEstimators.hpp"

double JackknifeEstimators::getJackknifeVariance()
{
	return ( createShiftedDataSample(2, getNthMoment(1)) ).getNthMoment(1) * getJackknifeNormalization();
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed(numberOfElements);
	return numberOfElements - 1;
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}




