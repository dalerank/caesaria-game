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

#ifndef __CAESARIA_ARMY_H_INCLUDED__
#define __CAESARIA_ARMY_H_INCLUDED__

#include "predefinitions.hpp"
#include "object.hpp"
#include "core/variant.hpp"

namespace world
{

class Army : public Object
{
public:
  static ArmyPtr create( Empire& empire, CityPtr base );
  virtual ~Army();

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

  virtual void setBase( CityPtr base);
  virtual void attack( ObjectPtr obj );

  virtual void timeStep(const unsigned int time);

private:
  Army( Empire& empire );
  __DECLARE_IMPL(Army)
};

}
#endif //__CAESARIA_ARMY_H_INCLUDED__
