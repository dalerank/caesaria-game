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

#include "oc3_servicebuildingmanager.hpp"
#include "oc3_water_buildings.hpp"
#include "oc3_service_building.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_prefecture.hpp"
#include "oc3_well.hpp"

ServiceBuilding* ServiceBuildingManager::getBuilding( ServiceType type )
{
  switch( type )
  {
    case S_WELL:	return new BuildingWell();
    case S_FOUNTAIN:	return new BuildingFountain();
    case S_ENGINEER:	return new BuildingEngineer();
    case S_PREFECT:	return new BuildingPrefecture();
    case S_THEATER:	return new BuildingTheater();
    default:		return 0;
  }
}
