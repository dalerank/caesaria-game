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

#include "military.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "walker/romesoldier.hpp"
#include "core/logger.hpp"
#include "events/event.hpp"
#include "walker/patrolpoint.hpp"
#include "barracks.hpp"
#include "game/gamedate.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include "walker/romehorseman.hpp"
#include "walker/helper.hpp"
#include "walker/romearcher.hpp"

using namespace constants;
using namespace gfx;

FortLegionary::FortLegionary()
  : Fort( building::fortLegionaire, 16 )
{
  setPicture( ResourceGroup::security, 12 );
}

void FortLegionary::build(PlayerCityPtr city, const TilePos& pos)
{
  Fort::build( city, pos );

  _setPatrolPoint( PatrolPoint::create( city, this,
                                        ResourceGroup::sprites, 21, 8,
                                        pos + TilePos( 3, 3 ) ) );

  BarracksList barracks;
  barracks << city->overlays();

  if( barracks.empty() )
  {
    _setError( "##need_barracks_for_work##" );
  }
}

void FortLegionary::_readyNewSoldier()
{
  RomeSoldierPtr soldier = RomeSoldier::create( _city(), walker::legionary );

  city::Helper helper( _city() );
  TilesArray tiles = helper.getAroundTiles( this );

  foreach( tile, tiles)
  {
    if( (*tile)->isWalkable( true ) )
    {
      soldier->send2city( this, (*tile)->pos() );
      addWalker( soldier.object() );
      return;
    }
  }
}

FortMounted::FortMounted()
  : Fort( constants::building::fortMounted, 15 )
{
  setPicture( ResourceGroup::security, 12 );
}

void FortMounted::build(PlayerCityPtr city, const TilePos& pos)
{
  Fort::build( city, pos );

  _setPatrolPoint( PatrolPoint::create( city, this,
                                        ResourceGroup::sprites, 39, 8,
                                        pos + TilePos( 3, 3 ) ) );

  BarracksList barracks;
  barracks << city->overlays();

  if( barracks.empty() )
  {
    _setError( "##need_barracks_for_work##" );
  }
}

void FortMounted::_readyNewSoldier()
{
  RomeHorsemanPtr soldier = RomeHorseman::create( _city() );

  city::Helper helper( _city() );
  TilesArray tiles = helper.getAroundTiles( this );

  foreach( tile, tiles)
  {
    if( (*tile)->isWalkable( true ) )
    {
      soldier->send2city( this, (*tile)->pos() );
      addWalker( soldier.object() );
      return;
    }
  }
}

FortJaveline::FortJaveline()
  : Fort( building::fortJavelin, 14 )
{
  setPicture( ResourceGroup::security, 12 );
}

void FortJaveline::_readyNewSoldier()
{
  RomeArcherPtr soldier = RomeArcher::create( _city() );

  city::Helper helper( _city() );
  TilesArray tiles = helper.getAroundTiles( this );

  foreach( tile, tiles)
  {
    if( (*tile)->isWalkable( true ) )
    {
      soldier->send2city( this, (*tile)->pos() );
      addWalker( soldier.object() );
      return;
    }
  }
}
