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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_SOLDIER_INCLUDE_H_
#define _CAESARIA_SOLDIER_INCLUDE_H_

#include "human.hpp"
#include "walkers_factory_creator.hpp"

/** Soldier, friend or enemy */
class Soldier : public Human
{
public:
  typedef enum { check4attack=0,
                 go2position,
                 go2enemy,
                 fightEnemy,
                 destroyBuilding,
                 back2base,
                 duckout,
                 patrol,
                 userAction,
                 doNothing=0xff } SldrAction;

  virtual ~Soldier();

  virtual void fight();

  float resistance() const;
  void setResistance( float value );

  float strike() const;
  void setStrike( float value );

  int morale() const;
  void updateMorale( int value );

  virtual void wait(int ticks);
  virtual void initialize(const VariantMap &options);

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual unsigned int attackDistance() const;
  virtual void setAttackDistance( unsigned int distance );
  virtual bool isFriendTo( WalkerPtr wlk ) const;
  virtual void setTarget( TilePos location );
  TilePos target() const;
  void addFriend( constants::walker::Type friendType);

protected:
  Soldier(PlayerCityPtr city, constants::walker::Type type);

  SldrAction _subAction() const;
  void _setSubAction( SldrAction action );

  virtual bool _move2freePos(TilePos target);
  virtual ConstructionList _findContructionsInRange(unsigned int range) = 0;
  virtual WalkerList _findEnemiesInRange(unsigned int range) = 0;

private:
  __DECLARE_IMPL(Soldier)
};

template< class T >
class SoldierCreator : public WalkerCreator
{
public:
  WalkerPtr create( PlayerCityPtr city ) { return T::create( city, rtype ).object();  }
  SoldierCreator( const constants::walker::Type type ) { rtype = type;  }
  constants::walker::Type rtype;
};

#define REGISTER_SOLDIER_IN_WALKERFACTORY(type,rtype,rclass,ctorname) \
namespace { \
struct Registrator_##ctorname { Registrator_##ctorname() { WalkerManager::instance().addCreator( type, new SoldierCreator<rclass>( rtype ) ); }}; \
static Registrator_##ctorname rtor_##ctorname; \
}

#endif //_CAESARIA_SOLDIER_INCLUDE_H_
