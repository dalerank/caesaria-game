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

#ifndef __CAESARIA_SIZE_H_INCLUDED__
#define __CAESARIA_SIZE_H_INCLUDED__

#include "core/vector2.hpp"

class SizeF;

class Size : Vector2<int>
{
public:
    Size( const int w, const int h ) : Vector2<int>( w, h ) {}
    Size() : Vector2<int>( 0, 0 ) {}
    Size( const int s ) : Vector2<int>( s, s ) {}

    Size operator+(const Size& other) const { return Size( _x + other._x, _y + other._y ); }

    inline int width() const { return _x; }
    inline int height() const { return _y; }

    void setWidth( int w ) { _x = w; }
    void setHeight( int h ) { _y = h; }

    SizeF toSizeF() const;

    int area() const { return _x * _y; }

    bool operator==(const Size& other) const{ return (_x == other._x) && ( _y == other._y ); }
    bool operator!=(const Size& other) const{ return (_x != other._x ) || ( _y != other._y ); }
    Size& operator+=(const Size& other) { _x += other._x; _y += other._y; return *this; }
    Size operator-(const Size& other) const { return Size( _x - other._x, _y - other._y ); }
    Size& operator=(const Vector2<int>& s ) { _x = s.x(), _y = s.y(); return *this; }
    Size operator/(float delim) const { return Size( int(_x/delim), int(_y/delim) ); }
    Size operator*(float delim) const { return Size( int(_x*delim), int(_y*delim) ); }
};

class SizeF : Vector2<float>
{
public:
  SizeF( const float w, const float h ) : Vector2<float>( w, h ) {}
  SizeF() : Vector2<float>( 0, 0 ) {}
  SizeF( const float s ) : Vector2<float>( s, s ) {}

  SizeF operator+(const SizeF& other) const { return SizeF( _x + other._x, _y + other._y ); }

  inline float width() const { return _x; }
  inline float height() const { return _y; }

  void setWidth( float w ) { _x = w; }
  void setHeight( float h ) { _y = h; }

  Size toSize() const { return Size( int(_x), int(_y) ); }

  float getArea() const { return _x * _y; }

  bool operator==(const SizeF& other) const { return IsEqual(other, math::ROUNDING_ERROR_f32); }
  bool operator!=(const SizeF& other) const { return !IsEqual(other, math::ROUNDING_ERROR_f32); }
};

inline SizeF Size::toSizeF() const
{
 return SizeF( float(_x), float(_y) ); 
}

#endif//__CAESARIA_SIZE_H_INCLUDED__

