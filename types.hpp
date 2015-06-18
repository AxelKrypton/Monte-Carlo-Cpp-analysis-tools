#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <boost/multiprecision/mpfr.hpp>

#ifdef _USE_BIG_FLOAT_
	typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<NUM_DECIMAL_DIGITS> > realFloat;
#else
	typedef double realFloat;
#endif

#endif /* TYPES_HPP_ */
