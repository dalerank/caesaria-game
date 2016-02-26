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

#ifndef __CAESARIA_POSITION_H_INCLUDED__
#define __CAESARIA_POSITION_H_INCLUDED__

#include <iostream>
#include "vector2.hpp"

class PointF;

class Point : public Vector2<int>
{
public:
  Point(const int x, const int y) : Vector2<int>( x, y ) {}
  Point() : Vector2<int>( 0, 0 ) {}
  Point(const Vector2<int>& pos) : Vector2<int>( pos ) {}

  Point operator+(const Point& other) const { return Point(_x + other._x, _y + other._y); }
  Point operator-(const Point& other) const { return Point(_x - other._x, _y - other._y); }
  Point operator-() const { return Point( -_x, -_y); }
  Point operator*(float mul) const{ return Point( (int)(_x*mul), (int)(_y*mul) ); }
  Point operator/(float div) const{ return Point( (int)(_x/div), (int)(_y/div) ); }
  static Point polar(int rad, float radian) { return Point( (int)(rad * sin(radian)), (int)(rad * cos(radian)) ); }

  void setX(const int nx) { _x = nx; }
  void setY(const int ny) { _y = ny; }

  float distanceTo(const Point& other) const { return sqrtf( pow( float(_x - other._x), 2.f) + pow( float(_y - other._y), 2.f) ); }

  PointF toPointF() const;
};

class PointF : public Vector2<float>
{
public:
  PointF( const float x, const float y ) : Vector2<float>( x, y ) {}
  PointF() : Vector2<float>( 0, 0 ) {}
  PointF( const Vector2<float>& pos ) : Vector2<float>( pos ) {}

  PointF operator+(const PointF& other) const { return PointF( _x + other._x, _y + other._y ); }

  void setX( const float nx ) { _x = nx; }
  void setY( const float ny ) { _y = ny; }
  PointF mul( float xm, float ym ) { return PointF( _x * xm, _y * ym ); }

  Point toPoint() const { return Point( (int)_x, (int)_y ); }
};

inline PointF Point::toPointF() const
{ 
  return PointF( (float)_x, (float)_y );
}

#endif //__CAESARIA_POSITION_H_INCLUDED__
