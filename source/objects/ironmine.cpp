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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "ironmine.hpp"

#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tilesarray.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::iron_mine, IronMine)

IronMine::IronMine()
  : Factory(good::none, good::iron, object::iron_mine, Size(2) )
{
  _picture().load( ResourceGroup::commerce, 54 );

  _animation().load( ResourceGroup::commerce, 55, 6 );
  _animation().setDelay( 5 );
  _fgPictures().resize( 2 );

  _setUnworkingInterval( 12 );
}

bool IronMine::build(const city::AreaInfo& info)
{
  bool isOk = Factory::build( info );

  bool mayCollapse = info.city->getOption( PlayerCity::claypitMayCollapse ) != 0;
  if( !mayCollapse )
    _setUnworkingInterval( 0 );

  return isOk;
}

bool IronMine::canBuild( const city::AreaInfo& areaInfo ) const
{
  bool is_constructible = Factory::canBuild( areaInfo );

  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray perimetr = tilemap.rect( areaInfo.pos + TilePos( -1, -1 ),
                                      areaInfo.pos + TilePos( 3, 3 ), Tilemap::checkCorners );

  bool near_mountain = perimetr.select( Tile::tlRock ).size() > 0;  // tells if the factory is next to a mountain

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

  events::dispatch<events::ShowInfobox>( "##iron_mine_collapse##", "##iron_mine_collapse_by_low_support##");

  collapse();
}
