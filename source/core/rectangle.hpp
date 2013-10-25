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


#ifndef __OPENCAESAR3_RECTANGLE_H_INCLUDED__
#define __OPENCAESAR3_RECTANGLE_H_INCLUDED__

#include "position.hpp"
#include "size.hpp"

//! RectT.
/** Mostly used by 2D GUI elements and for 2D drawing methods.
It has 2 positions instead of position and dimension and a fast
method for collision detection with other rectangles and points.

Coordinates are (0,0) for top-left corner, and increasing to the right
and to the bottom.
*/
template< class T >
class RectT
{
public:

	//! Default constructor creating empty rectangle at (0,0)
	RectT() : UpperLeftCorner(0,0), LowerRightCorner(0,0) {}

	//! Constructor with two corners
	RectT(T x, T y, T x2, T y2)
		: UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}

	//! Constructor with two corners
	RectT(const Vector2<T>& upperLeft, const Vector2<T>& lowerRight)
		: UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	//! move right by given numbers
	RectT<T> operator+(const Vector2<T>& pos) const
	{
		RectT<T> ret(*this);
		return ret+=pos;
	}

	//! move right by given numbers
	RectT<T>& operator+=(const Vector2<T>& pos)
	{
		UpperLeftCorner += pos;
		LowerRightCorner += pos;
		return *this;
	}

	//! move left by given numbers
	RectT<T> operator-(const Vector2<T>& pos) const
	{
		RectT<T> ret(*this);
		return ret-=pos;
	}

	//! move left by given numbers
	RectT<T>& operator-=(const Vector2<T>& pos)
	{
		UpperLeftCorner -= pos;
		LowerRightCorner -= pos;
		return *this;
	}

	//! equality operator
	bool operator==(const RectT<T>& other) const
	{
		return (UpperLeftCorner == other.UpperLeftCorner &&
			LowerRightCorner == other.LowerRightCorner);
	}

	//! inequality operator
	bool operator!=(const RectT<T>& other) const
	{
		return (UpperLeftCorner != other.UpperLeftCorner ||
			LowerRightCorner != other.LowerRightCorner);
	}

	//! compares size of rectangles
	bool operator<(const RectT<T>& other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of rectangle
	int getArea() const
	{
		return getWidth() * getHeight();
	}

  //! Adds a point to the rectangle
  /** Causes the rectangle to grow bigger if point is outside of
  the box
  \param p Point to add to the box. */
  void addInternalPoint(const Vector2<T>& p)
  {
    addInternalPoint(p.getX(), p.getY());
  }

  //! Adds a point to the bounding rectangle
  /** Causes the rectangle to grow bigger if point is outside of
  the box
  \param x X-Coordinate of the point to add to this box.
  \param y Y-Coordinate of the point to add to this box. */
  void addInternalPoint(T x, T y)
  {
    if (x>LowerRightCorner.getX() )
      LowerRightCorner.setX( x );
    if (y>LowerRightCorner.getY() )
      LowerRightCorner.setY( y );

    if (x<UpperLeftCorner.getX() )
      UpperLeftCorner.setX( x );
    if (y<UpperLeftCorner.getY())
      UpperLeftCorner.setY( y );
  }

	//! Returns if a 2d point is within this rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const Vector2<T>& pos) const
	{
		return (UpperLeftCorner.getX() <= pos.getX() &&
			UpperLeftCorner.getY() <= pos.getY() &&
			LowerRightCorner.getX() >= pos.getX() &&
			LowerRightCorner.getY() >= pos.getY() );
	}

	//! Check if the rectangle collides with another rectangle.
	/** \param other RectT to test collision with
	\return True if the rectangles collide. */
	bool isRectCollided(const RectT<T>& other) const
	{
		return (LowerRightCorner.getY() > other.UpperLeftCorner.getY() &&
			UpperLeftCorner.getY() < other.LowerRightCorner.getY() &&
			LowerRightCorner.getX() > other.UpperLeftCorner.getX() &&
			UpperLeftCorner.getX() < other.LowerRightCorner.getX());
	}

	//! Clips this rectangle with another one.
	/** \param other RectT to clip with */
	void clipAgainst(const RectT<T>& other)
	{
		if (other.LowerRightCorner.getX() < LowerRightCorner.getX())
			LowerRightCorner.setX( other.LowerRightCorner.getX() );

        if (other.LowerRightCorner.getY() < LowerRightCorner.getY())
			LowerRightCorner.setY( other.LowerRightCorner.getY() );

		if (other.UpperLeftCorner.getX() > UpperLeftCorner.getX())
			UpperLeftCorner.setX( other.UpperLeftCorner.getX() );

        if (other.UpperLeftCorner.getY() > UpperLeftCorner.getY())
			UpperLeftCorner.setY( other.UpperLeftCorner.getY() );

		// correct possible invalid rect resulting from clipping
		if (UpperLeftCorner.getY() > LowerRightCorner.getY())
			UpperLeftCorner.setY( LowerRightCorner.getY() );
		
        if (UpperLeftCorner.getX() > LowerRightCorner.getX())
			UpperLeftCorner.setX( LowerRightCorner.getX() );
	}

	//! Moves this rectangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const RectT<T>& other)
	{
		if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
			return false;

		int diff = other.LowerRightCorner.getX() - LowerRightCorner.getX();
		if (diff < 0)
		{
			LowerRightCorner += Vector2<T>( diff, 0 );
			UpperLeftCorner += Vector2<T>( diff, 0 );
		}

		diff = other.LowerRightCorner.getY() - LowerRightCorner.getY();
		if (diff < 0)
		{
			LowerRightCorner += Vector2<T>( 0, diff );
			UpperLeftCorner  += Vector2<T>( 0, diff );
		}

		diff = UpperLeftCorner.getX() - other.UpperLeftCorner.getX();
		if (diff < 0)
		{
			UpperLeftCorner -= Vector2<T>( diff, 0 );
			LowerRightCorner -= Vector2<T>( diff, 0 );
		}

		diff = UpperLeftCorner.getY() - other.UpperLeftCorner.getY();
		if (diff < 0)
		{
			UpperLeftCorner -= Vector2<T>( diff, 0 );
			LowerRightCorner -= Vector2<T>( diff, 0 );
		}

		return true;
	}

	//! Get width of rectangle.
	T getWidth() const
	{
		return LowerRightCorner.getX() - UpperLeftCorner.getX();
	}

	//! Get height of rectangle.
	T getHeight() const
	{
		return LowerRightCorner.getY() - UpperLeftCorner.getY();
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (LowerRightCorner.getX() < UpperLeftCorner.getX())
        {
            T tmp = LowerRightCorner.getX();
			LowerRightCorner.setX( UpperLeftCorner.getX() );
            UpperLeftCorner.setX( tmp );
        }

		if (LowerRightCorner.getY() < UpperLeftCorner.getY())
        {
            T tmp = LowerRightCorner.getY();
			LowerRightCorner.setY( UpperLeftCorner.getY() );
            UpperLeftCorner.setY( tmp );
        }
	}

	//! Returns if the rect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return ((LowerRightCorner.getX() >= UpperLeftCorner.getX()) &&
			(LowerRightCorner.getY() >= UpperLeftCorner.getY()));
	}

    //! Get the center of the rectangle
	Vector2<T> getCenter() const
	{
		return Vector2<T>(
				(UpperLeftCorner.getX() + LowerRightCorner.getX()) / 2,
				(UpperLeftCorner.getY() + LowerRightCorner.getY()) / 2);
	}

  RectT<T> relativeTo( const RectT<T>& other, T limit )
  {
    RectT<T> retRect( *this );
    retRect.UpperLeftCorner -= Vector2<T>( (retRect.UpperLeftCorner.getX() - other.UpperLeftCorner.getX()) / limit, 0 );
    retRect.UpperLeftCorner -= Vector2<T>( 0, (retRect.UpperLeftCorner.getY() - other.UpperLeftCorner.getY()) / limit );
    retRect.LowerRightCorner -= Vector2<T>( (retRect.LowerRightCorner.getX() - other.LowerRightCorner.getX()) / limit, 0 );
    retRect.LowerRightCorner -= Vector2<T>( 0, (retRect.LowerRightCorner.getY() - other.LowerRightCorner.getY()) / limit );

    return retRect;
  }

  bool IsEqual(const RectT<T>& other, float tolerance) const
  {
    return UpperLeftCorner.IsEqual( other.UpperLeftCorner, tolerance)
           && LowerRightCorner.IsEqual( other.LowerRightCorner, tolerance);
  }

	T getTop() const
	{
		return UpperLeftCorner.getY();
	}

	T getLeft() const
	{
		return UpperLeftCorner.getX();
	}

	T getBottom() const
	{
		return LowerRightCorner.getY();
	}

	T getRight()
	{
		return LowerRightCorner.getX();
	}

	T getRight() const
	{
		return LowerRightCorner.getX();
	}

	//! Upper left corner
	Vector2<T> UpperLeftCorner;
	//! Lower right corner
	Vector2<T> LowerRightCorner;
};

class RectF;

class Rect : public RectT<int>
{
public:

  Rect() : RectT<int>( 0, 0, 0, 0 ) {}
  //! Constructor with upper left corner and dimension
  Rect(const Point& pos, const Size& size)
    : RectT<int>( pos, Point( pos.getX() + size.getWidth(), pos.getY() + size.getHeight() ) ) {}

  Rect( int x1, int y1, int x2, int y2 )
    : RectT<int>( x1, y1, x2, y2 ) {}

  Rect operator+(const Point& offset ) const
  {
    return Rect( UpperLeftCorner + offset, LowerRightCorner + offset );
  }

  Rect operator-(const Point& offset ) const
  {
    return Rect( UpperLeftCorner - offset, LowerRightCorner - offset );
  }
  
  //! Get the dimensions of the rectangle
  Size getSize() const
  {
    return Size(getWidth(), getHeight());
  }

  Rect( const Point& p1, const Point& p2 ) 
    : RectT<int>( p1, p2 ) {}

  RectF toRectF() const;
};

class RectF : public RectT<float>
{
public:
  RectF() : RectT<float>( 0, 0, 0, 0 ) {}

  RectF( float x1, float y1, float x2, float y2 )
    : RectT<float>( x1, y1, x2, y2 ) {}

  RectF(const PointF& pos, const SizeF& size)
    : RectT<float>( pos, PointF( pos.getX() + size.getWidth(), pos.getY() + size.getHeight() ) ) {}

  RectF( const PointF& p1, const PointF& p2 )
    : RectT<float>( p1, p2 ) {}

  RectF& operator=( const RectT<float>& other )
  {
    UpperLeftCorner = other.UpperLeftCorner;
    LowerRightCorner = other.LowerRightCorner;

    return *this;
  }

  //! Get the dimensions of the rectangle
  SizeF getSize() const
  {
    return SizeF(getWidth(), getHeight());
  }

  RectF& operator=( const RectF& other )
  {
    UpperLeftCorner = other.UpperLeftCorner;
    LowerRightCorner = other.LowerRightCorner;

    return *this;
  }

  Rect toRect() const 
  { 
    return Rect( UpperLeftCorner.As<int>(), LowerRightCorner.As<int>() );
  }
};

inline RectF Rect::toRectF() const
{
  return RectF( UpperLeftCorner.As<float>(), LowerRightCorner.As<float>() );
}


#endif //__OPENCAESAR3_RECTANGLE_H_INCLUDED__

