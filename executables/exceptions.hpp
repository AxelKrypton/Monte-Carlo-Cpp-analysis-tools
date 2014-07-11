#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>

class wrongBinningParameter : public std::invalid_argument {
public:
	wrongBinningParameter() :
		std::invalid_argument("Binning parameters do not match number of data points. Aborting!")
	{}
};

#endif /* EXCEPTIONS_HPP_ */
