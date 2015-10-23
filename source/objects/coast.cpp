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

#include "coast.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "gfx/helper.hpp"
#include "core/foreach.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::coast, Coast)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Coast::Coast() : Overlay( object::terrain, Size(1) )
{
  setPicture( ResourceGroup::land1a, 199 );
}

bool Coast::build( const city::AreaInfo& info )
{
  Overlay::build( info );
  updatePicture();

  return true;
}

void Coast::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlWater, true );
  terrain.setFlag( Tile::tlCoast, true );
}

Picture Coast::computePicture()
{
<<<<<<< HEAD
  TilePos mpos = tile().epos();
  bool northnb = _map().at( mpos.northnb() ).getFlag( Tile::tlCoast );
  bool eastnb = _map().at( mpos.eastnb() ).getFlag( Tile::tlCoast );
  bool westnb = _map().at( mpos.westnb() ).getFlag( Tile::tlCoast );
  bool southnb = _map().at( mpos.southnb() ).getFlag( Tile::tlCoast );
=======


  int startOffset  = ( (math::random( 10 ) > 6) ? 62 : 232 );
  int imgId = math::random( 58-1 );
>>>>>>> 697ff34048b4300c94947b6eb06b091c1b885da3

  int start = 0;
  int size = 0;
  if( northnb && eastnb ) { start = 144; size = 4; }
  else if( northnb && southnb )
  {
    start = 132;
    size = 4;
    if( westnb && !eastnb ) start = 140;
  }
  else if( northnb && westnb ) { start = 156; size = 4; }
  else if( westnb && eastnb )
  {
    start = 128;
    size = 4;
    if( southnb && !northnb ) start = 136;
  }
  else if( eastnb && southnb ) { start = 148; size = 4; }
  else if( westnb && southnb ) { start = 152; size = 4; }

  return Picture( ResourceGroup::land1a, start + math::random(size-1) );
}

bool Coast::isWalkable() const{ return false;}
<<<<<<< HEAD
bool Coast::isFlat() const { return true; }
=======
bool Coast::isFlat() const { return true;}
>>>>>>> 697ff34048b4300c94947b6eb06b091c1b885da3
void Coast::destroy() {}
bool Coast::isDestructible() const { return false;}
Renderer::PassQueue Coast::passQueue() const { return riftPassQueue; }

void Coast::updatePicture()
{
  setPicture( computePicture() );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
}
