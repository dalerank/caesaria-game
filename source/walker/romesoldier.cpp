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

#include "romesoldier.hpp"
#include "game/city.hpp"
#include "game/name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "building/military.hpp"
#include "game/pathway_helper.hpp"

using namespace constants;

class RomeSoldier::Impl
{
public:
  typedef enum { doNothing=0, back2fort, go2position } State;
  FortPtr base;
  State action;
  gfx::Type walk;
  gfx::Type fight;
  double strikeForce, resistance;
};

RomeSoldier::RomeSoldier( PlayerCityPtr city, walker::Type type ) : Soldier( city ), _d( new Impl )
{
  _setType( type );
  _setAnimation( gfx::soldier );

  _init( type );
}

void RomeSoldier::_init( walker::Type type )
{
  _setType( type );
  switch( type )
  {
  case walker::legionary:
    _setAnimation( gfx::legionaryGo );
    _d->walk = gfx::legionaryGo;
    _d->fight = gfx::legionaryFight;
    _d->strikeForce = 3;
    _d->resistance = 1;
  break;
  default:
    _CAESARIA_DEBUG_BREAK_IF( __FILE__": function not work yet");
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

RomeSoldierPtr RomeSoldier::create(PlayerCityPtr city, walker::Type type)
{
  RomeSoldierPtr ret( new RomeSoldier( city, type ) );
  ret->drop();

  return ret;
}

void RomeSoldier::die()
{
  Soldier::die();

  switch( getType() )
  {
  case walker::legionary:
    Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen3, 705, 712 );
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }
}

bool RomeSoldier::_tryAttack()
{
  return false;
}

void RomeSoldier::_back2fort()
{
  if( _d->base.isValid() )
  {
    Pathway way = PathwayHelper::create( getIJ(), _d->base->getFreeSlot(), PathwayHelper::allTerrain );

    if( way.isValid() )
    {
      setPathway( way );
      _d->action = Impl::go2position;
      go();
      return;
    }
  }
  else
  {
    die();
  }
}

void RomeSoldier::_centerTile()
{
  switch( _d->action )
  {
  case Impl::doNothing:
  break;

  case Impl::go2position:
  {
    if( _tryAttack() )
      return;
  }
  break;

  default:
  break;
  }
  Walker::_centerTile();
}

void RomeSoldier::send2city(FortPtr base, TilePos pos )
{
  setIJ( pos );
  _d->base = base;
  _back2fort();

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}
