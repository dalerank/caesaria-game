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

#include "infobox_legion.hpp"
#include "walker/patrolpoint.hpp"
#include "infobox_citizen_mgr.hpp"
#include "game/infoboxmanager.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "infobox_citizen.hpp"
#include "infobox_land.hpp"


using namespace  constants;

template< class T >
class CitizenInfoboxCreator : public InfoboxCreator
{
public:
  gui::InfoboxSimple* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    /*CityHelper helper( city );
    WalkerList walkers = helper.find<Walker>( walker::any, pos );

    if( walkers.empty() )
    {
      return new T( parent, city->getTilemap().at( pos ) );
    }
    else
    {
      PatrolPointList pp = helper.find<PatrolPoint>( walker::patrolPoint, pos );
      if( pp.empty() )
      {
        return new gui::InfoboxCitizen( parent, walkers );
      }
      else
      {
        return new gui::InfoboxLegion( parent, pp.front() );
      }
    }*/
  }
};

void InfoboxCitizenManager::loadInfoboxes(InfoboxManager& manager)
{
  manager.addInfobox( construction::road,         CAESARIA_STR_EXT(Road),   new CitizenInfoboxCreator<gui::InfoboxLand>() );
  manager.addInfobox( construction::plaza,        CAESARIA_STR_EXT(Plaza),  new CitizenInfoboxCreator<gui::InfoboxLand>() );
  manager.addInfobox( building::unknown,          CAESARIA_STR_EXT(unknown), new CitizenInfoboxCreator<gui::InfoboxLand>() );
}
