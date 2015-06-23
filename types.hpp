#ifndef TYPES_HPP_
#define TYPES_HPP_

/*
 * NOTE: In principle we would like to use the boost::multiprecision::number objects with expression templates switched on.
 *       Nevertheless, this breaks down in compilation all the tests, since the floating point comparison header file seems
 *       to contain a bug:
 *       https://svn.boost.org/trac/boost/ticket/11054
 *
 *       For the moment we switch the expression templates of everywhere.
 */


#ifdef _USE_BIG_FLOAT_
	#include <boost/multiprecision/mpfr.hpp> //this include is here, otherwise linking to mpfr is needed where mpfr is not actually used
	typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<NUM_DECIMAL_DIGITS>, boost::multiprecision::et_off > realFloat;
#else
	typedef double realFloat;
#endif

#endif /* TYPES_HPP_ */
