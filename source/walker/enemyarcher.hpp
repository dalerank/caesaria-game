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

#ifndef _CAESARIA_ENEMYARCHER_INCLUDE_H_
#define _CAESARIA_ENEMYARCHER_INCLUDE_H_

#include "enemysoldier.hpp"
#include "predefinitions.hpp"

class EnemyArcher : public EnemySoldier
{
public:
  static EnemyArcherPtr create( PlayerCityPtr city, constants::walker::Type type );

  void send2city( TilePos pos );

  virtual void timeStep(const unsigned long time);

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

protected:
  virtual bool _tryAttack();
  virtual void _fire(TilePos pos );

private:
  EnemyArcher(PlayerCityPtr city, constants::walker::Type type);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_ENEMYARCHER_INCLUDE_H_
