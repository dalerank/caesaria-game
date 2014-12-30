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
#include "movableobject.hpp"
#include "core/variant.hpp"

namespace world
{

class Army : public MovableObject
{
public:
  static ArmyPtr create( EmpirePtr empire );
  virtual ~Army();

  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );
  virtual std::string type() const;
  virtual void setBase( CityPtr base);
  virtual void attack( ObjectPtr obj );
  virtual void setStrength( int value );
  virtual int strength() const;  
  virtual void killSoldiers( int percent );
  virtual void addObject(ObjectPtr);
  std::string target() const;

  static void battle(unsigned int attackers, unsigned int defenders, int &atcLoss, int &defLoss);

protected:
  virtual bool _isAgressiveArmy(ArmyPtr other) const;
  void _reachedWay();
  Army( EmpirePtr empire );

private: 
  __DECLARE_IMPL(Army)
};

}
#endif //__CAESARIA_ARMY_H_INCLUDED__
