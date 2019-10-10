#ifndef ESTIMATEANDERROR_HPP_
#define ESTIMATEANDERROR_HPP_
#include "../types.hpp"

class EstimateAndError
{
public:
	EstimateAndError() :
		estimate(0.), error(0.)
	{}
	EstimateAndError(realFloat mean, realFloat error) :
		estimate(mean), error(error)
	{}

	realFloat estimate;
	realFloat error;
};

#endif