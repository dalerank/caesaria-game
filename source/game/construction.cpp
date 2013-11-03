// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "construction.hpp"

#include "gfx/tile.hpp"
#include "game/tilemap.hpp"
#include "game/city.hpp"
#include "events/event.hpp"

Construction::Construction(const Type type, const Size& size)
: TileOverlay( type, size )
{
}

bool Construction::canBuild( CityPtr city, const TilePos& pos ) const
{
  Tilemap& tilemap = city->getTilemap();

  bool is_constructible = true;

  //return area for available tiles
  TilemapArea area = tilemap.getArea( pos, getSize() );

  //on over map size
  if( (int)area.size() != getSize().getArea() )
    return false;

  foreach( Tile* tile, area )
  {
     is_constructible &= tile->getFlag( Tile::isConstructible );
  }

  return is_constructible;
}

std::string Construction::getError() const { return ""; }

void Construction::build( CityPtr city, const TilePos& pos )
{
  TileOverlay::build( city, pos );

  computeAccessRoads();
}

const TilemapTiles& Construction::getAccessRoads() const
{
   return _accessRoads;
}

// here the problem lays: if we remove road, it is left in _accessRoads array
// also we need to recompute _accessRoads if we place new road tile
// on next to this road tile buildings
void Construction::computeAccessRoads()
{
  _accessRoads.clear();
  if( !_getMasterTile() )
      return;

  Tilemap& tilemap = _getCity()->getTilemap();

  int maxDst2road = getRoadAccessDistance();
  TilemapTiles rect = tilemap.getRectangle( _getMasterTile()->getIJ() + TilePos( -maxDst2road, -maxDst2road ),
                                            getSize() + Size( 2 * maxDst2road ), !Tilemap::checkCorners );
  foreach( Tile* tile, rect )
  {
    if( tile->getFlag( Tile::tlRoad ) )
    {
      _accessRoads.push_back( tile );
    }
  }
}

int Construction::getRoadAccessDistance() const
{
  return 1;
  // it is default value
  // for houses - 2
}

void Construction::burn()
{
  deleteLater();

  events::GameEventPtr event = events::DisasterEvent::create( getTile().getIJ(), events::DisasterEvent::fire );
  event->dispatch();
}

void Construction::collapse()
{
  deleteLater();
  events::GameEventPtr event = events::DisasterEvent::create( getTile().getIJ(), events::DisasterEvent::collapse );
  event->dispatch();
}

const MetaData::Desirability& Construction::getDesirabilityInfo() const
{
  return MetaDataHolder::instance().getData( getType() ).getDesirbilityInfo();
}

void Construction::destroy()
{
  TileOverlay::destroy();
}

bool Construction::isNeedRoadAccess() const
{
  return true;
}
