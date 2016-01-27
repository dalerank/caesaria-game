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

#include "balista.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "enemysoldier.hpp"
#include "gfx/tilepos.hpp"
#include "game/gamedate.hpp"
#include "config.hpp"
#include "spear.hpp"

Balista::Balista( PlayerCityPtr city )
  : WallGuard( city, walker::balista )
{
  _setType( walker::balista );
  setSpeedMultiplier( 0.f );
  setAttackDistance( config::distance::ballistaRange );

  _isActive = false;

  setName( _("##balista##") );
}

Balista::~Balista(){}

void Balista::setActive(bool active)
{
  _isActive = active;
  _setAction( active ? acWork : acNone );
}

bool Balista::_tryAttack()
{
  auto enemies = _findEnemiesInRange( attackDistance() ).select<EnemySoldier>();

  if( !enemies.empty() )
  {
    //find nearest walkable wall
    auto soldierInAttackRange = _findNearbyEnemy( enemies );

    if( soldierInAttackRange.isValid() )
    {
      _setSubAction( fightEnemy );
      fight();
      return true;
    }
  }

  return false;
}

void Balista::_fire( TilePos target )
{
  SpearPtr spear = Walker::create<Spear>( _city() );
  spear->setPicInfo( ResourceGroup::sprites, 146 );
  spear->setPicOffset( Point( -15, 15 ));
  spear->toThrow( pos(), target );
  wait( game::Date::days2ticks( 1 ) / 2 );
}

void Balista::timeStep(const unsigned long time)
{
  WallGuard::timeStep( time );
}

void Balista::_back2base()
{
  _setSubAction( Soldier::patrol );
  _setAction( acWork );
}
