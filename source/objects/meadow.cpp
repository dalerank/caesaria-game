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

#include "meadow.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"
#include "gfx/animation_bank.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::meadow, Meadow)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Meadow::Meadow() : Overlay( object::meadow, Size(1,1) )
{
  setPicture( info().randomPicture( Size(1,1) ) ); // 110 111 112 113
}

Meadow::~Meadow() {}

bool Meadow::build( const city::AreaInfo& info )
{
  Overlay::build( info );

  const Animation& meadow = AnimationBank::simple( AnimationBank::animMeadow );
  Animation meadowAnim;
  int index = math::random( meadow.size()-1 );
  meadowAnim.addFrame( meadow.frame( index ) );

  tile().setAnimation( meadowAnim );
  tile().setOverlay( nullptr );

  deleteLater();

  return true;
}

void Meadow::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlMeadow, true );
}

TilesArray Meadow::neighbors() const
{
  return _map().getNeighbors(pos(), Tilemap::AllNeighbors)
               .select( Tile::tlMeadow );
}

bool Meadow::isWalkable() const{ return true;}
bool Meadow::isFlat() const { return true;}
bool Meadow::isDestructible() const { return true;}
Renderer::PassQueue Meadow::passQueue() const {  return riftPassQueue; }
