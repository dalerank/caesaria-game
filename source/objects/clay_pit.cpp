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

#include "clay_pit.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "objects/metadata.hpp"
#include "objects/constants.hpp"
#include "events/showinfobox.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace events;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::clay_pit, ClayPit)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::flooded_clay_pit, FloodedClayPit)

ClayPit::ClayPit()
  : Factory( good::none, good::clay, object::clay_pit, Size(2) )
{
  _fgPictures().resize(2);
  _setUnworkingInterval( 12 );
}

bool ClayPit::build(const city::AreaInfo& info)
{
  bool isOk = Factory::build( info );

  bool mayCollapse = info.city->getOption( PlayerCity::minesMayCollapse ) != 0;
  if( !mayCollapse )
    _setUnworkingInterval( 0 );

  return isOk;
}

void ClayPit::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

void ClayPit::flood()
{
  deleteLater();

  Logger::warning( "WARNING!!! ClaiPit was flooded at [{},{}]", pos().i(), pos().j() );

  OverlayPtr flooded( new FloodedClayPit() );
  flooded->drop();
  flooded->build( city::AreaInfo( _city(), pos() ) );
  _city()->addOverlay( flooded );
}

void ClayPit::_reachUnworkingTreshold()
{
  Factory::_reachUnworkingTreshold();

  events::dispatch<ShowInfobox>( "##clay_pit_flooded##", "##clay_pit_flooded_by_low_support##");

  flood();
}

bool ClayPit::canBuild( const city::AreaInfo& areaInfo ) const
{
  bool is_constructible = Construction::canBuild( areaInfo );

  if( !is_constructible )
    return false;

  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray perimetr = tilemap.rect( areaInfo.pos + TilePos( -1, -1), size() + Size( 2 ), Tilemap::checkCorners );

  bool near_water = !perimetr.select( Tile::tlWater ).empty();

  const_cast<ClayPit*>( this )->_setError( near_water ? "" : "##clay_pit_need_water##" );

  return near_water;
} 

FloodedClayPit::FloodedClayPit()
  : Ruins( object::flooded_clay_pit )
{
  setSize( Size(2) );
  setPicture( info().randomPicture( size() ) );
}
