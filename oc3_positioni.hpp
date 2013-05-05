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

#ifndef __OPENCAESAR3_POSITION_H_INCLUDED__
#define __OPENCAESAR3_POSITION_H_INCLUDED__

#include <iostream>

#include "oc3_vector2.hpp"

class PointF;

class Point : public Vector2<int>
{
public:
    Point( const int x, const int y ) : Vector2<int>( x, y ) {}
    Point() : Vector2<int>( 0, 0 ) {}

    Point operator+(const Point& other) const { return Point( x + other.x, y + other.y ); }
    Point operator-(const Point& other) const { return Point( x - other.x, y - other.y ); }

    void setX( const int nx ) { x = nx; }
    void setY( const int ny ) { y = ny; }

    PointF toPointF() const; 
};

class PointF : public Vector2<float>
{
public:
    PointF( const float x, const float y ) : Vector2<float>( x, y ) {}
    PointF() : Vector2<float>( 0, 0 ) {}

    PointF operator+(const PointF& other) const { return PointF( x + other.x, y + other.y ); }

    void setX( const float nx ) { x = nx; }
    void setY( const float ny ) { y = ny; }

    Point toPoint() { return Point( (int)x, (int)y ); }
};

inline PointF Point::toPointF() const
{ 
    return PointF( (float)x, (float)y ); 
}

class TilePos : Vector2<int>
{
public:
  TilePos( const int i, const int j ) : Vector2<int>( i, j ) {}
  TilePos() : Vector2<int>( 0, 0 ) {}

  int getI() const { return x; }
  int getJ() const { return y; }
  int getZ() const { return y - x; }

  void setI( const int i ) { x = i; }
  void setJ( const int j ) { y = j; }

  float distanceFrom( const TilePos& other ) { return getDistanceFrom( other );}

  TilePos& operator=(const TilePos& other) { set( other.x, other.y ); return *this; }
  TilePos& operator+=(const TilePos& other) { set( x+other.x, y+other.y ); return *this; }
  TilePos operator+(const TilePos& other) const { return TilePos( x + other.x, y + other.y ); }
  TilePos operator-(const TilePos& other) const { return TilePos( x - other.x, y - other.y ); }
  TilePos operator-() const { return TilePos( -x, -y ); }
  bool operator==(const TilePos& other) const{ return (x == other.x) && ( y == other.y ); }
  bool operator!=(const TilePos& other) const{ return (x != other.x ) || ( y != other.y ); }
private:
  friend std::ostream & operator<<(std::ostream&, const TilePos&);
};

std::ostream & operator << (std::ostream& os, const TilePos& tp)
{
  return os << "(" << tp.getI() << "," << tp.getJ() << ")";
}

#endif // __NRP_POSITION_H_INCLUDED__
