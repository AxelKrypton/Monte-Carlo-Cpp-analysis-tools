#ifndef ESTIMATEANDERROR_HPP_
#define ESTIMATEANDERROR_HPP_

class EstimateAndError
{
public:
	EstimateAndError() :
		estimate(0.), error(0.)
	{}
	EstimateAndError(double mean, double error) :
		estimate(mean), error(error)
	{}

	double estimate;
	double error;
};

#endif