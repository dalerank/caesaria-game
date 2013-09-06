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

#include "oc3_scenario_event.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_scenario.hpp"
#include "oc3_city.hpp"
#include "oc3_gettext.hpp"

void DisasterEvent::create( const TilePos& pos, Type type )
{
  DisasterEvent* event = new DisasterEvent();
  event->_pos = pos;
  event->_type = type;

  ScenarioEventPtr ret( event );
  ret->drop();

  Scenario::instance().addEvent( ret );
}

void DisasterEvent::exec( CityPtr city )
{
  Tilemap& tmap = city->getTilemap();
  TerrainTile& terrain = tmap.at( _pos ).getTerrain();
  TilePos rPos = _pos;

  if( terrain.isDestructible() )
  {
    Size size( 1 );

    LandOverlayPtr overlay = terrain.getOverlay();
    if( overlay.isValid() )
    {
      overlay->deleteLater();
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
    }

    //bool deleteRoad = false;

    PtrTilesArea clearedTiles = tmap.getFilledRectangle( rPos, size );
    foreach( Tile* tile, clearedTiles )
    {
      BuildingType dstr2constr[] = { B_BURNING_RUINS, B_COLLAPSED_RUINS, B_PLAGUE_RUINS };
      bool canCreate = ConstructionManager::getInstance().canCreate( dstr2constr[_type] );
      if( canCreate )
      {
        city->build( dstr2constr[_type], tile->getIJ() );
      }
    }

    std::string dstr2string[] = { _("##alarm_fire_in_city##"), _("##alarm_building_collapsed##"),
                                  _("##alarm_plague_in_city##") };
    city->onDisasterEvent().emit( _pos, dstr2string[_type] );
  }
}
