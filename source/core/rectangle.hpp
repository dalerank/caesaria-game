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

#ifndef __CAESARIA_RECTANGLE_H_INCLUDED__
#define __CAESARIA_RECTANGLE_H_INCLUDED__

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
  RectT() : _lefttop(0,0), _bottomright(0,0) {}

  //! Constructor with two corners
  RectT(T x, T y, T x2, T y2)
          : _lefttop(x,y), _bottomright(x2,y2) {}

  //! Constructor with two corners
  RectT(const Vector2<T>& upperLeft, const Vector2<T>& lowerRight)
          : _lefttop(upperLeft), _bottomright(lowerRight) {}

  //! move right by given numbers
  RectT<T> operator+(const Vector2<T>& pos) const
  {
          RectT<T> ret(*this);
          return ret+=pos;
  }

  //! move right by given numbers
  RectT<T>& operator+=(const Vector2<T>& pos)
  {
          _lefttop += pos;
          _bottomright += pos;
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
          _lefttop -= pos;
          _bottomright -= pos;
          return *this;
  }

  //! equality operator
  bool operator==(const RectT<T>& other) const
  {
          return (_lefttop == other._lefttop &&
                  _bottomright == other._bottomright);
  }

  //! inequality operator
  bool operator!=(const RectT<T>& other) const
  {
          return (_lefttop != other._lefttop ||
                  _bottomright != other._bottomright);
  }

  //! compares size of rectangles
  bool operator<(const RectT<T>& other) const
  {
          return getArea() < other.getArea();
  }

  //! Returns size of rectangle
  int getArea() const
  {
          return width() * height();
  }

  //! Adds a point to the rectangle
  /** Causes the rectangle to grow bigger if point is outside of
  the box
  \param p Point to add to the box. */
  void addInternalPoint(const Vector2<T>& p)
  {
    addInternalPoint(p.x(), p.y());
  }

  //! Adds a point to the bounding rectangle
  /** Causes the rectangle to grow bigger if point is outside of
  the box
  \param x X-Coordinate of the point to add to this box.
  \param y Y-Coordinate of the point to add to this box. */
  void addInternalPoint(T x, T y)
  {
    if (x>_bottomright.x() )
      _bottomright.setX( x );
    if (y>_bottomright.y() )
      _bottomright.setY( y );

    if (x<_lefttop.x() )
      _lefttop.setX( x );
    if (y<_lefttop.y())
      _lefttop.setY( y );
  }

  //! Returns if a 2d point is within this rectangle.
  /** \param pos Position to test if it lies within this rectangle.
  \return True if the position is within the rectangle, false if not. */
  bool isPointInside(const Vector2<T>& pos) const
  {
          return (_lefttop.x() <= pos.x() &&
                  _lefttop.y() <= pos.y() &&
                  _bottomright.x() >= pos.x() &&
                  _bottomright.y() >= pos.y() );
  }

  //! Check if the rectangle collides with another rectangle.
  /** \param other RectT to test collision with
  \return True if the rectangles collide. */
  bool isRectCollided(const RectT<T>& other) const
  {
          return (_bottomright.y() > other._lefttop.y() &&
                  _lefttop.y() < other._bottomright.y() &&
                  _bottomright.x() > other._lefttop.x() &&
                  _lefttop.x() < other._bottomright.x());
  }

  //! Clips this rectangle with another one.
  /** \param other RectT to clip with */
  void clipAgainst(const RectT<T>& other)
  {
    if (other._bottomright.x() < _bottomright.x())
      _bottomright.setX( other._bottomright.x() );

        if (other._bottomright.y() < _bottomright.y())
      _bottomright.setY( other._bottomright.y() );

    if (other._lefttop.x() > _lefttop.x())
      _lefttop.setX( other._lefttop.x() );

        if (other._lefttop.y() > _lefttop.y())
      _lefttop.setY( other._lefttop.y() );

		// correct possible invalid rect resulting from clipping
    if (_lefttop.y() > _bottomright.y())
      _lefttop.setY( _bottomright.y() );
		
        if (_lefttop.x() > _bottomright.x())
      _lefttop.setX( _bottomright.x() );
	}

	//! Moves this rectangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const RectT<T>& other)
	{
		if (other.width() < width() || other.height() < height())
			return false;

    int diff = other._bottomright.x() - _bottomright.x();
		if (diff < 0)
		{
      _bottomright += Vector2<T>( diff, 0 );
      _lefttop += Vector2<T>( diff, 0 );
		}

    diff = other._bottomright.y() - _bottomright.y();
		if (diff < 0)
		{
      _bottomright += Vector2<T>( 0, diff );
      _lefttop  += Vector2<T>( 0, diff );
		}

    diff = _lefttop.x() - other._lefttop.x();
		if (diff < 0)
		{
      _lefttop -= Vector2<T>( diff, 0 );
      _bottomright -= Vector2<T>( diff, 0 );
		}

    diff = _lefttop.y() - other._lefttop.y();
		if (diff < 0)
		{
      _lefttop -= Vector2<T>( diff, 0 );
      _bottomright -= Vector2<T>( diff, 0 );
		}

		return true;
	}

	//! Get width of rectangle.
	T width() const
	{
    return _bottomright.x() - _lefttop.x();
	}

	//! Get height of rectangle.
	T height() const
	{
    return _bottomright.y() - _lefttop.y();
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
    if (_bottomright.x() < _lefttop.x())
		{
      T tmp = _bottomright.x();
      _bottomright.setX( _lefttop.x() );
      _lefttop.setX( tmp );
		}

    if (_bottomright.y() < _lefttop.y())
		{
      T tmp = _bottomright.y();
      _bottomright.setY( _lefttop.y() );
      _lefttop.setY( tmp );
		}
	}

	//! Returns if the rect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
    return ((_bottomright.x() >= _lefttop.x()) &&
      (_bottomright.y() >= _lefttop.y()));
	}

    //! Get the center of the rectangle
  Vector2<T> center() const
	{
		return Vector2<T>(
        (_lefttop.x() + _bottomright.x()) / 2,
        (_lefttop.y() + _bottomright.y()) / 2);
	}

  RectT<T> relativeTo( const RectT<T>& other, T limit )
  {
    RectT<T> retRect( *this );
    retRect._lefttop -= Vector2<T>( (retRect._lefttop.x() - other._lefttop.x()) / limit, 0 );
    retRect._lefttop -= Vector2<T>( 0, (retRect._lefttop.y() - other._lefttop.y()) / limit );
    retRect._bottomright -= Vector2<T>( (retRect._bottomright.x() - other._bottomright.x()) / limit, 0 );
    retRect._bottomright -= Vector2<T>( 0, (retRect._bottomright.y() - other._bottomright.y()) / limit );

    return retRect;
  }

  bool IsEqual(const RectT<T>& other, float tolerance) const
  {
    return _lefttop.IsEqual( other._lefttop, tolerance)
           && _bottomright.IsEqual( other._bottomright, tolerance);
  }

  Vector2<T> righttop() const { return Vector2<T>( _bottomright.x(), _lefttop.y() ); }
  Vector2<T> leftbottom() const { return Vector2<T>( _lefttop.x(), _bottomright.y() ); }
  const Vector2<T>& lefttop() const { return _lefttop; }
  const Vector2<T>& rightbottom() const { return _bottomright; }

  inline T top() const 	{		return _lefttop.y();   }
  inline T& rtop()      {		return _lefttop.ry();	}
  inline T left() const	{		return _lefttop.x();   }
  inline T& rleft()     {		return _lefttop.rx();	}
  inline T bottom()const{		return _bottomright.y();	}
  inline T& rbottom()   {   return _bottomright.ry(); }
  inline T right() const{		return _bottomright.x();	}
  inline T& rright()    {   return _bottomright.rx(); }
  inline void setLeft( T value ) { _lefttop.setX( value ); }
  inline void setTop( T value ) { _lefttop.setY( value ); }
  inline void setRight( T value ) { _bottomright.setX( value ); }
  inline void setBottom( T value ) { _bottomright.setY( value ); }

	//! Upper left corner
  Vector2<T> _lefttop;
	//! Lower right corner
  Vector2<T> _bottomright;
};

class RectF;

class Rect : public RectT<int>
{
public:

  Rect() : RectT<int>( 0, 0, 0, 0 ) {}
  //! Constructor with upper left corner and dimension
  Rect(const Point& pos, const Size& size)
    : RectT<int>( pos, Point( pos.x() + size.width(), pos.y() + size.height() ) ) {}

  Rect( int x1, int y1, int x2, int y2 )
    : RectT<int>( x1, y1, x2, y2 ) {}

  Rect operator+(const Point& offset ) const
  {
    return Rect( _lefttop + offset, _bottomright + offset );
  }

  Rect operator-(const Point& offset ) const
  {
    return Rect( _lefttop - offset, _bottomright - offset );
  }
  
  //! Get the dimensions of the rectangle
  Size size() const
  {
    return Size(width(), height());
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
    : RectT<float>( pos, PointF( pos.x() + size.width(), pos.y() + size.height() ) ) {}

  RectF( const PointF& p1, const PointF& p2 )
    : RectT<float>( p1, p2 ) {}

  RectF& operator=( const RectT<float>& other )
  {
    _lefttop = other._lefttop;
    _bottomright = other._bottomright;

    return *this;
  }

  //! Get the dimensions of the rectangle
  SizeF getSize() const
  {
    return SizeF(width(), height());
  }

  RectF& operator=( const RectF& other )
  {
    _lefttop = other._lefttop;
    _bottomright = other._bottomright;

    return *this;
  }

  Rect toRect() const 
  { 
    return Rect( _lefttop.As<int>(), _bottomright.As<int>() );
  }
};

inline RectF Rect::toRectF() const
{
  return RectF( _lefttop.As<float>(), _bottomright.As<float>() );
}


#endif //__CAESARIA_RECTANGLE_H_INCLUDED__

