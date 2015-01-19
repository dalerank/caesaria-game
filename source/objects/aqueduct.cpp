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

#include "aqueduct.hpp"

#include "core/utils.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "constants.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "objects/predefinitions.hpp"
#include "objects/road.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::aqueduct, Aqueduct)

Aqueduct::Aqueduct() : WaterSource( objects::aqueduct, Size(1) )
{
  setPicture( ResourceGroup::aqueduct, 133 ); // default picture for aqueduct
  _setIsRoad( false );
  // land2a 119 120         - aqueduct over road
  // land2a 121 122         - aqueduct over plain ground
  // land2a 123 124 125 126 - aqueduct corner
  // land2a 127 128         - aqueduct over dirty roads
  // land2a 129 130 131 132 - aqueduct T-shape crossing
  // land2a 133             - aqueduct crossing
  // land2a 134 - 148       - aqueduct without water
}

bool Aqueduct::build( const CityAreaInfo& info )
{
  Tilemap& tilemap = info.city->tilemap();
  Tile& terrain = tilemap.at( info.pos );

  // we can't build if already have aqueduct here
  AqueductPtr aqueveduct = ptr_cast<Aqueduct>( terrain.overlay() );
  if( aqueveduct.isValid() )
  {
    return false;
  }

  _setIsRoad( terrain.getFlag( Tile::tlRoad ) );
  RoadPtr road = ptr_cast<Road>( terrain.overlay() );
  if( road.isValid() )
  {
    road->setState( (Construction::Param)Road::lockTerrain, 1 );
  }

  WaterSource::build( info );

  city::Helper helper( info.city );
  TilePos offset( 2, 2 );
  AqueductList aqueducts = helper.find<Aqueduct>( objects::aqueduct, info.pos - offset, info.pos + offset );

  foreach( aqueduct, aqueducts ) { (*aqueduct)->updatePicture( info.city ); }
  return true;
}

void Aqueduct::addWater(const WaterSource &source)
{
  WaterSource::addWater( source );

  const TilePos offsets[4] = { TilePos( -1, 0 ), TilePos( 0, 1), TilePos( 1, 0), TilePos( 0, -1) };
  _produceWater( offsets, 4 );
}

void Aqueduct::initTerrain(Tile& terrain) {}

void Aqueduct::destroy()
{
  Construction::destroy();

  if( _city().isValid() )
  {
    TilesArray area = _city()->tilemap().getArea( pos() - TilePos( 2, 2 ), Size( 5 ) );
    foreach( tile, area )
    {
      AqueductPtr aq = ptr_cast<Aqueduct>( (*tile)->overlay() );
      if( aq.isValid() )
      {
        aq->updatePicture( _city() );
      }
    }
  }

  if( tile().getFlag( Tile::tlRoad ) || _isRoad() )
  {
    RoadPtr r( new Road() );
    r->drop();

    CityAreaInfo info = { _city(), pos(), TilesArray() };
    r->build( info );
    _city()->addOverlay( ptr_cast<TileOverlay>( r ) );
  }
}

void Aqueduct::timeStep(const unsigned long time)
{
  WaterSource::timeStep( time );
}

bool Aqueduct::canBuild( const CityAreaInfo& areaInfo) const
{
  bool is_free = Construction::canBuild( areaInfo );

  if( is_free )
      return true; // we try to build on free tile

  // we can place on road
  Tilemap& tilemap = areaInfo.city->tilemap();
  Tile& terrain = tilemap.at( areaInfo.pos );

  // we can't build on plazas
  if( is_kind_of<Plaza>( terrain.overlay() ) )
      return false;

  // we can show that won't build over other aqueduct
  if( is_kind_of<Aqueduct>( terrain.overlay() ) )
      return false;

  // also we can't build if next tile is road + aqueduct
  if( terrain.getFlag( Tile::tlRoad ) )
  {
    TilePos tp_from = areaInfo.pos + TilePos( -1, -1 );
    TilePos tp_to = areaInfo.pos + TilePos( 1, 1 );

    if (!tilemap.isInside(tp_from))
      tp_from = areaInfo.pos;

    if (!tilemap.isInside(tp_to))
      tp_to = areaInfo.pos;

    TilesArray perimetr = tilemap.getRectangle(tp_from, tp_to, !Tilemap::checkCorners);
    foreach( tile, perimetr )
    {
      AqueductPtr bldAqueduct;
      foreach( it, areaInfo.aroundTiles )
      {
        if( (*it)->pos() == (*tile)->pos() )
        {
          bldAqueduct = ptr_cast< Aqueduct >( (*it)->overlay() );
          break;
        }
      }

      if( (*tile)->getFlag( Tile::tlRoad ) && bldAqueduct.isValid()  )
        return false;
    }
  }

  // and we can't build on intersections
  if ( terrain.getFlag( Tile::tlRoad ) )
  {
    bool canBuildWithRoad = canAddRoad( areaInfo.city, areaInfo.pos );
    if( canBuildWithRoad )
    {
      Picture pic = picture( areaInfo );
      const_cast<Aqueduct*>( this )->setPicture( pic );
      return true;
    }
  }

  return false;
}

const Picture& Aqueduct::picture( const CityAreaInfo& info ) const
{
  // find correct picture as for roads
  Tilemap& tmap = info.city->tilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (info.aroundTiles.empty()) ? tile().epos() : info.pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::aqueduct, 121 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection];
  bool is_busy[countDirection] = { false };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east ]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west ]  = tile_pos + TilePos( -1,  0);

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
  overlay_d[east ] = tmap.at( tile_pos_d[east]  ).overlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).overlay();
  overlay_d[west ] = tmap.at( tile_pos_d[west]  ).overlay();

  // if we have a TMP array with aqueducts, calculate them
  const TilePos& p = info.pos;
  if (!info.aroundTiles.empty())
  {
    foreach( it, info.aroundTiles )
    {
      int i = (*it)->epos().i();
      int j = (*it)->epos().j();

      if( !is_kind_of<Aqueduct>( (*it)->overlay() ) )
        continue;

      if( i == p.i() && j == (p.j() + 1)) is_busy[north] = true;
      else if (i == p.i() && j == (p.j() - 1))is_busy[south] = true;
      else if (j == p.j() && i == (p.i() + 1))is_busy[east] = true;
      else if (j == p.j() && i == (p.i() - 1))is_busy[west] = true;
    }    
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i)
  {
    bool isReservoirNear = is_kind_of<Reservoir>( overlay_d[i] );
    if( !is_border[i] && (is_kind_of<Aqueduct>( overlay_d[i] ) || isReservoirNear || is_busy[i] ) )
    {
      if( isReservoirNear )
      {
        ReservoirPtr reservoir = ptr_cast<Reservoir>( overlay_d[ i ] );
        switch( i )
        {
        case north: directionFlags += ( reservoir->entry( south ) == p + TilePos( 0, 1 ) ? 1 : 0 ); break;
        case east:  directionFlags += ( reservoir->entry( west ) == p + TilePos( 1, 0 ) ? 2 : 0 ); break;
        case south: directionFlags += ( reservoir->entry( north ) == p + TilePos( 0, -1 ) ? 4 : 0 ); break;
        case west:  directionFlags += ( reservoir->entry( east ) == p + TilePos( -1, 0 ) ? 8 : 0 ); break;
        default: break;
        }
      }
      else
      {
        switch( i )
        {
        case north: directionFlags += 1; break;
        case east:  directionFlags += 2; break;
        case south: directionFlags += 4; break;
        case west:  directionFlags += 8; break;
        default: break;
        }
      }
    }
  }

  int index;
  Direction mapDirection = info.city->tilemap().direction();

  switch (directionFlags)
  {
  case 0:  // no neighbours!
  {
    index = 121;
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      const_cast<Aqueduct*>( this )->_setIsRoad( true );

      RoadPtr rwest  = ptr_cast<Road>( info.city->getOverlay( tile_pos + TilePos( -1, 0 ) ) );
      RoadPtr rnorth = ptr_cast<Road>( info.city->getOverlay( tile_pos + TilePos( 0, 1 ) ) );
      RoadPtr reast  = ptr_cast<Road>( info.city->getOverlay( tile_pos + TilePos( 1, 0 ) ) );
      RoadPtr rsouth = ptr_cast<Road>( info.city->getOverlay( tile_pos + TilePos( 0, -1 ) ));

      if( rwest != NULL || reast != NULL )
      {
        index = 127;
      }
      else if( rsouth != NULL || rnorth != NULL )
      {
        index = 128;
      }           
    }
    else
    {
      index += ( mapDirection == west || mapDirection == east ) ? 1 : 0;
    }
  }
  break;

  case 1:  // N
  case 4:  // S
  case 5:  // N + S
  {
    index = 121; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 119; 
      const_cast<Aqueduct*>( this )->_setIsRoad( true );
    }
    //index += ( mapDirection == west || mapDirection == east ) ? 1 : 0;
  }
  break;
    
  case 3:  // N + E
    index = 123; break;

  case 6:  // E + S
    index = 124; break;
  case 7:  // N + E + S
    index = 129;
  break;

  case 9:  // N + W
    index = 126; break;
  case 2:  // E
  case 8:  // W
  case 10: // E + W
  {
    index = 122; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 120; 
      const_cast<Aqueduct*>( this )->_setIsRoad( true );
    }
    //index -= ( mapDirection == west || mapDirection == east ) ? 1 : 0;
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

  const Picture& pic = Picture::load( ResourceGroup::aqueduct, index + (water() == 0 ? 15 : 0) );
  return pic;
}

void Aqueduct::updatePicture(PlayerCityPtr city)
{
  CityAreaInfo info = { city, _masterTile() ? _masterTile()->pos() : TilePos(), TilesArray() };
  setPicture( picture( info ) );
}

void Aqueduct::addRoad()
{
  bool canBuildWithRoad = canAddRoad( _city(), pos() );
  if( canBuildWithRoad )
  {
    tile().setFlag( Tile::tlRoad, true );
    updatePicture( _city() );
  }
}

bool Aqueduct::canAddRoad( PlayerCityPtr city, TilePos pos) const
{
  if( !city.isValid() )
    city = _city();

  if( !city.isValid() )
    return false;

  Tilemap& tilemap = city->tilemap();
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
    return true;
  }

  return false;
}

bool Aqueduct::isNeedRoadAccess() const {  return false; }
void Aqueduct::_waterStateChanged(){  updatePicture( _city() ); }
bool Aqueduct::isWalkable() const {  return _isRoad(); }

void Aqueduct::changeDirection(Tile* masterTile, Direction direction)
{
  Construction::changeDirection( masterTile, direction );
  updatePicture( _city() );
}

std::string Aqueduct::sound() const
{
  return ( water() == 0 ? "" : WaterSource::sound() );
}

const Picture& Aqueduct::picture() const
{
  return WaterSource::picture();
}
