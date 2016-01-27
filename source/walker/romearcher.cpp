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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "romearcher.hpp"
#include "spear.hpp"
#include "city/city.hpp"
#include "gfx/animation.hpp"
#include "objects/construction.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

REGISTER_NAMED_CLASS_IN_WALKERFACTORY( walker::romeSpearman, RomeArcher, archer )

RomeArcher::RomeArcher(PlayerCityPtr city, walker::Type type )
  : RomeSoldier( city, type )
{
  _setSubAction( Soldier::check4attack );
  setAttackDistance( 6 );
}

void RomeArcher::_fire( TilePos p )
{
  SpearPtr spear = Walker::create<Spear>( _city() );
  spear->toThrow( pos(), p );
  wait( game::Date::days2ticks( 1 ) / 2 );
}

void RomeArcher::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  switch( _subAction() )
  {
  case Soldier::fightEnemy:
  {
    WalkerPtr enemy = _findEnemiesInRange( attackDistance() ).firstOrEmpty();

    if( !enemy.isValid() )
    {
      turn( enemy->pos() );

      if( _animation().index() == (int)(_animation().frameCount()-1) )
      {
        _fire( enemy->pos() );
        _updateAnimation( time+1 );
      }
    }
    else
    {
      //_check4attack();
      send2patrol();
    }
  }
  break;

  case Soldier::destroyBuilding:
  {
    ConstructionPtr construction = _findContructionsInRange( attackDistance() ).firstOrEmpty();

    if( !construction.isValid() )
    {
      turn( construction->pos() );

      if( _animation().index() == (int)(_animation().frameCount()-1) )
      {
        _fire( construction->pos() );
        _updateAnimation( time+1 );
      }
    }
    else
    {
      //_check4attack();
      send2patrol();
    }
  }

  case Soldier::patrol:
    if( game::Date::current().day() % 2 == 0 )
    {
      _tryAttack();
    }
  break;

  default: break;
  } // end switch( _d->action )
}
