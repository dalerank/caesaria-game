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

#include "water.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "gfx/imgid.hpp"
#include "core/foreach.hpp"
#include "coast.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::water, Water)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Water::Water() : Overlay( object::water, Size(1) )
{
  setPicture( computePicture() );
}

bool Water::build( const city::AreaInfo& info )
{
  Overlay::build( info );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
  tile().setOverlay( nullptr );
  tile().setAnimation( Animation() );
  deleteLater();

  TilesArray tiles = _map().getNeighbors(pos(), Tilemap::AllNeighbors);
  for( auto tile : tiles )
  {
    bool isWater = tile->getFlag( Tile::tlWater );
    isWater |= tile->getFlag( Tile::tlDeepWater );    
    if( !isWater )
    {
      tile->setPicture( Picture::getInvalid() );
      CoastPtr ov = Overlay::create<Coast>();
      city::AreaInfo binfo( info.city, tile->epos() );
      ov->build( binfo );
    }
    bool isCoast = tile->getFlag( Tile::tlCoast );
    if( isCoast )
    {
      CoastPtr coast = tile->overlay<Coast>();
      if( coast.isValid() )
        coast->updatePicture();
      else
      {
        CoastPtr ov = Overlay::create<Coast>();
        city::AreaInfo binfo( info.city, tile->epos() );
        ov->build( binfo );
      }
    }
  }

  return true;
}

void Water::initTerrain(Tile& tile)
{
  tile.terrain().clear();
  tile.terrain().water = true;
}

Picture Water::computePicture()
{
  return randomPicture();
}

bool Water::isWalkable() const{ return false;}
bool Water::isFlat() const { return true;}
void Water::destroy() {}
bool Water::isDestructible() const { return false;}
Renderer::PassQueue Water::passQueue() const {  return riftPassQueue; }

Picture Water::randomPicture()
{
  int startOffset  = 120;
  int imgId = math::random( 7 );

  return Picture( ResourceGroup::land1a, startOffset + imgId );
}

void Water::updatePicture()
{
  setPicture( computePicture() );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
}
