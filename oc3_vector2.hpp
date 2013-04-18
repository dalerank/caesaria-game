// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_VECTOR2_INCLUDED__
#define __OPENCAESAR3_VECTOR2_INCLUDED__

#include "oc3_math.hpp"

template <class T>
class Vector2
{
public:
	//! Default constructor (null vector)
	Vector2() : x(0), y(0) {}
	//! Constructor with two different values
	Vector2(T nx, T ny) : x(nx), y(ny) {}
	//! Constructor with the same value for both members
	explicit Vector2(T n) : x(n), y(n) {}
	//! Copy constructor
	Vector2(const Vector2<T>& other) : x(other.x), y(other.y) {}

	// operators
	Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

	Vector2<T>& operator=(const Vector2<T>& other) { x = other.x; y = other.y; return *this; }

	Vector2<T> operator+(const Vector2<T>& other) const { return Vector2<T>(x + other.x, y + other.y); }
	Vector2<T>& operator+=(const Vector2<T>& other) { x+=other.x; y+=other.y; return *this; }
	Vector2<T> operator+(const T v) const { return Vector2<T>(x + v, y + v); }
	Vector2<T>& operator+=(const T v) { x+=v; y+=v; return *this; }

	Vector2<T> operator-(const Vector2<T>& other) const { return Vector2<T>(x - other.x, y - other.y); }
	Vector2<T>& operator-=(const Vector2<T>& other) { x-=other.x; y-=other.y; return *this; }
	Vector2<T> operator-(const T v) const { return Vector2<T>(x - v, y - v); }
	Vector2<T>& operator-=(const T v) { x-=v; y-=v; return *this; }

	Vector2<T> operator*(const Vector2<T>& other) const { return Vector2<T>(x * other.x, y * other.y); }
	Vector2<T>& operator*=(const Vector2<T>& other) { x*=other.x; y*=other.y; return *this; }
	Vector2<T> operator*(const T v) const { return Vector2<T>(x * v, y * v); }
	Vector2<T>& operator*=(const T v) { x*=v; y*=v; return *this; }

	Vector2<T> operator/(const Vector2<T>& other) const { return Vector2<T>(x / other.x, y / other.y); }
	Vector2<T>& operator/=(const Vector2<T>& other) { x/=other.x; y/=other.y; return *this; }
	Vector2<T> operator/(const T v) const { return Vector2<T>(x / v, y / v); }
	Vector2<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

	//! sort in order X, Y. Equality with rounding tolerance.
    bool operator<=(const Vector2<T>& other) const
	{
        return 	(x<other.x || math::isEqual(x, other.x)) ||
                (math::isEqual(x, other.x) && (y<other.y || math::isEqual(y, other.y)));
	}

	//! sort in order X, Y. Equality with rounding tolerance.
	bool operator>=(const Vector2<T>&other) const
	{
                return 	(x>other.x || math::isEqual(x, other.x)) ||
                                (math::isEqual(x, other.x) && (y>other.Y || math::isEqual(y, other.y)));
	}

	//! sort in order X, Y. Difference must be above rounding tolerance.
	bool operator<(const Vector2<T>&other) const
	{
                return 	(x<other.x && !math::isEqual(x, other.x)) ||
                                (math::isEqual(x, other.X) && y<other.Y && !math::isEqual(y, other.y));
	}

	//! sort in order X, Y. Difference must be above rounding tolerance.
	bool operator>(const Vector2<T>&other) const
	{
        return 	(x>other.x && !math::isEqual(x, other.x)) ||
                                (math::isEqual(x, other.x) && y>other.y && !math::isEqual(y, other.y));
	}

    bool operator==(const Vector2<T>& other) const { return IsEqual(other, math::ROUNDING_ERROR_f32); }
    bool operator!=(const Vector2<T>& other) const { return !IsEqual(other, math::ROUNDING_ERROR_f32); }

	// functions

    //! Checks if this vector equals the other one.
	/** Takes floating point rounding errors into account.
	\param other Vector to compare with.
	\return True if the two vector are (almost) equal, else false. */
    bool IsEqual(const Vector2<T>& other, float tolerance) const
	{
        return math::isEqual(x, other.x) && math::isEqual(y, other.y);
	}

	Vector2<T>& set(T nx, T ny) {x=nx; y=ny; return *this; }
	Vector2<T>& set(const Vector2<T>& p) { x=p.x; y=p.y; return *this; }

	//! Gets the length of the vector.
	/** \return The length of the vector. */
    float getLength() const { return sqrt( (float)x*(float)x + (float)y*(float)y ); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	T getLengthSQ() const { return x*x + y*y; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const Vector2<T>& other) const
	{
		return x*other.x + y*other.y;
	}

    template< class A >
    Vector2<A> As()
    {
        return Vector2<A>( (A)x, (A)y );
    }

    template< class A >
    Vector2<A> As() const
    {
        return Vector2<A>( (A)x, (A)y );
    }

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	float getDistanceFrom(const Vector2<T>& other) const
	{
		return Vector2<T>(x - other.x, y - other.y).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const Vector2<T>& other) const
	{
		return Vector2<T>(x - other.x, y - other.y).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
	Vector2<T>& rotateBy(float degrees, const Vector2<T>& center=Vector2<T>())
	{
        degrees *= math::DEGTORAD64;
		const float cs = cos(degrees);
		const float sn = sin(degrees);

		x -= center.X;
		y -= center.Y;

		set((T)(x*cs - y*sn), (T)(x*sn + y*cs));

		x += center.X;
		y += center.Y;
		return *this;
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	Vector2<T>& normalize()
	{
		float length = (float)(x*x + y*y);
        
        if (math::isEqual(length, 0.f))
			return *this;
        length = 1.f / sqrt( length );
		x = (T)(x * length);
		y = (T)(y * length);
		return *this;
	}

	//! Calculates the angle of this vector in degrees in the trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase counter-clockwise.
	This method has been suggested by Pr3t3nd3r.
	\return Returns a value between 0 and 360. */
	float getAngleTrig() const
	{
		if (y == 0)
			return x < 0 ? 180 : 0;
		else
		if (x == 0)
			return y < 0 ? 270 : 90;

		if ( y > 0)
			if (x > 0)
                                return atanf((float)y/(float)x) * math::RADTODEG;
			else
                                return 180.0-atanf((float)y/-(float)x) * math::RADTODEG;
		else
			if (x > 0)
                                return 360.0-atanf(-(float)y/(float)x) * math::RADTODEG;
			else
                                return 180.0+atanf(-(float)y/-(float)x) * math::RADTODEG;
	}

	//! Calculates the angle of this vector in degrees in the counter trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase clockwise.
	\return Returns a value between 0 and 360. */
	inline float getAngle() const
	{
		if (y == 0) // corrected thanks to a suggestion by Jox
			return x < 0 ? 180 : 0;
		else if (x == 0)
			return y < 0 ? 90 : 270;

		// don't use getLength here to avoid precision loss with s32 vectors
                float tmp = y /  sqrt((float)(x*x + y*y));
                tmp = atanf( sqrt(1 - tmp*tmp) / tmp) * math::RADTODEG64;

		if (x>0 && y>0)
			return tmp + 270;
		else
		if (x>0 && y<0)
			return tmp + 90;
		else
		if (x<0 && y<0)
			return 90 - tmp;
		else
		if (x<0 && y>0)
			return 270 - tmp;

		return tmp;
	}

	//! Calculates the angle between this vector and another one in degree.
	/** \param b Other vector to test with.
	\return Returns a value between 0 and 90. */
	inline float getAngleWith(const Vector2<T>& b) const
	{
		double tmp = x*b.x + y*b.y;

		if (tmp == 0.0)
			return 90.0;

                tmp = tmp / sqrtf((float)((x*x + y*y) * (b.x*b.x + b.y*b.y)));
		if (tmp < 0.0)
			tmp = -tmp;

                return atanf(sqrtf(1 - tmp*tmp) / tmp) * math::RADTODEG;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const Vector2<T>& begin, const Vector2<T>& end) const
	{
		if (begin.x != end.x)
		{
			return ((begin.x <= x && x <= end.x) ||
				(begin.x >= x && x >= end.x));
		}
		else
		{
			return ((begin.y <= y && y <= end.y) ||
				(begin.y >= y && y >= end.y));
		}
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	Vector2<T> getInterpolated(const Vector2<T>& other, T d) const
	{
		float inv = 1.0f - (float)d;
		return Vector2<T>((T)(other.x*inv + x*d), (T)(other.y*inv + y*d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	Vector2<T> getInterpolated_quadratic(const Vector2<T>& v2, const Vector2<T>& v3, const T d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const float inv = 1.0f - d;
		const float mul0 = inv * inv;
		const float mul1 = 2.0f * d * inv;
		const float mul2 = d * d;

		return Vector2<T> ( (T)(x * mul0 + v2.X * mul1 + v3.X * mul2),
					(T)(y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	Vector2<T>& interpolate(const Vector2<T>& a, const Vector2<T>& b, const T d)
	{
		x = (T)((float)b.x + ( ( a.x - b.x ) * d ));
		y = (T)((float)b.y + ( ( a.y - b.y ) * d ));
		return *this;
	}

    T getX() const { return x; }
    T getY() const { return y; }

protected:
	//! X coordinate of vector.
	T x;

	//! Y coordinate of vector.
	T y;
};

#endif
