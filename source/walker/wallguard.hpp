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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_WALLGUARD_INCLUDE_H_
#define _CAESARIA_WALLGUARD_INCLUDE_H_

#include "romesoldier.hpp"
#include "objects/predefinitions.hpp"

/** Soldier, friend or enemy */
class WallGuard : public RomeSoldier
{
public:
  static WallGuardPtr create( PlayerCityPtr city, constants::walker::Type type );

  void send2city(TowerPtr tower, Pathway pathway);
  void setBase( TowerPtr tower );

  virtual bool die();
  virtual void timeStep(const unsigned long time);
  virtual void fight();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual std::string thoughts(Thought th) const;
  virtual TilePos places(Place type) const;

protected:
  virtual void _centerTile();
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual void _waitFinished();
  virtual void _fire( TilePos target );
  virtual void _back2base();
  virtual bool _tryAttack();

  void _back2patrol();
  FortificationList _findNearestWalls( EnemySoldierPtr enemy );
  EnemySoldierPtr _findNearbyEnemy(EnemySoldierList enemies);

  Pathway _attackEnemyInRange(unsigned int range);

  WallGuard(PlayerCityPtr city, constants::walker::Type type);

  virtual ~WallGuard();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //_CAESARIA_WALLGUARD_INCLUDE_H_
