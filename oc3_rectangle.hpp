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

#include "oc3_positioni.hpp"
#include "oc3_size.hpp"

//! Rectangle.
/** Mostly used by 2D GUI elements and for 2D drawing methods.
It has 2 positions instead of position and dimension and a fast
method for collision detection with other rectangles and points.

Coordinates are (0,0) for top-left corner, and increasing to the right
and to the bottom.
*/
class Rect
{
public:

	//! Default constructor creating empty rectangle at (0,0)
	Rect() : UpperLeftCorner(0,0), LowerRightCorner(0,0) {}

	//! Constructor with two corners
	Rect(int x, int y, int x2, int y2)
		: UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}

	//! Constructor with two corners
	Rect(const Point& upperLeft, const Point& lowerRight)
		: UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	//! Constructor with upper left corner and dimension
	Rect(const Point& pos, const Size& size)
		: UpperLeftCorner(pos), LowerRightCorner(pos.getX() + size.getWidth(), pos.getY() + size.getHeight() ) {}

	//! move right by given numbers
	Rect operator+(const Point& pos) const
	{
		Rect ret(*this);
		return ret+=pos;
	}

	//! move right by given numbers
	Rect& operator+=(const Point& pos)
	{
		UpperLeftCorner += pos;
		LowerRightCorner += pos;
		return *this;
	}

	//! move left by given numbers
	Rect operator-(const Point& pos) const
	{
		Rect ret(*this);
		return ret-=pos;
	}

	//! move left by given numbers
	Rect& operator-=(const Point& pos)
	{
		UpperLeftCorner -= pos;
		LowerRightCorner -= pos;
		return *this;
	}

	//! equality operator
	bool operator==(const Rect& other) const
	{
		return (UpperLeftCorner == other.UpperLeftCorner &&
			LowerRightCorner == other.LowerRightCorner);
	}

	//! inequality operator
	bool operator!=(const Rect& other) const
	{
		return (UpperLeftCorner != other.UpperLeftCorner ||
			LowerRightCorner != other.LowerRightCorner);
	}

	//! compares size of rectangles
	bool operator<(const Rect& other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of rectangle
	int getArea() const
	{
		return getWidth() * getHeight();
	}

	//! Returns if a 2d point is within this rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const Point& pos) const
	{
		return (UpperLeftCorner.getX() <= pos.getX() &&
			UpperLeftCorner.getY() <= pos.getY() &&
			LowerRightCorner.getX() >= pos.getX() &&
			LowerRightCorner.getY() >= pos.getY() );
	}

	//! Check if the rectangle collides with another rectangle.
	/** \param other Rectangle to test collision with
	\return True if the rectangles collide. */
	bool isRectCollided(const Rect& other) const
	{
		return (LowerRightCorner.getY() > other.UpperLeftCorner.getY() &&
			UpperLeftCorner.getY() < other.LowerRightCorner.getY() &&
			LowerRightCorner.getX() > other.UpperLeftCorner.getX() &&
			UpperLeftCorner.getX() < other.LowerRightCorner.getX());
	}

	//! Clips this rectangle with another one.
	/** \param other Rectangle to clip with */
	void clipAgainst(const Rect& other)
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
	bool constrainTo(const Rect& other)
	{
		if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
			return false;

		int diff = other.LowerRightCorner.getX() - LowerRightCorner.getX();
		if (diff < 0)
		{
			LowerRightCorner += Point( diff, 0 );
			UpperLeftCorner += Point( diff, 0 );
		}

		diff = other.LowerRightCorner.getY() - LowerRightCorner.getY();
		if (diff < 0)
		{
			LowerRightCorner += Point( 0, diff );
			UpperLeftCorner  += Point( 0, diff );
		}

		diff = UpperLeftCorner.getX() - other.UpperLeftCorner.getX();
		if (diff < 0)
		{
			UpperLeftCorner -= Point( diff, 0 );
			LowerRightCorner -= Point( diff, 0 );
		}

		diff = UpperLeftCorner.getY() - other.UpperLeftCorner.getY();
		if (diff < 0)
		{
			UpperLeftCorner -= Point( diff, 0 );
			LowerRightCorner -= Point( diff, 0 );
		}

		return true;
	}

	//! Get width of rectangle.
	int getWidth() const
	{
		return LowerRightCorner.getX() - UpperLeftCorner.getX();
	}

	//! Get height of rectangle.
	int getHeight() const
	{
		return LowerRightCorner.getY() - UpperLeftCorner.getY();
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (LowerRightCorner.getX() < UpperLeftCorner.getX())
        {
            int tmp = LowerRightCorner.getX();
			LowerRightCorner.setX( UpperLeftCorner.getX() );
            UpperLeftCorner.setX( tmp );
        }

		if (LowerRightCorner.getY() < UpperLeftCorner.getY())
        {
            int tmp = LowerRightCorner.getY();
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
	Point getCenter() const
	{
		return Point(
				(UpperLeftCorner.getX() + LowerRightCorner.getX()) / 2,
				(UpperLeftCorner.getY() + LowerRightCorner.getY()) / 2);
	}

	//! Get the dimensions of the rectangle
	Size getSize() const
	{
		return Size(getWidth(), getHeight());
	}

    Rect RelativeTo( const Rect& other, int limit )
    {
        Rect retRect( *this );
        retRect.UpperLeftCorner -= Point( (retRect.UpperLeftCorner.getX() - other.UpperLeftCorner.getX()) / limit, 0 );
        retRect.UpperLeftCorner -= Point( 0, (retRect.UpperLeftCorner.getY() - other.UpperLeftCorner.getY()) / limit );
        retRect.LowerRightCorner -= Point( (retRect.LowerRightCorner.getX() - other.LowerRightCorner.getX()) / limit, 0 );
        retRect.LowerRightCorner -= Point( 0, (retRect.LowerRightCorner.getY() - other.LowerRightCorner.getY()) / limit );

        return retRect;
    }

    bool IsEqual(const Rect& other, float tolerance) const
    {
         return UpperLeftCorner.IsEqual( other.UpperLeftCorner, tolerance)
                && LowerRightCorner.IsEqual( other.LowerRightCorner, tolerance);
    }

	int getTop() const
	{
		return UpperLeftCorner.getY();
	}

	int getLeft() const
	{
		return UpperLeftCorner.getX();
	}

	int getBottom() const
	{
		return LowerRightCorner.getY();
	}

	int getRight()
	{
		return LowerRightCorner.getX();
	}

	int getRight() const
	{
		return LowerRightCorner.getX();
	}

	//! Upper left corner
	Point UpperLeftCorner;
	//! Lower right corner
	Point LowerRightCorner;
};

#endif //__OPENCAESAR3_RECTANGLE_H_INCLUDED__

