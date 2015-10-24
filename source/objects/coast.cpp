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
  //deleteLater();
  //tile().setOverlay( nullptr );
  tile().setAnimation( Animation() );

  CoastList coasts = neighbors();
  for( auto nb : coasts )
    nb->updatePicture();

  return true;
}

void Coast::initTerrain(Tile& tile)
{
  tile.terrain().clear();
  tile.terrain().water = true;
  tile.terrain().coast = true;
}

Picture Coast::computePicture()
{
  return calcPicture( _map(), tile() );
}

bool Coast::isWalkable() const{ return false;}
bool Coast::isFlat() const { return true;}
void Coast::destroy() {}
bool Coast::isDestructible() const { return false;}
Renderer::PassQueue Coast::passQueue() const { return riftPassQueue; }

static bool __isWater( Tile& tile )
{
  bool isWater = tile.getFlag( Tile::tlWater ) || tile.getFlag( Tile::tlDeepWater );
  bool isCoast = tile.getFlag( Tile::tlCoast );
  return isWater && !isCoast;
}

Picture Coast::calcPicture( Tilemap& tmap, Tile& tile)
{
  TilePos mpos = tile.epos();

  bool waterN = __isWater( tmap.at( mpos.nb().north() ) );
  bool waterNE = __isWater( tmap.at( mpos.nb().northeast() ) );
  bool waterE = __isWater( tmap.at( mpos.nb().east() ) );
  bool waterSE = __isWater( tmap.at( mpos.nb().southeast() ) );
  bool waterS = __isWater( tmap.at( mpos.nb().south() ) );
  bool waterSW = __isWater( tmap.at( mpos.nb().southwest() ) );
  bool waterW = __isWater( tmap.at( mpos.nb().west() ) );
  bool waterNW = __isWater( tmap.at( mpos.nb().northwest() ) );
  bool coastN = tmap.at( mpos.nb().north() ).getFlag( Tile::tlCoast );
  bool coastNE = tmap.at( mpos.nb().northeast() ).getFlag( Tile::tlCoast );
  bool coastE = tmap.at( mpos.nb().east() ).getFlag( Tile::tlCoast );
  bool coastW = tmap.at( mpos.nb().west() ).getFlag( Tile::tlCoast );
  bool coastSW = tmap.at( mpos.nb().southwest() ).getFlag( Tile::tlCoast );

  int start = 0;
  int size = 0;
  //if( waterN && waterNE && waterNW)  {    start = 128;    size = 4;  }
  /*else if( waterN && waterNE && waterE ) { start = 144; size = 4; }
  else if( waterNE && waterE && waterSE) { start = 132; size = 4; }
  else if( waterE && waterSE && waterS ) { start = 148; size = 4; }
  else if( waterSE && waterS && waterSW) { start = 136; size = 4; }
  else if( waterS && waterSW && waterW ) { start = 152; size = 4; }
  else if( waterSW && waterW && waterNW ) { start = 140; size = 4; }*/
  //else

  if( waterW && waterNW && waterSW )
  {
    start = 140; size = 4;
    if( waterN || (coastN && coastNE && coastE) ) { start = 173; size = 1; }
    else if( waterS ) { start = 172; size = 1; }
  }
  else if( waterE && waterNE && waterSE )
  {
    start = 132; size = 4;
    if( waterN ) { start = 170; size = 1; }
    else if( waterS ) { start = 171; size = 1; }
  }
  else if( waterSW && waterS ) { start = 136; size = 4; }
  else if( waterNW )   {    start = 156; size = 4;  }
  else if( waterSW )   {    start = 152; size = 4;  }
  else if( waterNE )   {    start = 144; size = 4;  }
  else if( waterSE )   {    start = 148; size = 4;  }
  else if( coastN && coastNE && coastW && coastSW ) { start = 156; size = 4; }

  return Picture( ResourceGroup::land1a, start + math::random(size-1) );
}

void Coast::updatePicture()
{
  setPicture( computePicture() );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
}

CoastList Coast::neighbors() const
{
  return _map().getNeighbors(pos(), Tilemap::AllNeighbors)
               .overlays<Coast>();
}
