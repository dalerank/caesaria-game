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


#ifndef __OPENCAESAR3_SIZE_H_INCLUDED__
#define __OPENCAESAR3_SIZE_H_INCLUDED__

#include "oc3_vector2.hpp"

class SizeF;

class Size : Vector2<int>
{
public:
    Size( const int w, const int h ) : Vector2<int>( w, h ) {}
    Size() : Vector2<int>( 0, 0 ) {}
    Size( const int s ) : Vector2<int>( s, s ) {}

    Size operator+(const Size& other) const { return Size( x + other.x, y + other.y ); }

    int getWidth() const { return x; }
    int getHeight() const { return y; }

    void setWidth( int w ) { x = w; }
    void setHeight( int h ) { y = h; }

    SizeF toSizeF() const;

    int getArea() const { return x * y; }

    bool operator==(const Size& other) const{ return (x == other.x) && ( y == other.y ); }
    bool operator!=(const Size& other) const{ return (x != other.x ) || ( y != other.y ); }
    Size& operator+=(const Size& other) { x += other.x; y += other.y; return *this; }
    Size& operator=(const Vector2<int>& s ) { x = s.getX(), y = s.getY(); return *this; }
};

class SizeF : Vector2<float>
{
public:
  SizeF( const float w, const float h ) : Vector2<float>( w, h ) {}
  SizeF() : Vector2<float>( 0, 0 ) {}
  SizeF( const float s ) : Vector2<float>( s, s ) {}

  SizeF operator+(const SizeF& other) const { return SizeF( x + other.x, y + other.y ); }

  float getWidth() const { return x; }
  float getHeight() const { return y; }

  void setWidth( float w ) { x = w; }
  void setHeight( float h ) { y = h; }

  Size toSize() const { return Size( int(x), int(y) ); }

  float getArea() const { return x * y; }

  bool operator==(const SizeF& other) const { return IsEqual(other, math::ROUNDING_ERROR_f32); }
  bool operator!=(const SizeF& other) const { return !IsEqual(other, math::ROUNDING_ERROR_f32); }
};

inline SizeF Size::toSizeF() const
{
 return SizeF( float(x), float(y) ); 
}

#endif

