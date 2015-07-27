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
#include <random>
#include <limits>
#include <type_traits>

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
const float RECIPROCAL_2PI	= RECIPROCAL_PI/2.f;
const float SQRT_PI	= sqrt(PI);
const float SQRT_2PI	= sqrt(2*PI);
const float RECIPROCAL_SQRT_2PI	= 1.0f/SQRT_2PI;

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
const double DEGREE360 = 360.0;

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

template<typename T = int>
inline T __random(T max, std::false_type, std::false_type)
{
  static_assert(std::numeric_limits<T>::is_integer, "Integer type required.");
  static std::random_device random_device;
  static std::default_random_engine engine(random_device());
  std::uniform_int_distribution<T> distribution(0, max );
  return distribution(engine);
}

template<typename T>
inline T __random(T max, std::false_type, std::true_type)
{
  static_assert(std::is_enum<T>::value, "Enum type required.");
  return static_cast<T>(__random<typename std::underlying_type<T>::type>(max, std::false_type(), std::false_type()));
}

template<typename T>
inline T __random(T max, std::true_type, std::false_type)
{
  static_assert(std::is_floating_point<T>::value, "Floating point type required.");
  max = (max == 0) ? std::numeric_limits<T>::max() : max;
  static std::random_device random_device;
  static std::default_random_engine engine(random_device());
  std::uniform_real_distribution<T> distribution(0, std::nextafter(max, std::numeric_limits<T>::max()));
  return distribution(engine);
}

// Accepts: integral types, enums, floating point types
template<typename T = int>
inline T random(T max)
{
  return __random(max, typename std::is_floating_point<T>::type(), typename std::is_enum<T>::type());
}

/**
 * Fills array with distinct random values from range min<->max
 */
inline bool random_values_of_range(int arr[], size_t size, int min, int max)
{
  for (size_t i = 0; i < size; ++i)
  {
    bool fail = false;
    int next;
    do
    {
      next = random(max - min) + min;
      for (size_t j = 0; j < i; ++j)
      {
        if (arr[j] == next)
        {
          fail = true;
          break;
        }
      }
    } while (fail);
    arr[i] = next;
  }
  return true;
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
inline T clamp(const T value, const T low, const T high)
{
  const T& mx = math::max<T>( value, low);
  return math::min<T>( mx, high);
}

inline int signnum( float x )
{
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

inline bool probably( float chance )
{
  double u = (float)rand()/(float)RAND_MAX;
  return ( u < chance );
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
