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

#include "terrain.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "gfx/helper.hpp"
#include "core/foreach.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::terrain, Terrain)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Terrain::Terrain() : Overlay( object::terrain, Size(1) )
{
  setPicture( computePicture() );
}

bool Terrain::build( const city::AreaInfo& info )
{
  Overlay::build( info );
  tile().setPicture( picture() );
  deleteLater();

  return true;
}

void Terrain::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlWater, true );
}

Picture Terrain::computePicture()
{
  int startOffset  = ( (math::random( 10 ) > 6) ? 62 : 232 );
  int imgId = math::random( 58-1 );

  return Picture( ResourceGroup::land1a, startOffset + imgId );
}

bool Terrain::isWalkable() const{ return true;}
bool Terrain::isFlat() const { return true;}
void Terrain::destroy() {}
bool Terrain::isDestructible() const { return false;}
Renderer::PassQueue Terrain::passQueue() const {  return riftPassQueue; }

void Terrain::updatePicture()
{
  setPicture( computePicture() );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
}
