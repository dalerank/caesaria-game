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
// Copyright 2014-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_METRIC_H_INCLUDED__
#define __CAESARIA_METRIC_H_INCLUDED__

namespace metric
{

static const float unit2QtyLimiter = 100.f;
class Unit;

class Qty
{
public:
  explicit Qty( unsigned int value ) : _value( value ) {}

  operator unsigned int() const { return _value; }
  Unit toUnits() const;

private:
  unsigned int _value;
};

class Unit
{
public:
  Unit( const Qty& qty ) { _value = (unsigned int)qty / unit2QtyLimiter; }
  static Unit fromQty( unsigned int value ) { return Unit( value / unit2QtyLimiter ); }
  unsigned int toQty() { return _value * unit2QtyLimiter; }
  float value() const { return _value; }
  float ivalue() const { return (int)_value; }
  bool operator>( float v ) const { return _value > v; }

private:
  explicit Unit( unsigned int value) : _value( value ) {}

  float _value;
};

inline Unit Qty::toUnits() const { return Unit::fromQty( _value ); }

}
#endif //__CAESARIA_METRIC_H_INCLUDED__
