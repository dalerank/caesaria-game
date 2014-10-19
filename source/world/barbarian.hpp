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

#ifndef __CAESARIA_WORLD_BARBARIAN_H_INCLUDED__
#define __CAESARIA_WORLD_BARBARIAN_H_INCLUDED__

#include "army.hpp"

namespace world
{

class Barbarian : public Army
{
public:
  static BarbarianPtr create(EmpirePtr empire, Point location=Point(-1,-1) );
  virtual std::string type() const;

  virtual void timeStep( unsigned int time );

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  virtual void updateStrength( int value );
  virtual int viewDistance() const;

protected:
  virtual bool _isAgressiveArmy(ArmyPtr other) const;
  virtual void _reachedWay();
  virtual void _check4attack();
  virtual void _goaway();
  virtual void _noWay();
  virtual void _attackAny();
  virtual bool _attackObject( ObjectPtr obj );

  Barbarian( EmpirePtr empire );

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}

#endif //__CAESARIA_WORLD_BARBARIAN_H_INCLUDED__
