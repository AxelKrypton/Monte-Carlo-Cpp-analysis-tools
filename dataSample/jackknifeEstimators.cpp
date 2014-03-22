#include "jackknifeEstimators.hpp"

//todo: refactor
double JackknifeEstimators::getJackknifeVariance()
{
	return (pow((values - getNthMoment(1)), 2.)).sum() * (numberOfElements  - 1.) / numberOfElements;
}

//todo: remove!
double JackknifeEstimators::getJackknifeVariance_v2()
{
	return ( getNthMoment(2) - pow(getNthMoment(1),2.) ) * (numberOfElements - 1.);
}

double JackknifeEstimators::getJackknifeError()
{
	return sqrt(getJackknifeVariance());
}

void JackknifeEstimators::checkIfJackknifeCanBePerformed()
{
	if(numberOfElements <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}

int JackknifeEstimators::getJackknifeNormalization()
{
	checkIfJackknifeCanBePerformed();
	return numberOfElements - 1;
}

void JackknifeEstimatorsFromBinning::checkIfJackknifeCanBePerformed(int n)
{
	if(n <= 1)
		throw std::invalid_argument("Cannot create jackknifeEstimators from one or less elements!");
}




