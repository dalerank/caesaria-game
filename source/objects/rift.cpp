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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "rift.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "objects/aqueduct.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "walker/dustcloud.hpp"
#include "core/foreach.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::rift, Rift)

namespace{
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Rift::Rift() : TileOverlay( objects::rift, Size(1) )
{  
}

bool Rift::build( const CityAreaInfo& info )
{
  TileOverlay::build( info );
  setPicture( computePicture() );

  RiftList rifts = neighbors();
  foreach( it, rifts )
  {
    (*it)->updatePicture();
  }

  DustCloud::create( info.city, info.pos, 5 );

  return true;
}

void Rift::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRift, true );
  terrain.setFlag( Tile::tlRock, true );
}

RiftList Rift::neighbors() const
{
  RiftList ret;

  TilesArray tiles = _city()->tilemap().getNeighbors(pos(), Tilemap::FourNeighbors);

  foreach( it, tiles )
  {
    RiftPtr rt = ptr_cast<Rift>( (*it)->overlay() );
    if( rt.isValid() )
    {
      ret.push_back( rt );
    }
  }

  return ret;
}

Picture Rift::computePicture()
{
  int i = tile().i();
  int j = tile().j();

  RiftList neigs = neighbors();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
  foreach( it, neigs )
  {
    Tile* tile = &(*it)->tile();
    if (tile->j() > j)      { directionFlags += 1; } // road to the north
    else if (tile->j() < j) { directionFlags += 4; } // road to the south
    else if (tile->i() > i) { directionFlags += 2; } // road to the east
    else if (tile->i() < i) { directionFlags += 8; } // road to the west
  }

  // std::cout << "direction flags=" << directionFlags << std::endl;

  int index = 0;
  switch (directionFlags)
  {
  case 0: index = 224; break; // no RIFT!
  case 1: index = 216 + math::random( 2 ); break; // North
  case 2: index = 220 + math::random( 2 ); break; // East
  case 4: index = 218 + math::random( 2 ); break; // South
  case 3: index = 208 + math::random( 2 );  break; // North+East
  case 5: index = 200 + math::random( 4 ); break;  // North+South
  case 6: index = 210 + math::random( 2 );  break; // East+South
  case 7: index = 225; break; // North+East+South
  case 8: index = 222 + math::random( 2 ); break; // West
  case 9: index = 214 + math::random( 2 ); break; // North+West
  case 10: index = 204 + + math::random( 4 ); break;  // East+West
  case 11: index = 228; break; // North+East+West
  case 12: index = 212 + math::random( 2 ); break;  // South+West
  case 13: index = 227; break; // North+South+West
  case 14: index = 226; break; // East+South+West
  case 15: index = 229; break; // North+East+South+West
  }

  return Picture::load( ResourceGroup::land1a, index);
}

bool Rift::isWalkable() const{  return false;}
bool Rift::isFlat() const {  return true;}
void Rift::destroy() {}
bool Rift::isDestructible() const {  return false;}
Renderer::PassQueue Rift::passQueue() const {  return riftPassQueue; }

void Rift::updatePicture()
{
  setPicture( computePicture() );
}

void Rift::load(const VariantMap& stream)
{
  updatePicture();
}
