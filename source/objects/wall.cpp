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

#include "wall.hpp"

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "constants.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "objects/road.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"

using namespace constants;

Wall::Wall() : Building( building::wall, Size(1) )
{
  setPicture( ResourceGroup::wall, 178 ); // default picture for wall
}

void Wall::build(PlayerCityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build if already have wall here
  WallPtr wall = terrain.getOverlay().as<Wall>();
  if( wall.isValid() )
  {
    return;
  }

  Construction::build( city, pos );

  CityHelper helper( city );
  WallList walls = helper.find<Wall>( building::wall );

  foreach( wall, walls ) { (*wall)->updatePicture( city ); }

  updatePicture( city );
}

void Wall::destroy()
{
  Construction::destroy();

  if( _getCity().isValid() )
  {
    TilesArray area = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 2, 2), Size( 5 ) );

    foreach( tile, area )
    {
      WallPtr wall = (*tile)->getOverlay().as<Wall>();
      if( wall.isValid()  )
      {
        wall->updatePicture( _getCity() );
      }
    }
  }
}

void Wall::initTerrain(Tile &terrain)
{
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );

  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, false );
  terrain.setFlag( Tile::tlMeadow, isMeadow );
}

bool Wall::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool ret = Construction::canBuild( city, pos, aroundTiles );

  if( ret )
  {
    Picture pic = const_cast< Wall* >( this )->getPicture( city, pos, aroundTiles );
    const_cast< Wall* >( this )->setPicture( pic );
  }

  return ret;
}

const Picture& Wall::getPicture(PlayerCityPtr city, TilePos pos, const TilesArray& tmp) const
{
  // find correct picture as for roads
  Tilemap& tmap = city->getTilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (tmp.empty()) ? getTilePos() : pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::wall, 178 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection] = { 0 };
  bool is_busy[countDirection] = { 0 };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west]  = tile_pos + TilePos( -1,  0);
  tile_pos_d[northEast] = tile_pos + TilePos( 1, 1 );
  tile_pos_d[southEast] = tile_pos + TilePos( 1, -1 );


  // all tiles must be in map range
  for (int i = 0; i < countDirection; ++i)
  {
    is_border[i] = !tmap.isInside(tile_pos_d[i]);
    if (is_border[i])
      tile_pos_d[i] = tile_pos;
  }

  // get overlays for all directions
  TileOverlayPtr overlay_d[countDirection];
  overlay_d[north] = tmap.at( tile_pos_d[north] ).getOverlay();
  overlay_d[east] = tmap.at( tile_pos_d[east]  ).getOverlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).getOverlay();
  overlay_d[west] = tmap.at( tile_pos_d[west]  ).getOverlay();
  overlay_d[northEast] = tmap.at( tile_pos_d[northEast]  ).getOverlay();
  overlay_d[southEast] = tmap.at( tile_pos_d[southEast]  ).getOverlay();

  // if we have a TMP array with wall, calculate them
  if (!tmp.empty())
  {
    for( TilesArray::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
      if( (*it)->getOverlay().isNull()
          || (*it)->getOverlay()->getType() != building::wall)
      {
        continue;
      }

      TilePos rpos = (*it)->getIJ();
      int i = (*it)->getI();
      int j = (*it)->getJ();

      if( (pos + TilePos( 0, 1 )) == rpos ) is_busy[north] = true;
      else if(i == pos.i() && j == (pos.j() - 1)) is_busy[south] = true;
      else if(j == pos.j() && i == (pos.i() + 1)) is_busy[east] = true;
      else if(j == pos.j() && i == (pos.i() - 1)) is_busy[west] = true;
      else if((pos + TilePos(1, 1)) == rpos ) is_busy[northEast] = true;
      else if((pos + TilePos(1, -1)) == rpos ) is_busy[southEast] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i) {
    if( !is_border[i] &&
       ( (overlay_d[i].isValid() &&
          (overlay_d[i]->getType() == building::wall || overlay_d[i]->getType() == building::gatehouse ) )
        || is_busy[i] ) )
    {
      switch (i)
      {
      case north: directionFlags += 0x1; break;
      case east:  directionFlags += 0x2; break;
      case south: directionFlags += 0x4; break;
      case west:  directionFlags += 0x8; break;
      case northEast: directionFlags += 0x10; break;
      case southEast: directionFlags += 0x20; break;      
      default: break;
      }
    }
  }

  Wall& th = *const_cast< Wall* >( this );
  th._fgPicturesRef().clear();
  int index;
  switch( directionFlags & 0xf )
  {  
  case 0: index = 168; break;  // no neighbours!
  case 1: index = 166; break; // N

  case 2: index = 157; break; // E
  case 3: index = 179; break; // N + E
  case 4: index = 183; break; // S

  case 5: index = 183; break; // N + S
  case 6: index = 174; break; // E + S
  case 8: index = 167; break; // W
  case 9: index = 164; break; // N + W
  case 10: index = 184; break; // E + W

  case 12: index = 180; break; // S + W
  case 14: index = 178;  // N + S + W
  {
    if( (directionFlags & 0x20) == 0 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 183 ) );
      th._fgPicturesRef().back().addOffset( -15, -8 );
    }
  }
  break;

  case 11: index = 178; break; // N + E + W

  case 7: // N + E + S
  case 13: // W + S + N
  case 15: // W + E + N + S
    index = 178;
    if( (directionFlags & 0x20) == 0 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 183 ) );
      th._fgPicturesRef().back().addOffset( -15, -8 );
    }
  break; // N + S + E + W (crossing)

  default:
    index = 178; // it's impossible, but ...
    Logger::warning( "Impossible direction on wall building [%d,%d]", pos.i(), pos.j() );
  }

  return Picture::load( ResourceGroup::wall, index );
}

void Wall::updatePicture(PlayerCityPtr city)
{
  setPicture( getPicture( city, TilePos(), TilesArray() ) );
}

bool Wall::isNeedRoadAccess() const
{
  return false;
}

bool Wall::isWalkable() const
{
  return false;
}

bool Wall::isRoad() const
{
  return false;
}
