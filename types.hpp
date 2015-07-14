#ifndef TYPES_HPP_
#define TYPES_HPP_

#ifdef _USE_BIG_FLOAT_
	#include <boost/multiprecision/mpfr.hpp> //this include is here, otherwise linking to mpfr is needed where mpfr is not actually used
	typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<NUM_DECIMAL_DIGITS>, boost::multiprecision::et_on > realFloat;
#else
	typedef double realFloat;
#endif

#endif /* TYPES_HPP_ */
