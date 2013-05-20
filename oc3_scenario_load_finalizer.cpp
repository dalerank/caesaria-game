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

  signTile.set_picture(&Picture::load( "land3a", picIdStart + idOffset ));
  signTile.get_terrain().setRock( true );
};

void ScenarioLoadFinalizer::check()
{
  Tilemap& tileMap = _scenario.getCity().getTilemap();

  // exit and entry can't point to one tile or .... can!
  initEntryExitTile( _scenario.getCity().getRoadEntry(), tileMap, 89, false );
  initEntryExitTile( _scenario.getCity().getRoadExit(),  tileMap, 85, true  ); 
}

ScenarioLoadFinalizer::ScenarioLoadFinalizer( Scenario& scenario )
: _scenario( scenario )
{

}
