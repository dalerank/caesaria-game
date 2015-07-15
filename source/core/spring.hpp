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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com


#ifndef __CAESARIA_MATH_SPRING_H_INCLUDE__
#define __CAESARIA_MATH_SPRING_H_INCLUDE__

#include "color.hpp"

namespace math
{

template<class T>
class Spring
{
public:
  Spring() : _signdelta(1) {}

  Spring(const T& value, const T& min, const T& max, const T& delta)
    : _value( value ), _delta( delta ), _min( min ), _max( max ), _signdelta( 1 )
  {

  }

  void setCondition( const T& min, const T& max, const T& delta )
  {
    _min = min;
    _max = max;
    _delta = delta;
    update();
  }

  void setDelta( const T& delta )
  {
    _delta = delta;
  }

  void update()
  {
    if( _min == _max )
      return;

    T newValue = _value + _signdelta * _delta;
    if( newValue <= _min || newValue >= _max )
    {
      _signdelta *= -1;
      if( newValue <= _min ) newValue = _min;
      else                   newValue = _max;
    }

    _value = newValue;
  }

  operator T() const { return _value; }

  const T& value() const { return _value; }

private:
  T _value;
  T _delta;
  T _min, _max;
  int _signdelta;
};

typedef Spring<int> SpringI;
typedef Spring<float> SpringF;

class SpringColor
{
public:
  SpringColor( const NColor& color = DefaultColors::clear )
    : _a( 0xff, 0, 0xff, 1 ),
      _r( 0xff, 0, 0, 0 ),
      _g( 0xff, 0, 0, 0 ),
      _b( 0xff, 0, 0, 0 ),
      _color( color )
  {

  }

  void setColor( const NColor& color )
  {
    _color = color;
  }

  void setAlphaCondition( int min, int max, int delta )
  {
    _a.setCondition( min, max, delta );
  }

  void setDelta( int value )
  {
    _a.setDelta( value);
    _r.setDelta( value );
    _g.setDelta( value );
    _b.setDelta( value );

    update();
  }

  void update()
  {
    _a.update();
    _r.update();
    _g.update();
    _b.update();

    _color.color = (_a << 24) + (_r << 16) + (_g << 8) + _b;
  }

  operator NColor() const { return _color; }

  const NColor& color() const { return _color; }

private:
  SpringI _a, _r, _g, _b;
  NColor _color;
};

}//end namespace math

#endif //__CAESARIA_MATH_SPRING_H_INCLUDE__
