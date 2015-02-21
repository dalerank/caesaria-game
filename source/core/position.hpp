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
  Point( const int x, const int y ) : Vector2<int>( x, y ) {}
  Point() : Vector2<int>( 0, 0 ) {}
  Point( const Vector2<int>& pos ) : Vector2<int>( pos ) {}

  Point operator+(const Point& other) const { return Point( _x + other._x, _y + other._y ); }
  Point operator-(const Point& other) const { return Point( _x - other._x, _y - other._y ); }
  Point operator -() const { return Point( -_x, -_y); }
  Point operator*(float mul) const{ return Point( _x*mul, _y*mul ); }

  void setX( const int nx ) { _x = nx; }
  void setY( const int ny ) { _y = ny; }

  float distanceTo( const Point& other ) const { return sqrtf( pow( float(_x - other._x), 2.f) + pow( float(_y - other._y), 2.f) ); }

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

  Point toPoint() const { return Point( (int)_x, (int)_y ); }
};

inline PointF Point::toPointF() const
{ 
    return PointF( (float)_x, (float)_y );
}

class TilePos : Vector2<int>
{
public:
  TilePos( const int i, const int j ) : Vector2<int>( i, j ) {}
  TilePos( const TilePos& other ) : Vector2<int>( other._x, other._y ) {}
  TilePos() : Vector2<int>( 0, 0 ) {}

  int i() const { return _x; }
  int j() const { return _y; }
  int z() const { return _y - _x; }

  int& ri() { return _x; }
  int& rj() { return _y; }

  inline TilePos northnb() const { return TilePos( _x, _y+1 ); }
  inline TilePos southnb() const { return TilePos( _x, _y-1 ); }
  inline TilePos eastnb() const { return TilePos( _x+1, _y ); }
  inline TilePos westnb() const { return TilePos( _x-1, _y ); }

  void setI( const int i ) { _x = i; }
  void setJ( const int j ) { _y = j; }

  float distanceFrom( const TilePos& other ) const { return getDistanceFrom( other );}
  int getDistanceFromSQ(const TilePos& other) const { return Vector2<int>::getDistanceFromSQ(other);}

  TilePos& operator=(const TilePos& other) { set( other._x, other._y ); return *this; }
  TilePos& operator+=(const TilePos& other) { set( _x + other._x, _y + other._y ); return *this; }
  TilePos operator+(const TilePos& other) const { return TilePos( _x + other._x, _y + other._y ); }
  TilePos operator-(const TilePos& other) const { return TilePos( _x - other._x, _y - other._y ); }
  TilePos operator*(int v) const { return TilePos( _x * v, _y * v ); }
  TilePos operator-() const { return TilePos( -_x, -_y ); }
  bool operator==(const TilePos& other) const{ return (_x == other._x) && ( _y == other._y ); }
  bool operator!=(const TilePos& other) const{ return (_x != other._x ) || ( _y != other._y ); }
  bool operator<(const TilePos& other) const{ return (_x<other._x) || (_x==other._x && _y<other._y); }

  double getAngleICW() const { return getAngle(); }

  TilePos fit( const TilePos& lur, const TilePos& tbr ) const
  {
    TilePos ret = *this;
    ret._x = math::clamp( ret._x, lur._x, tbr._x );
    ret._y = math::clamp( ret._y, lur._y, tbr._y );

    return ret;
  }

  TilePos nextStep( const TilePos& dst ) const
  {
    return *this + TilePos( math::signnum( dst.i() - i() ),
                            math::signnum( dst.j() - j() ) );
  }
};

#endif //__CAESARIA_POSITION_H_INCLUDED__
