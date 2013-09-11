// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_scenario_load_finalizer.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_positioni.hpp"
#include "oc3_tile.hpp"
#include "oc3_picture.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_city.hpp"
#include "oc3_foreach.hpp"
#include "oc3_animation_bank.hpp"
#include "oc3_resourcegroup.hpp"

static void initEntryExitTile( const TilePos& tlPos, Tilemap& tileMap, const unsigned int picIdStart, bool exit )
{
  unsigned int idOffset = 0;
  TilePos tlOffset;
  if( tlPos.getI() == 0 || tlPos.getI() == tileMap.getSize() - 1 )
  {
    tlOffset = TilePos( 0, 1 );
    idOffset = (tlPos.getI() == 0 ? 1 : 3 );

  }
  else if( tlPos.getJ() == 0 || tlPos.getJ() == tileMap.getSize() - 1 )
  {
    tlOffset = TilePos( 1, 0 );
    idOffset = (tlPos.getJ() == 0 ? 2 : 0 );
  }

  Tile& signTile = tileMap.at( tlPos + tlOffset );

  StringHelper::debug( 0xff, "(%d, %d)", tlPos.getI(),    tlPos.getJ()    );
  StringHelper::debug( 0xff, "(%d, %d)", tlOffset.getI(), tlOffset.getJ() );

  signTile.setPicture( &Picture::load( ResourceGroup::land3a, picIdStart + idOffset ) );
  signTile.getTerrain().setRock( true );
}

static void initWaterTileAnimation( Tilemap& tmap )
{
  TilemapArea area = tmap.getFilledRectangle( TilePos( 0, 0 ), Size( tmap.getSize() ) );

  Animation water;
  water.setFrameDelay( 12 );
  water.load( ResourceGroup::land1a, 121, 7 );
  water.load( ResourceGroup::land1a, 127, 7, true );
  foreach( Tile* tile, area )
  {
    int rId = tile->getTerrain().getOriginalImgId() - 364;
    if( rId >= 0 && rId < 8 )
    {
      water.setCurrentIndex( rId );
      tile->setAnimation( water );
    }
  }
}

void ScenarioLoadFinalizer::finalize( Scenario& scenario )
{
  Tilemap& tileMap = scenario.getCity()->getTilemap();

  // exit and entry can't point to one tile or .... can!
  initEntryExitTile( scenario.getCity()->getRoadEntry(), tileMap, 89, false );
  initEntryExitTile( scenario.getCity()->getRoadExit(),  tileMap, 85, true  );
  initWaterTileAnimation( tileMap );
}
