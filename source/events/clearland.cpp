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
#include "gfx/helper.hpp"
#include "requestdestroy.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;
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

    TileOverlayPtr overlay = cursorTile.overlay();

    bool deleteRoad = cursorTile.getFlag( Tile::tlRoad );

    ConstructionPtr constr = ptr_cast<Construction>(overlay);
    if( constr.isValid() && !constr->canDestroy() )
    {
      GameEventPtr e = WarningMessage::create( _( constr->errorDesc() ) );
      e->dispatch();

      const MetaData& md = MetaDataHolder::getData( constr->type() );
      if( md.getOption( MetaDataOptions::requestDestroy, false ).toBool() )
      {
        e = RequestDestroy::create( constr );
        e->dispatch();
      }
      return;
    }

    if( overlay.isValid() )
    {
      size = overlay->size();
      rPos = overlay->pos();
      overlay->deleteLater();
    }

    TilesArray clearedTiles = tmap.getArea( rPos, size );
    foreach( it, clearedTiles )
    {
      Tile* tile = *it;
      tile->setMasterTile( NULL );
      tile->setFlag( Tile::tlTree, false);
      tile->setFlag( Tile::tlRoad, false);
      tile->setFlag( Tile::tlGarden, false);
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
        int imgId = math::random( 58 );

        Picture pic = Picture::load( ResourceGroup::land1a, startOffset + imgId );
        tile->setPicture( ResourceGroup::land1a, startOffset + imgId );
        tile->setOriginalImgId( imgid::fromResource( pic.name() ) );
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    //
    if( deleteRoad )
    {
      game.city()->setOption( PlayerCity::updateRoads, 1 );
    }
  }

  game.city()->setOption( PlayerCity::updateTiles, 1 );
}

} //end namespace events
