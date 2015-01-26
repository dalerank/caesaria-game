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

#include "core/utils.hpp"
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
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::wall, Wall)

Wall::Wall()
  : Building( objects::wall, Size(1) )
{
  setPicture( ResourceGroup::wall, 178 ); // default picture for wall
}

bool Wall::build( const CityAreaInfo& info )
{
  Tilemap& tilemap = info.city->tilemap();
  Tile& terrain = tilemap.at( info.pos );

  // we can't build if already have wall here
  WallPtr wall = ptr_cast<Wall>( terrain.overlay() );
  if( wall.isValid() )
  {
    return false;
  }

  Construction::build( info );

  city::Helper helper( info.city );
  WallList walls = helper.find<Wall>( objects::wall );

  foreach( wall, walls ) { (*wall)->updatePicture( info.city ); }

  updatePicture( info.city );

  return true;
}

void Wall::destroy()
{
  Construction::destroy();

  if( _city().isValid() )
  {
    TilesArray area = _city()->tilemap().getArea( pos() - TilePos( 2, 2), Size( 5 ) );

    foreach( tile, area )
    {
      WallPtr wall = ptr_cast<Wall>( (*tile)->overlay() );
      if( wall.isValid()  )
      {
        wall->updatePicture( _city() );
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

bool Wall::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool ret = Construction::canBuild( areaInfo );

  if( ret )
  {
    Picture pic = const_cast< Wall* >( this )->picture( areaInfo );
    const_cast< Wall* >( this )->setPicture( pic );
  }

  return ret;
}

const Picture& Wall::picture(const CityAreaInfo& areaInfo) const
{
  // find correct picture as for roads
  Tilemap& tmap = areaInfo.city->tilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (areaInfo.aroundTiles.empty()) ? pos() : areaInfo.pos;

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
  overlay_d[north] = tmap.at( tile_pos_d[north] ).overlay();
  overlay_d[east] = tmap.at( tile_pos_d[east]  ).overlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).overlay();
  overlay_d[west] = tmap.at( tile_pos_d[west]  ).overlay();
  overlay_d[northEast] = tmap.at( tile_pos_d[northEast]  ).overlay();
  overlay_d[southEast] = tmap.at( tile_pos_d[southEast]  ).overlay();

  // if we have a TMP array with wall, calculate them
  if (!areaInfo.aroundTiles.empty())
  {
    foreach( it, areaInfo.aroundTiles )
    {
      if( (*it)->overlay().isNull()
          || (*it)->overlay()->type() != objects::wall)
      {
        continue;
      }

      TilePos rpos = (*it)->pos();
      int i = (*it)->i();
      int j = (*it)->j();

      const TilePos& p = areaInfo.pos;
      if( (p + TilePos( 0, 1 )) == rpos ) is_busy[north] = true;
      else if(i == p.i() && j == (p.j() - 1)) is_busy[south] = true;
      else if(j == p.j() && i == (p.i() + 1)) is_busy[east] = true;
      else if(j == p.j() && i == (p.i() - 1)) is_busy[west] = true;
      else if((p + TilePos(1, 1)) == rpos ) is_busy[northEast] = true;
      else if((p + TilePos(1, -1)) == rpos ) is_busy[southEast] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i) {
    if( !is_border[i] &&
       ( (overlay_d[i].isValid() &&
          (overlay_d[i]->type() == objects::wall || overlay_d[i]->type() == objects::gatehouse ) )
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
    Logger::warning( "Impossible direction on wall building [%d,%d]", areaInfo.pos.i(), areaInfo.pos.j() );
  }

  return Picture::load( ResourceGroup::wall, index );
}

void Wall::updatePicture(PlayerCityPtr city)
{
  CityAreaInfo areaInfo = { city, TilePos(), TilesArray() };
  setPicture( picture( areaInfo ) );
}

bool Wall::isNeedRoadAccess() const
{
  return false;
}

bool Wall::isWalkable() const
{
  return false;
}
