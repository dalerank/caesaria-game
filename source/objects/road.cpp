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

#include "road.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "objects/aqueduct.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

namespace {
static Renderer::PassQueue roadPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Road::Road()
  : Construction( construction::road, Size(1) )
{
  _paved = 0;
}

void Road::build( PlayerCityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->tilemap();
  TileOverlayPtr overlay = tilemap.at( pos ).overlay();

  if( is_kind_of<Road>( overlay ) )
  {
    return;
  }

  Construction::build( city, pos );

  if( is_kind_of<Aqueduct>( overlay ) )
  {
    overlay->build( city, pos );
    return;
  }

  city->setOption( PlayerCity::updateRoads, 1 );
}

bool Road::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool is_free = Construction::canBuild( city, pos, aroundTiles );

  if( is_free )
    return true; // we try to build on free tile

  TileOverlayPtr overlay  = city->tilemap().at( pos ).overlay();

  Picture pic = picture( city, pos, aroundTiles );
  const_cast<Road*>( this )->setPicture( pic );

  return ( is_kind_of<Aqueduct>( overlay ) || is_kind_of<Road>( overlay ) );
}


void Road::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::tlRoad, true );
}

const gfx::Picture& Road::picture( PlayerCityPtr city, TilePos p, const gfx::TilesArray& tmp ) const
{
  int i = p.i();
  int j = p.j();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
  if (!tmp.empty())
  {
    foreach( it, tmp )
    {
      int i = (*it)->i();
      int j = (*it)->j();

      if( (*it)->getFlag( Tile::tlRoad ) ||
          is_kind_of<Road>( (*it)->overlay() ) )
      {

        if( i == p.i() && j == (p.j() + 1)) directionFlags |= 0x1; // road to the north
        else if (i == p.i() && j == (p.j() - 1)) directionFlags |= 0x4; // road to the south
        else if (j == p.j() && i == (p.i() + 1)) directionFlags |= 0x2; // road to the east
        else if (j == p.j() && i == (p.i() - 1)) directionFlags |= 0x8; // road to the west
      }
    }
  }

  TilesArray roads;
  if( city.isValid() )
    roads = city->tilemap().getNeighbors( p, Tilemap::FourNeighbors );

  foreach( it, roads )
  {
    Tile* tile = *it;
    if( tile->getFlag( Tile::tlRoad ) ||
        is_kind_of<Road>( tile->overlay() ) )
    {
      if (tile->j() > j )      { directionFlags |= 0x1; } // road to the north
      else if (tile->j() < j ) { directionFlags |= 0x4; } // road to the south
      else if (tile->i() > i ) { directionFlags |= 0x2; } // road to the east
      else if (tile->i() < i ) { directionFlags |= 0x8; } // road to the west
    }
  }

  // std::cout << "direction flags=" << directionFlags << std::endl;

  int index = 0;
  if( _paved == 0 )
  {
    switch (directionFlags)
    {
    case 0: index = 101; break; // no road!
    case 1: index = 101; break; // North
    case 2: index = 102; break; // East
    case 4: index = 103; break; // South
    case 8: index = 104; break; // West
    case 3: index = 97;  break; // North+East
    case 5: index = 93+2*((p.i() + p.j()) % 2); break;  // 93/95 // North+South
    case 6: index = 98;  break; // East+South
    case 7: index = 106; break; // North+East+South
    case 9: index = 100; break; // North+West
    case 10: index = 94+2*((p.i() + p.j())%2); break;  // 94/96 // East+West
    case 11: index = 109; break; // North+East+West
    case 12: index = 99; break;  // South+West
    case 13: index = 108; break; // North+South+West
    case 14: index = 107; break; // East+South+West
    case 15: index = 110; break; // North+East+South+West
    }
  }
  else
  {
    switch (directionFlags)
    {
    case 0: index = 52; break; // no road!
    case 1: index = 52+4*((p.i() + p.j())%2); break; // North
    case 2: index = 53; break; // East
    case 4: index = 54; break; // South
    case 8: index = 55; break; // West
    case 3: index = 48;  break; // North+East
    case 5: index = 44+2*((p.i() + p.j())%2); break;  // 93/95 // North+South
    case 6: index = 49;  break; // East+South
    case 9: index = 51; break; // North+West
    case 10: index = 45+2*((p.i() + p.j())%2); break;  // 94/96 // East+West
    case 12: index = 50; break;  // South+West

    case 7:
    case 11:
    case 13:
    case 14:
    case 15:
      index = 78 + (p.i() + p.j()) % 14;
    break;
    }
  }

  return Picture::load( ResourceGroup::road, index);
}

bool Road::isWalkable() const {  return true;}
bool Road::isFlat() const{  return true;}
void Road::updatePicture(){  setPicture( picture( _city(), _masterTile() ? _masterTile()->pos() : TilePos(), TilesArray() ) );}
bool Road::isNeedRoadAccess() const {  return false; }

void Road::destroy()
{
  city::Helper helper( _city() );
  TilesArray tiles = helper.getArea( this );

  foreach( it, tiles )
  {
    (*it)->setFlag( Tile::tlRoad, false );
  }
}

void Road::burn() {}

void Road::appendPaved( int value )
{
  bool saveValue = _paved > 0;
  _paved = math::clamp( _paved += value, 0, 4 );

  if( saveValue != (_paved > 0) )
  {
    updatePicture();
  }
}

void Road::computeAccessRoads()
{
  Construction::computeAccessRoads();
  updatePicture();
}

int Road::pavedValue() const {  return _paved; }
Renderer::PassQueue Road::passQueue() const{  return roadPassQueue;}

void Road::save(VariantMap& stream) const
{
  Construction::save( stream );

  stream[ "paved" ] = _paved;
}

void Road::load(const VariantMap& stream)
{
  Construction::load( stream );
  _paved  = (int)stream.get( "paved", 0 );

  updatePicture();
}

// I didn't decide what is the best approach: make Plaza as constructions or as upgrade to roads
Plaza::Plaza()
{
  // somewhere we need to delete original road and then we need to think
  // because as we remove original road we need to recompute adjacent tiles
  // or we will run into big troubles

  setType(construction::plaza);
  setPicture( Picture::load( ResourceGroup::entertaiment, 102) ); // 102 ~ 107
  setSize( Size( 1 ) );
}

// Plazas can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)

bool Plaza::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  //std::cout << "Plaza::canBuild" << std::endl;
  Tilemap& tilemap = city->tilemap();

  bool is_constructible = true;

  TilesArray area = tilemap.getArea( pos, size() ); // something very complex ???
  foreach( tile, area )
  {
    is_constructible &= (*tile)->getFlag( Tile::tlRoad );
  }

  return is_constructible;
}

void Plaza::appendPaved(int) {}

void Plaza::build(PlayerCityPtr city, const TilePos& p)
{
  Tilemap& tilemap = city->tilemap();
  TileOverlayPtr overlay = tilemap.at( p ).overlay();

  if( !is_kind_of<Road>( overlay ) )
  {
    return;
  }

  Construction::build( city, p );
  setPicture( MetaDataHolder::randomPicture( type(), size() ) );

  if( size().area() == 1 )
  {
    TilesArray tilesAround = city->tilemap().getNeighbors( pos(), Tilemap::AllNeighbors);
    foreach( tile, tilesAround )
    {
      PlazaPtr plaza = ptr_cast<Plaza>( (*tile)->overlay() );
      if( plaza.isValid() )
      {
        plaza->updatePicture();
      }
    }
  }
}

void Plaza::save(VariantMap& stream) const
{
  Road::save( stream );

  stream[ "picture" ] = Variant( picture().name() );
}

void Plaza::load(const VariantMap& stream)
{
  Road::load( stream );

  if( size().area() > 1 )
  {
    Construction::build( _city(), pos() );
  }

  setPicture( Picture::load( stream.get( "picture" ).toString() ) );
}

void Plaza::updatePicture()
{
  TilesArray nearTiles = _city()->tilemap().getArea( pos(), Size(2) );

  bool canGrow2squarePlaza = ( nearTiles.size() == 4 ); // be carefull on map edges
  foreach( tile, nearTiles )
  {
    PlazaPtr garden = ptr_cast<Plaza>( (*tile)->overlay() );
    canGrow2squarePlaza &= (garden.isValid() && garden->size().area() <= 2 );
  }

  if( canGrow2squarePlaza )
  {
    nearTiles.remove( pos() );
    foreach( tile, nearTiles )
    {
      if( (*tile)->overlay().isValid() )
      {
        (*tile)->overlay()->deleteLater();
      }
    }

    city::Helper helper( _city() );
    helper.updateDesirability( this, city::Helper::offDesirability );
    setSize( 2 );
    Construction::build( _city(), pos() );
    setPicture( MetaDataHolder::randomPicture( type(), size() ) );
    helper.updateDesirability( this, city::Helper::onDesirability );
  }
}
