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

#include "aqueduct.hpp"

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "constants.hpp"
#include "game/city.hpp"
#include "gfx/tilemap.hpp"
#include "game/road.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"

using namespace constants;

Aqueduct::Aqueduct() : WaterSource( building::aqueduct, Size(1) )
{
  setPicture( ResourceGroup::aqueduct, 133 ); // default picture for aqueduct
  _setIsRoad( false );
  _setResolved( false );
  // land2a 119 120         - aqueduct over road
  // land2a 121 122         - aqueduct over plain ground
  // land2a 123 124 125 126 - aqueduct corner
  // land2a 127 128         - aqueduct over dirty roads
  // land2a 129 130 131 132 - aqueduct T-shape crossing
  // land2a 133             - aqueduct crossing
  // land2a 134 - 148       - aqueduct without water
}

void Aqueduct::build(PlayerCityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build if already have aqueduct here
  AqueductPtr aqueveduct = terrain.getOverlay().as<Aqueduct>();
  if( aqueveduct.isValid() )
  {
    return;
  }

  Construction::build( city, pos );

  CityHelper helper( city );
  AqueductList aqueducts = helper.find<Aqueduct>( building::aqueduct );
  foreach( AqueductPtr aqueduct, aqueducts )
  {
    aqueduct->updatePicture( city );
  }

  updatePicture( city );
}

void Aqueduct::destroy()
{
  Construction::destroy();

  if( _getCity().isValid() )
  {
    TilesArray area = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 2, 2), Size( 5 ) );
    foreach( Tile* tile, area )
    {
      TileOverlayPtr overlay = tile->getOverlay();
      if( overlay.isValid() && overlay->getType() == building::aqueduct )
      {
        overlay.as<Aqueduct>()->updatePicture( _getCity() );
      }
    }
  }
}

void Aqueduct::initTerrain(Tile &terrain)
{
  bool isRoad   = terrain.getFlag( Tile::tlRoad );
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );

  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, isRoad );
  terrain.setFlag( Tile::tlMeadow, isMeadow);
  terrain.setFlag( Tile::tlAqueduct, true); // mandatory!
}

bool Aqueduct::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool is_free = Construction::canBuild( city, pos, aroundTiles );

  if( is_free )
      return true; // we try to build on free tile

  // we can place on road
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build on plazas
  if( terrain.getOverlay().as<Plaza>().isValid() )
      return false;

  // we can show that won't build over other aqueduct
  if( terrain.getOverlay().as<Aqueduct>().isValid() )
      return false;

  // also we can't build if next tile is road + aqueduct
  if ( terrain.getFlag( Tile::tlRoad ) )
  {
    TilePos tp_from = pos + TilePos (-1, -1);
    TilePos tp_to = pos + TilePos (1, 1);

    if (!tilemap.isInside(tp_from))
      tp_from = pos;

    if (!tilemap.isInside(tp_to))
      tp_to = pos;

    TilesArray perimetr = tilemap.getRectangle(tp_from, tp_to, !Tilemap::checkCorners);
    foreach( Tile* tile, perimetr )
    {
      if( tile->getFlag( Tile::tlRoad ) && tile->getFlag( Tile::tlAqueduct ) )
        return false;
    }
  }

  // and we can't build on intersections
  if ( terrain.getFlag( Tile::tlRoad ) )
  {
    int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

    TilePos tile_pos_d[countDirection];
    bool is_border[countDirection];

    tile_pos_d[north] = pos + TilePos(  0,  1);
    tile_pos_d[east]  = pos + TilePos(  1,  0);
    tile_pos_d[south] = pos + TilePos(  0, -1);
    tile_pos_d[west]  = pos + TilePos( -1,  0);

    // all tiles must be in map range
    for (int i = 0; i < countDirection; ++i) {
      is_border[i] = !tilemap.isInside(tile_pos_d[i]);
      if (is_border[i])
        tile_pos_d[i] = pos;
    }

    if (tilemap.at(tile_pos_d[north]).getFlag( Tile::tlRoad )) { directionFlags += 1; } // road to the north
    if (tilemap.at(tile_pos_d[east]).getFlag( Tile::tlRoad )) { directionFlags += 2; } // road to the east
    if (tilemap.at(tile_pos_d[south]).getFlag( Tile::tlRoad )) { directionFlags += 4; } // road to the south
    if (tilemap.at(tile_pos_d[west]).getFlag( Tile::tlRoad )) { directionFlags += 8; } // road to the west

    Logger::warning( "direction flags=%d", directionFlags );

    switch (directionFlags)
    {
    case 0:  // no road!
    case 1:  // North
    case 2:  // East
    case 4:  // South
    case 8:  // West
    case 5:  // North+South
    case 10: // East+West
      {
        Picture pic = const_cast<Aqueduct*>( this )->getPicture( city, pos, aroundTiles );
        const_cast<Aqueduct*>( this )->setPicture( pic );
      }
      return true;
    }
  }
  return false;
}

const Picture& Aqueduct::getPicture(PlayerCityPtr city, TilePos pos, const TilesArray& tmp ) const
{
  // find correct picture as for roads
  Tilemap& tmap = city->getTilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (tmp.empty()) ? getTilePos() : pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::aqueduct, 121 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection];
  bool is_busy[countDirection] = { false };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west]  = tile_pos + TilePos( -1,  0);

  // all tiles must be in map range
  for (int i = 0; i < countDirection; ++i) {
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

  // if we have a TMP array with aqueducts, calculate them
  if (!tmp.empty())
  {
    for( TilesArray::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
      int i = (*it)->getI();
      int j = (*it)->getJ();

      if( !(*it)->getOverlay().is<Aqueduct>() )
        continue;

      if (i == pos.getI() && j == (pos.getJ() + 1)) is_busy[north] = true;
      else if (i == pos.getI() && j == (pos.getJ() - 1))is_busy[south] = true;
      else if (j == pos.getJ() && i == (pos.getI() + 1))is_busy[east] = true;
      else if (j == pos.getJ() && i == (pos.getI() - 1))is_busy[west] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i) {
    if (!is_border[i] && (overlay_d[i].is<Aqueduct>() || overlay_d[i].is<Reservoir>() || is_busy[i]))
      switch (i) {
      case north: directionFlags += 1; break;
      case east:  directionFlags += 2; break;
      case south: directionFlags += 4; break;
      case west:  directionFlags += 8; break;
      default: break;
      }
  }

  int index;
  switch (directionFlags)
  {
  case 0:  // no neighbours!
    index = 121; break;
  case 1:  // N
  case 4:  // S
  case 5:  // N + S
    index = 121; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 119; 
      const_cast<Aqueduct*>( this )->_setIsRoad( true );
    }
    break;
    
  case 3:  // N + E
    index = 123; break;
  case 6:  // E + S
    index = 124; break;
  case 7:  // N + E + S
    index = 129; break;
  case 9:  // N + W
    index = 126; break;
  case 2:  // E
  case 8:  // W
  case 10: // E + W
    index = 122; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 120; 
      const_cast<Aqueduct*>( this )->_setIsRoad( true );
    }
    break;
   
  case 11: // N + E + W
    index = 132; break;
  case 12: // S + W
    index = 125;  break;
  case 13: // N + S + W
    index = 131; break;
  case 14: // E + S + W
    index = 130; break;
  case 15: // N + S + E + W (crossing)
    index = 133; break;
  default:
    index = 121; // it's impossible, but ...
  }

  return Picture::load( ResourceGroup::aqueduct, index + (_getWater() == 0 ? 15 : 0) );
}

void Aqueduct::updatePicture(PlayerCityPtr city)
{
  setPicture( getPicture( city, TilePos(), TilesArray() ) );
}

bool Aqueduct::isNeedRoadAccess() const
{
  return false;
}

void Aqueduct::_waterStateChanged()
{
  updatePicture( _getCity() );
}

void Aqueduct::addWater( const WaterSource& source )
{
  if( !_isResolved() )
  {
    _setResolved( true );
    WaterSource::addWater( source );

    const TilePos offsets[4] = { TilePos( -1, 0 ), TilePos( 0, 1), TilePos( 1, 0), TilePos( 0, -1) };
    _produceWater( offsets, 4 );
    _setResolved( false );
  }
}

bool Aqueduct::isWalkable() const
{
  return _isRoad();
}

bool Aqueduct::isRoad() const
{
  return _isRoad();
}
