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
  typedef enum { attackAll, attackFood, attackCitizen, attackBestBuilding,
                 attackIndustry, attackSenate } AttackPriority;
  static EnemySoldierPtr create( PlayerCityPtr city, constants::walker::Type type );

  virtual void timeStep(const unsigned long time);

  virtual void load( const VariantMap& stream );
  virtual void save( VariantMap& stream ) const;

  virtual void send2City( TilePos pos );
  virtual bool die();
  virtual void setAttackPriority( AttackPriority who );

  virtual void acceptAction(Action action, TilePos pos);

  virtual ~EnemySoldier();

  virtual int agressive() const;

protected:
  virtual void _centerTile();
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual bool _tryAttack();
  virtual void _waitFinished();
  virtual AttackPriority _attackPriority() const;

  virtual BuildingList _findBuildingsInRange(unsigned int range);
  virtual WalkerList _findEnemiesInRange(unsigned int range);
  virtual void _check4attack();
  virtual Pathway _findPathway2NearestConstruction(unsigned int range);

  EnemySoldier( PlayerCityPtr city, constants::walker::Type type );

private:
  Pathway _findFreeSlot(TilePos target, const int range);
  Pathway _findPathway2NearestEnemy(unsigned int range);
  AttackPriority _atPriority;
};

#endif //__CAESARIA_ENEMYSOLDIER_H_INCLUDED__
