// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_MATH_INCLUDE_
#define __CAESARIA_MATH_INCLUDE_

#include <cmath>
#include <algorithm>
#include <climits>
#include <cfloat>
#include "platform.hpp"
#ifdef CAESARIA_PLATFORM_MACOSX
  #include <stdlib.h>
#endif
namespace math
{

const int ROUNDING_ERROR_S32 = 0;
const float ROUNDING_ERROR_f32 = 0.000001f;
const double ROUNDING_ERROR_f64 = 0.00000001;
const float FI = 1.618f;

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
//! Constant for PI.
const float PI = 3.14159265359f;

//! Constant for reciprocal of PI.
const float RECIPROCAL_PI	= 1.0f/PI;

//! Constant for half of PI.
const float HALF_PI	= PI/2.0f;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
//! Constant for 64bit PI.
const double PI64		= 3.1415926535897932384626433832795028841971693993751;

//! Constant for 64bit reciprocal of PI.
const double RECIPROCAL_PI64 = 1.0/PI64;

//! 32bit Constant for converting from degrees to radians
const float DEGTORAD = PI / 180.0f;

//! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
const float RADTODEG   = 180.0f / PI;

//! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
const double DEGTORAD64 = PI64 / 180.0;

//! 64bit constant for converting from radians to degrees
const double RADTODEG64 = 180.0 / PI64;

//! Utility function to convert a radian value to degrees
/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
\param radians	The radians value to convert to degrees.
*/
inline float rad2Degf( float radians)
{
	return RADTODEG * radians;
}

inline int percentage( int value, int base )
{
	return base > 0 ? (value * 100 / base) : 0;
}

inline int random( int max )
{
  max = ( max == 0 ) ? 0xffffffff : max;
#ifdef CAESARIA_PLATFORM_MACOSX
  return rand() % max;
#else
  return std::rand() % max;
#endif
}

#ifdef _MSC_VER
#undef max
#undef min
#endif

template <class T>
inline const T& max( const T& a, const T& b) { return a > b ? a : b; }

template <class T>
inline const T& min( const T& a, const T& b) { return a < b ? a : b; }

template< class T >
inline T clamp(const T& value, const T& low, const T& high)
{
  const T& mx = math::max<T>( value, low);
  return math::min<T>( mx, high);
}

inline int signnum( float x )
{
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

//! Utility function to convert a radian value to degrees
/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
\param radians	The radians value to convert to degrees.
*/
inline double rad2Degaq( double radians)
{
	return RADTODEG64 * radians;
}

//! Utility function to convert a degrees value to radians
/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
\param degrees	The degrees value to convert to radians.
*/
inline float degToRad( float degrees)
{
	return DEGTORAD * degrees;
}

//! Utility function to convert a degrees value to radians
/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
\param degrees	The degrees value to convert to radians.
*/
inline double degToRad64( double degrees)
{
	return DEGTORAD64 * degrees;
}

//! returns if a equals b, taking possible rounding errors into account
template<class T>
inline bool isEqual(const T a, const T b)
{
    return (a + ROUNDING_ERROR_f32 >= b) && (a - ROUNDING_ERROR_f32 <= b);
}

template<class T>
inline bool isEqual(const T a, const T b, const T tolerance)
{
    return (a + tolerance >= b) && (a - tolerance <= b);
}

}

#endif //__CAESARIA_MATH_INCLUDE_
