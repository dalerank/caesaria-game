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

#include "clay_pit.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "events/showinfobox.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::clay_pit, ClayPit)

ClayPit::ClayPit()
  : Factory( good::none, good::clay, constants::objects::clay_pit, Size(2) )
{
  _fgPicturesRef().resize(2);

  _setUnworkingInterval( 8 );
}

void ClayPit::timeStep( const unsigned long time )
{
  Factory::timeStep( time );
}

void ClayPit::_reachUnworkingTreshold()
{
  Factory::_reachUnworkingTreshold();

  events::GameEventPtr e = events::ShowInfobox::create( "##clay_pit_flooded##", "##clay_pit_flooded_by_low_support##");
  e->dispatch();
}

bool ClayPit::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool is_constructible = Construction::canBuild( areaInfo );
  bool near_water = false;

  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray perimetr = tilemap.getRectangle( areaInfo.pos + TilePos( -1, -1), size() + Size( 2 ), Tilemap::checkCorners );

  foreach( tile, perimetr )  {  near_water |= (*tile)->getFlag( Tile::tlWater ); }

  const_cast<ClayPit*>( this )->_setError( near_water ? "" : "##clay_pit_need_water##" );

  return (is_constructible && near_water);
} 
