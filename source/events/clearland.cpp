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

#include "clearland.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "warningmessage.hpp"
#include "core/gettext.hpp"
#include "gfx/imgid.hpp"
#include "requestdestroy.hpp"
#include "objects/construction.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

namespace events
{

GameEventPtr ClearTile::create(const TilePos& pos)
{
  ClearTile* ev = new ClearTile();
  ev->_pos = pos;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

bool ClearTile::_mayExec(Game& game, unsigned int) const{ return true; }

void ClearTile::_exec( Game& game, unsigned int )
{
  Tilemap& tmap = game.city()->tilemap();

  Tile& cursorTile = tmap.at( _pos );

  if( cursorTile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );
    TilePos rPos = _pos;

    OverlayPtr overlay = cursorTile.overlay();

    bool deleteRoad = cursorTile.getFlag( Tile::tlRoad );

    if( overlay.isValid() && !overlay->canDestroy() )
    {
      auto info = overlay->info();
      events::dispatch<WarningMessage>( _( overlay->errorDesc() ), WarningMessage::neitral );

      if( info.requestDestroy() )
      {
        events::dispatch<RequestDestroy>( overlay );
      }
      return;
    }

    if( overlay.isValid() )
    {
      size = overlay->size();
      rPos = overlay->pos();
      overlay->deleteLater();
    }

    TilesArray clearedTiles = tmap.area( rPos, size );
    for( auto tile : clearedTiles )
    {
      tile->setMaster( NULL );
      tile->terrain().tree = false;
      tile->terrain().road = false;
      tile->terrain().garden = false;
      tile->setOverlay( NULL );

      deleteRoad |= tile->getFlag( Tile::tlRoad );

      if( tile->getFlag( Tile::tlMeadow ) || tile->getFlag( Tile::tlWater ) )
      {
        //tile->setPicture( imgid::toResource( tile->originalImgId() ) );
      }
      else
      {
        // choose a random landscape picture:
        // flat land1a 2-9;
        // wheat: land1a 18-29;
        // green_something: land1a 62-119;  => 58
        // green_flat: land1a 232-289; => 58

        // choose a random background image, green_something 62-119 or green_flat 232-240
        // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (math::random( 10 ) > 6) ? 62 : 232 );
        int imgId = math::random( 58-1 );

        Picture pic( config::rc.land1a, startOffset + imgId );
        tile->setPicture( pic );
        tile->setImgId( imgid::fromResource( pic.name() ) );
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    //
    if( deleteRoad )
    {
      game.city()->setOption( PlayerCity::updateRoadsOnNextFrame, 1 );
    }
  }

  game.city()->setOption( PlayerCity::updateTiles, 1 );
}

} //end namespace events
