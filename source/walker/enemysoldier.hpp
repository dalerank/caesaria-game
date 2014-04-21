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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ENEMYSOLDIER_H_INCLUDED__
#define __CAESARIA_ENEMYSOLDIER_H_INCLUDED__

#include "soldier.hpp"

class EnemySoldier : public Soldier
{
public:
  typedef enum { check4attack=0,
                 go2position,
                 go2enemy,
                 fightEnemy,
                 destroyBuilding,
                 doNothing } EsAction;

  static EnemySoldierPtr create( PlayerCityPtr city, constants::walker::Type type );

  virtual void timeStep(const unsigned long time);

  virtual void load( const VariantMap& stream );
  virtual void save( VariantMap& stream ) const;

  virtual void wait( unsigned int time );

  virtual void send2City( TilePos pos );
  virtual void die();

  virtual ~EnemySoldier();

  virtual int agressive() const;

protected:
  virtual void _centerTile();
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual bool _tryAttack();

  void _setSubAction( EsAction action );
  EsAction _getSubAction() const;

  BuildingList _findBuildingsInRange(unsigned int range);
  WalkerList _findEnemiesInRange(unsigned int range);
  virtual void _check4attack();
  bool _isTileBusy( TilePos p);
  bool _move2freePos();

  EnemySoldier( PlayerCityPtr city, constants::walker::Type type );

private:
  Pathway _findFreeSlot(const int range);
  Pathway _findPathway2NearestEnemy(unsigned int range);
  Pathway _findPathway2NearestConstruction(unsigned int range);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_ENEMYSOLDIER_H_INCLUDED__
