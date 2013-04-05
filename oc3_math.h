#ifndef __OPENCAESAR3_MATH_INCLUDE_
#define __OPENCAESAR3_MATH_INCLUDE_

#include <math.h>
#include <limits.h>
#include <float.h>

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
const float PI		= 3.14159265359f;

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
inline float radToDeg( float radians)
{
	return RADTODEG * radians;
}

template <class T>
inline T clamp (const T& value, const T& low, const T& high)
{
    return (std::min<T>)( (std::max<T>)( value,low ), high );
}

//! Utility function to convert a radian value to degrees
/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
\param radians	The radians value to convert to degrees.
*/
inline double radToDeg64( double radians)
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
inline bool isEqual(const float a, const float b)
{
    return (a + ROUNDING_ERROR_f32 >= b) && (a - ROUNDING_ERROR_f32 <= b);
}

}

#endif
