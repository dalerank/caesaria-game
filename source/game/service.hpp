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

#ifndef __CAESARIA_SERVICE_H_INCLUDED__
#define __CAESARIA_SERVICE_H_INCLUDED__

#include "core/enumerator.hpp"
#include "core/math.hpp"

class Service
{
public:
  typedef enum
  {
    well=0, fountain,
    market,
    engineer,
    senate, forum,
    prefect,
    religionNeptune, religionCeres, religionVenus, religionMars, religionMercury, oracle,
    doctor, barber, baths, hospital,
    school, library, academy,
    theater, amphitheater, colloseum,
    hippodrome,
    recruter,
    crime,
    guard,
    srvCount
  } Type;

  Service() : _value( 0 ), _min( 0 ), _max( 100 ) {}

  void set( float i ) { _value = math::clamp<float>( i, _min, _max); }
  float value() const { return _value; }

  int max() const { return _max; }
  void setMax( int value ) { _max = value; set( _value ); }

  operator float() const { return _value; }

  Service& operator=( float i) { set( i ); return *this; }
  Service& operator-=(float i) { set( _value - i ); return *this; }
  Service& operator+=(float i) { set( _value + i ); return *this; }
private:
  float _value;
  int _min, _max;
};

class ServiceHelper : public EnumsHelper<Service::Type>
{
public:
  static ServiceHelper& instance();

  static Service::Type getType(const std::string& name);
  static std::string getName( Service::Type type );

private:
  ServiceHelper();
};

#endif //__CAESARIA_SERVICE_H_INCLUDED__
