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

#include "oc3_constructionmanager.hpp"
#include "oc3_service_building.hpp"
#include "oc3_training_building.hpp"
#include "oc3_water_buildings.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_collapsedruins.hpp"
#include "oc3_buildingengineer.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_burningruins.hpp"
#include "oc3_house.hpp"
#include "oc3_senate.hpp"
#include "oc3_buildingprefect.hpp"

#include <map>

class ConstructionManager::Impl
{
public:
    std::map<BuildingType, LandOverlay*> buildings;

    Impl()
    {
        // first call to this method

        // entertainment
        buildings[B_THEATER]      = new BuildingTheater();
        buildings[B_AMPHITHEATER] = new BuildingAmphiTheater();
        buildings[B_COLLOSSEUM]   = new BuildingCollosseum();
        buildings[B_ACTOR]        = new BuildingActor();
        buildings[B_GLADIATOR]    = new BuildingGladiator();
        buildings[B_LION]         = new BuildingLion();
        buildings[B_CHARIOT]      = new BuildingChariot();
        buildings[B_HIPPODROME]   = new BuildingHippodrome();
        // road&house
        buildings[B_HOUSE] = new House( House::smallHovel );
        buildings[B_ROAD]  = new Road();
        // administration
        buildings[B_FORUM]  = new Forum();
        buildings[B_SENATE] = new Senate();
        buildings[B_GOVERNOR_HOUSE]  = new GovernorsHouse();
        buildings[B_GOVERNOR_VILLA]  = new GovernorsVilla();
        buildings[B_GOVERNOR_PALACE] = new GovernorsPalace(); 
        buildings[B_STATUE1] = new SmallStatue(); 
        buildings[B_STATUE2] = new MediumStatue(); 
        buildings[B_STATUE3] = new BigStatue();
        buildings[B_GARDEN]  = new Garden();
        buildings[B_PLAZA]   = new Plaza();
        // water
        buildings[B_WELL]      = new BuildingWell();
        buildings[B_FOUNTAIN]  = new BuildingFountain();
        buildings[B_AQUEDUCT]  = new Aqueduct();
        buildings[B_RESERVOIR] = new Reservoir();
        // security
        buildings[B_PREFECT]   = new BuildingPrefect();
        buildings[B_FORT_LEGIONNAIRE] = new FortLegionnaire();
        buildings[B_FORT_JAVELIN]     = new FortJaveline();
        buildings[B_FORT_MOUNTED]     = new FortMounted();
        buildings[B_MILITARY_ACADEMY] = new Academy();
        buildings[B_BARRACKS]         = new Barracks();
        // commerce
        buildings[B_MARKET]    = new Market();
        buildings[B_WAREHOUSE] = new Warehouse();
        buildings[B_GRANARY]   = new Granary();
        // farms
        buildings[B_WHEAT]     = new FarmWheat();
        buildings[B_OLIVE]     = new FarmOlive();
        buildings[B_GRAPE]     = new FarmGrape();
        buildings[B_MEAT]      = new FarmMeat();
        buildings[B_FRUIT]     = new FarmFruit();
        buildings[B_VEGETABLE] = new FarmVegetable();
        // raw materials
        buildings[B_IRON]   = new FactoryIron();
        buildings[B_TIMBER] = new FactoryTimber();
        buildings[B_CLAY]   = new FactoryClay();
        buildings[B_MARBLE] = new FactoryMarble();
        // factories
        buildings[B_WEAPON]    = new FactoryWeapon();
        buildings[B_FURNITURE] = new FactoryFurniture();
        buildings[B_WINE]      = new FactoryWine();
        buildings[B_OIL]       = new FactoryOil();
        buildings[B_POTTERY]   = new FactoryPottery();
        // utility
        buildings[B_ENGINEER] = new BuildingEngineer();
        buildings[B_DOCK]     = new Dock();
        buildings[B_SHIPYARD] = new Shipyard();
        buildings[B_WHARF]    = new Wharf();
        buildings[B_TRIUMPHAL_ARCH]  = new TriumphalArch();
        // religion
        buildings[B_TEMPLE_CERES]   = new TempleCeres();
        buildings[B_TEMPLE_NEPTUNE] = new TempleNeptune();
        buildings[B_TEMPLE_MARS]    = new TempleMars();
        buildings[B_TEMPLE_VENUS]   = new TempleVenus();
        buildings[B_TEMPLE_MERCURE] = new TempleMercure();
        buildings[B_BIG_TEMPLE_CERES]   = new BigTempleCeres();
        buildings[B_BIG_TEMPLE_NEPTUNE] = new BigTempleNeptune();
        buildings[B_BIG_TEMPLE_MARS]    = new BigTempleMars();
        buildings[B_BIG_TEMPLE_VENUS]   = new BigTempleVenus();
        buildings[B_BIG_TEMPLE_MERCURE] = new BigTempleMercure();
        buildings[B_TEMPLE_ORACLE]  = new TempleOracle();
        // health
        buildings[B_BATHS]    = new Baths();
        buildings[B_BARBER]   = new Barber();
        buildings[B_DOCTOR]   = new Doctor();
        buildings[B_HOSPITAL] = new Hospital();
        // education
        buildings[B_SCHOOL]  = new School();
        buildings[B_LIBRARY] = new Library();
        buildings[B_COLLEGE] = new College();
        buildings[B_MISSION_POST] = new MissionPost();
        // natives
        buildings[B_NATIVE_HUT]    = new NativeHut();
        buildings[B_NATIVE_CENTER] = new NativeCenter();
        buildings[B_NATIVE_FIELD]  = new NativeField();

        //damages
        buildings[B_BURNING_RUINS ] = new BurningRuins();
        buildings[B_COLLAPSED_RUINS ] = new CollapsedRuins();
    }
};

LandOverlay* ConstructionManager::create(const BuildingType buildingType)
{
    std::map<BuildingType, LandOverlay*>::iterator mapIt;
    mapIt = _d->buildings.find(buildingType);
    LandOverlay *res;

    if (mapIt == _d->buildings.end())
    {
        // THROW("Unknown building type:" << buildingType);
        res = NULL;
    }
    else
    {
        res = mapIt->second;
    }
    return res;
}

ConstructionManager& ConstructionManager::getInstance()
{
    static ConstructionManager inst;
    return inst;
}

ConstructionManager::ConstructionManager() : _d( new Impl )
{

}
