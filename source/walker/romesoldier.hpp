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

#ifndef _CAESARIA_ROMESOLDIER_INCLUDE_H_
#define _CAESARIA_ROMESOLDIER_INCLUDE_H_

#include "soldier.hpp"
#include "objects/predefinitions.hpp"

class RomeSoldier : public Soldier
{
public:  
  static RomeSoldierPtr create( PlayerCityPtr city, constants::walker::Type type );

  void send2city(FortPtr base, TilePos pos);
  void send2expedition(const std::string& name);

  virtual bool die();
  virtual void timeStep(const unsigned long time);
  virtual void return2fort();
  virtual void send2patrol();  

  virtual void save(VariantMap &stream) const;
  virtual FortPtr base() const;
  virtual void load(const VariantMap &stream);

  virtual std::string thoughts(Thought th) const;
  virtual TilePos places(Place type) const;

  virtual ~RomeSoldier();

protected:
  virtual void _centerTile();
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual bool _tryAttack();
  virtual void _back2base();
  virtual void _duckout();

  void _init( constants::walker::Type type );

  virtual WalkerList _findEnemiesInRange(unsigned int range);
  virtual ConstructionList _findContructionsInRange(unsigned int range);

  Pathway _findPathway2NearestEnemy(unsigned int range);
  RomeSoldier(PlayerCityPtr city, constants::walker::Type type);

private:
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //_CAESARIA_ROMESOLDIER_INCLUDE_H_
