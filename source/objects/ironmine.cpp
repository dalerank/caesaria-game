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

#include "ironmine.hpp"

#include "gfx/tile.hpp"
#include "gfx/tilesarray.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "city/helper.hpp"
#include "events/showinfobox.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::iron_mine, IronMine)

IronMine::IronMine()
  : Factory(good::none, good::iron, objects::iron_mine, Size(2) )
{
  setPicture( ResourceGroup::commerce, 54 );

  _animationRef().load( ResourceGroup::commerce, 55, 6 );
  _animationRef().setDelay( 5 );
  _fgPicturesRef().resize( 2 );

  _setUnworkingInterval( 8 );
}

bool IronMine::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool is_constructible = WorkingBuilding::canBuild( areaInfo );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray perimetr = tilemap.getRectangle( areaInfo.pos + TilePos( -1, -1 ),
                                              areaInfo.pos + TilePos(3, 3), Tilemap::checkCorners );

  foreach( it, perimetr ) { near_mountain |= (*it)->getFlag( Tile::tlRock ); }

  const_cast< IronMine* >( this )->_setError( near_mountain ? "" : "##iron_mine_need_mountain_near##" );

  return (is_constructible && near_mountain);
}

void IronMine::timeStep(const unsigned long time)
{
  Factory::timeStep( time );
}

void IronMine::_reachUnworkingTreshold()
{
  Factory::_reachUnworkingTreshold();

  events::GameEventPtr e = events::ShowInfobox::create( "##iron_mine_collapse##", "##iron_mine_collpase_by_low_support##");
  e->dispatch();
}
