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

#include "river.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"
#include "objects_factory.hpp"
#include "gfx/imgid.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::river, River)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

River::River()
  : Overlay( object::river, Size(1) )
{
  setPicture( config::rc.land1a, 182 );
}

bool River::build( const city::AreaInfo& info )
{
  Overlay::build( info );
  updatePicture( info );

  RiverList rivers = neighbors();
  for( auto tile : rivers )
    tile->updatePicture( info );

  return true;
}

void River::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlWater, true );
}

RiverList River::neighbors() const
{
  return _map().getNeighbors(pos(), Tilemap::FourNeighbors)
               .overlays<River>();
}

const Picture& River::picture( const city::AreaInfo& info ) const
{
  const TilePos tile_pos = info.tiles().empty() ? tile().epos() : info.pos;

  int i = tile_pos.i();
  int j = tile_pos.j();

  RiverList neigs = neighbors();

  int directions = 0;  // bit field, N=1, E=2, S=4, W=8
  for( auto riverTile : neigs )
  {
    Tile* tile = &riverTile->tile();
    if (tile->j() > j)      { directions |= 1; } // road to the north
    else if (tile->j() < j) { directions |= 4; } // road to the south
    else if (tile->i() > i) { directions |= 2; } // road to the east
    else if (tile->i() < i) { directions |= 8; } // road to the west
  }

  int advDirections = 0;
  bool haveAdvTiles = !info.tiles().empty();
  if (haveAdvTiles)
  {
    const TilePos& p = tile_pos;
    for( auto tile : info.tiles() )
    {
      int i = tile->epos().i();
      int j = tile->epos().j();

      if( !tile->overlay().is<River>() )
        continue;

      if( i == p.i() && j == (p.j() + 1)) advDirections |= 1;
      else if (i == p.i() && j == (p.j() - 1)) advDirections |= 4;
      else if (j == p.j() && i == (p.i() + 1)) advDirections |= 2;
      else if (j == p.j() && i == (p.i() - 1)) advDirections |= 8;
    }
  }

  if( advDirections > 0 )
    directions |= advDirections;

  // std::cout << "direction flags=" << directionFlags << std::endl;

  int index = 0;
  if( tile().getFlag( Tile::tlCoast ) )
  {
    switch (directions)
    {
    case 1: index = 175; break;
    case 2: index = 176; break;
    case 4: index = 177; break;
    case 8: index = 174; break;
    }
  }
  else
  {
    switch (directions)
    {
    case 0: index = 199; break; // no River!
    case 1: index = 164; break; // North
    case 2: index = 165; break; // East
    case 3: index = 1188;  break; // North+East
    case 4: index = 166; break; // South
    case 5: index = 162 + (i+j)%2; break;  // North+South
    case 6: index = 1198;  break; // East+South
    case 7: index = 188; break; // North+East+South
    case 8: index = 167; break; // West
    case 9: index = 198; break; // North+West
    case 0xa: index = 160 + (i+j)%2; break;  // East+West
    case 0xb: index = 185; break; // North+East+West
    case 0xc: index = 1194; break;  // South+West
    case 0xd: index = 194; break; // North+South+West
    case 0xe: index = 191; break; // East+South+West
    case 0xf: index = 182; break; // North+East+South+West
    }
  }

  static Picture ret;
  ret.load( config::rc.land1a, index);

  return ret;
}

bool River::isWalkable() const{ return false;}
bool River::isFlat() const { return true;}
void River::destroy() {}
bool River::isDestructible() const { return false;}
Renderer::PassQueue River::passQueue() const {  return riftPassQueue; }

void River::updatePicture(const city::AreaInfo& info)
{
  auto texture = picture( info );
  setPicture( texture );
  tile().setPicture( texture );
  tile().setImgId( imgid::fromResource( texture.name() ) );
}

void River::load(const VariantMap& stream)
{
  city::AreaInfo info( _city(), pos() );
  updatePicture( info );
}
