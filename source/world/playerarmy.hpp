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

#ifndef __CAESARIA_WORLD_PLAYERARMY_H_INCLUDED__
#define __CAESARIA_WORLD_PLAYERARMY_H_INCLUDED__

#include "army.hpp"
#include "walker/romesoldier.hpp"

namespace world
{

class PlayerArmy : public Army
{
public:
  typedef enum { findAny, go2location, wait, go2home } Mode;

  static PlayerArmyPtr create(EmpirePtr empire, CityPtr city=CityPtr() );
  virtual std::string type() const;

  virtual void timeStep( const unsigned int time );
  virtual void move2location( Point location );
  virtual void setFortPos(const TilePos& base );
  virtual void return2fort();
  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  virtual void killSoldiers(int percent);

  Mode mode() const;
  virtual int viewDistance() const;

  virtual void addSoldiers(RomeSoldierList soldiers);

protected:
  virtual bool _isAgressiveArmy(ArmyPtr other) const;
  virtual void _reachedWay();
  virtual void _check4attack();
  virtual void _noWay();
  virtual void _attackAny();
  virtual bool _attackObject( ObjectPtr obj );

  PlayerArmy( EmpirePtr empire );

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}

#endif //__CAESARIA_WORLD_PLAYERARMY_H_INCLUDED__
