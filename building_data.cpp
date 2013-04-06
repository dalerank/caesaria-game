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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include "building_data.hpp"

#include <iostream>

#include "exception.hpp"
#include "gettext.hpp"


BuildingData::BuildingData(const BuildingType buildingType, const std::string &name, const int cost)
{
   _buildingType = buildingType;
   _name = name;
   std::string key = "building_"+name;
   _prettyName = _(key.c_str());  // i18n translation
   _cost = cost;
}

std::string BuildingData::getName() const
{
   return _name;
}

std::string BuildingData::getPrettyName() const
{
   return _prettyName;
}

BuildingType BuildingData::getType() const
{
   return _buildingType;
}

int BuildingData::getCost() const
{
   return _cost;
}


BuildingDataHolder* BuildingDataHolder::_instance = NULL;

BuildingDataHolder& BuildingDataHolder::instance()
{
   if (_instance == NULL)
   {
      _instance = new BuildingDataHolder();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}

BuildingType BuildingDataHolder::getBuildingTypeByService(const ServiceType serviceType)
{
   return _mapBuildingByService[serviceType];
}

BuildingType BuildingDataHolder::getBuildingTypeByInGood(const GoodType inGoodType)
{
   BuildingType res = B_NONE;

   std::map<GoodType, BuildingType>::iterator mapIt;
   mapIt = _mapBuildingByInGood.find(inGoodType);
   if (mapIt != _mapBuildingByInGood.end())
   {
      res = mapIt->second;
   }
   return res;
}

BuildingType BuildingDataHolder::getBuildingTypeByOutGood(const GoodType outGoodType)
{
   return _mapBuildingByOutGood[outGoodType];
}

BuildingData& BuildingDataHolder::getData(const BuildingType buildingType)
{
   std::map<BuildingType, BuildingData>::iterator mapIt;
   mapIt = _mapDataByName.find(buildingType);
   if (mapIt == _mapDataByName.end())
   {
      THROW("Unknown building " << buildingType);
   }
   return mapIt->second;
}

bool BuildingDataHolder::hasData(const BuildingType buildingType)
{
   bool res = true;
   std::map<BuildingType, BuildingData>::iterator mapIt;
   mapIt = _mapDataByName.find(buildingType);
   if (mapIt == _mapDataByName.end())
   {
      res = false;
   }
   return res;
}

void BuildingDataHolder::addData(const BuildingData &data)
{
   BuildingType buildingType = data.getType();

   if (hasData(buildingType))
   {
      THROW("Building is already set " << data.getName());
   }

   _mapDataByName.insert(std::make_pair(buildingType, data));
}


BuildingDataHolder::BuildingDataHolder()
{
   // entertainment
   addData(BuildingData(B_AMPHITHEATER, "amphitheater", 100));
   addData(BuildingData(B_THEATER, "theater", 50));
   addData(BuildingData(B_HIPPODROME, "hippodrome", 2500));
   addData(BuildingData(B_COLLOSSEUM, "collosseum", 500));
   addData(BuildingData(B_ACTOR, "actor", 50));
   addData(BuildingData(B_GLADIATOR, "gladiator", 75));
   addData(BuildingData(B_LION, "lion", 75));
   addData(BuildingData(B_CHARIOT, "chariot", 75));

   // road & house
   addData(BuildingData(B_HOUSE, "house", 10));
   addData(BuildingData(B_ROAD, "road", 4));
   addData(BuildingData(B_PLAZA, "plaza", 15));
   addData(BuildingData(B_GARDEN, "garden", 12));

   // administration
   addData(BuildingData(B_SENATE, "senate", 250));
   addData(BuildingData(B_FORUM, "forum", 75));
   addData(BuildingData(B_GOVERNOR_HOUSE, "governor's_house", 500));
   addData(BuildingData(B_GOVERNOR_VILLA, "governor's_villa", 500));
   addData(BuildingData(B_GOVERNOR_PALACE, "governor's_palace", 500));
   
   // security
   addData(BuildingData(B_FORT_LEGIONNAIRE, "fort_legionaries", 1000));
   addData(BuildingData(B_FORT_JAVELIN, "fort_javelin", 1000));
   addData(BuildingData(B_FORT_MOUNTED, "fort_horse", 1000));
   addData(BuildingData(B_PREFECT, "prefect", 30));
   addData(BuildingData(B_BARRACKS, "barracks", 150));
   addData(BuildingData(B_MILITARY_ACADEMY, "academy", 1000));

   // health
   addData(BuildingData(B_DOCTOR, "doctor", 30));
   addData(BuildingData(B_HOSPITAL, "hospital", 300));
   addData(BuildingData(B_BATHS, "baths", 50));
   addData(BuildingData(B_BARBER, "barber", 25));

   // education
   addData(BuildingData(B_SCHOOL, "school", 50));
   addData(BuildingData(B_COLLEGE, "college", 100));
   addData(BuildingData(B_LIBRARY, "library", 75));
   addData(BuildingData(B_MISSION_POST, "mission post", 0));

   // // religion
   addData(BuildingData(B_TEMPLE_CERES, "temple_ceres", 50));
   addData(BuildingData(B_TEMPLE_NEPTUNE, "temple_neptune", 50));
   addData(BuildingData(B_TEMPLE_MARS, "temple_mars", 50));
   addData(BuildingData(B_TEMPLE_MERCURE, "temple_mercure", 50));
   addData(BuildingData(B_TEMPLE_VENUS, "temple_venus", 50));

   addData(BuildingData(B_BIG_TEMPLE_CERES, "big_temple_ceres", 150));
   addData(BuildingData(B_BIG_TEMPLE_NEPTUNE, "big_temple_neptune", 150));
   addData(BuildingData(B_BIG_TEMPLE_MARS, "big_temple_mars", 150));
   addData(BuildingData(B_BIG_TEMPLE_MERCURE, "big_temple_mercure", 150));
   addData(BuildingData(B_BIG_TEMPLE_VENUS, "big_temple_venus", 150));

   addData(BuildingData(B_TEMPLE_ORACLE, "temple_oracle", 200));

   // commerce
   addData(BuildingData(B_MARKET, "market", 40));
   addData(BuildingData(B_GRANARY, "granary", 100));
   addData(BuildingData(B_WAREHOUSE, "warehouse", 70));
   addData(BuildingData(B_WHEAT, "wheat", 40));
   addData(BuildingData(B_FRUIT, "fruit", 40));
   addData(BuildingData(B_VEGETABLE, "vegetable", 40));
   addData(BuildingData(B_OLIVE, "olive", 40));
   addData(BuildingData(B_GRAPE, "grape", 40));
   addData(BuildingData(B_MEAT, "meat", 40));
   addData(BuildingData(B_MARBLE, "marble", 50));
   addData(BuildingData(B_IRON, "iron", 50));
   addData(BuildingData(B_TIMBER, "timber", 40));
   addData(BuildingData(B_CLAY, "clay", 40));
   addData(BuildingData(B_WINE, "wine", 45));
   addData(BuildingData(B_OIL, "oil", 50));
   addData(BuildingData(B_WEAPON, "weapon", 50));
   addData(BuildingData(B_FURNITURE, "furniture", 40));
   addData(BuildingData(B_POTTERY, "pottery", 40));

   // utility
   addData(BuildingData(B_ENGINEER,    "engineer",     30));
   addData(BuildingData(B_STATUE1,     "statue1",      12));
   addData(BuildingData(B_STATUE2,     "statue2",      60));
   addData(BuildingData(B_STATUE3,     "statue3",     150));
   addData(BuildingData(B_LOW_BRIDGE,  "low_bridge",   40));
   addData(BuildingData(B_HIGH_BRIDGE, "high_bridge", 100));
   addData(BuildingData(B_DOCK,        "dock",        100));
   addData(BuildingData(B_SHIPYARD,    "shipyard",    100));
   addData(BuildingData(B_WHARF,       "wharf",       100));
   addData(BuildingData(B_TRIUMPHAL_ARCH, "arch",     100));

   // water
   addData(BuildingData(B_WELL, "well", 5));
   addData(BuildingData(B_FOUNTAIN, "fountain", 15));
   addData(BuildingData(B_AQUEDUCT, "aqueduct", 0));
   addData(BuildingData(B_RESERVOIR, "reservoir", 0));
   
   // natives
   addData(BuildingData(B_NATIVE_HUT, "native hut", 0));
   addData(BuildingData(B_NATIVE_CENTER, "native center", 0));   
   addData(BuildingData(B_NATIVE_FIELD, "native field", 0));   
   

   // populate _mapBuildingByService
   _mapBuildingByService[S_ENGINEER] = B_ENGINEER;
   _mapBuildingByService[S_PREFECT] = B_PREFECT;
   _mapBuildingByService[S_FORUM] = B_FORUM;
   _mapBuildingByService[S_SENATE] = B_SENATE;
   _mapBuildingByService[S_TEMPLE_NEPTUNE] = B_TEMPLE_NEPTUNE;
   _mapBuildingByService[S_TEMPLE_CERES] = B_TEMPLE_CERES;
   _mapBuildingByService[S_TEMPLE_VENUS] = B_TEMPLE_VENUS;
   _mapBuildingByService[S_TEMPLE_MARS] = B_TEMPLE_MARS;
   _mapBuildingByService[S_TEMPLE_MERCURE] = B_TEMPLE_MERCURE;
   _mapBuildingByService[S_DOCTOR] = B_DOCTOR;
   _mapBuildingByService[S_BARBER] = B_BARBER;
   _mapBuildingByService[S_BATHS] = B_BATHS;
   _mapBuildingByService[S_HOSPITAL] = B_HOSPITAL;
   _mapBuildingByService[S_SCHOOL] = B_SCHOOL;
   _mapBuildingByService[S_LIBRARY] = B_LIBRARY;
   _mapBuildingByService[S_COLLEGE] = B_COLLEGE;
   _mapBuildingByService[S_THEATER] = B_THEATER;
   _mapBuildingByService[S_AMPHITHEATER] = B_AMPHITHEATER;
   _mapBuildingByService[S_COLLOSSEUM] = B_COLLOSSEUM;
   _mapBuildingByService[S_HIPPODROME] = B_HIPPODROME;

   // populate _mapBuildingByInGood
   _mapBuildingByInGood[G_IRON]   = B_WEAPON;
   _mapBuildingByInGood[G_TIMBER] = B_FURNITURE;
   _mapBuildingByInGood[G_CLAY]   = B_POTTERY;
   _mapBuildingByInGood[G_OLIVE]  = B_OIL;
   _mapBuildingByInGood[G_GRAPE]  = B_WINE;

   // populate _mapBuildingByOutGood
   _mapBuildingByOutGood[G_WHEAT]     = B_WHEAT;
   _mapBuildingByOutGood[G_FRUIT]     = B_FRUIT;
   _mapBuildingByOutGood[G_VEGETABLE] = B_VEGETABLE;
   _mapBuildingByOutGood[G_OLIVE]     = B_OLIVE;
   _mapBuildingByOutGood[G_GRAPE]     = B_GRAPE;
   _mapBuildingByOutGood[G_MEAT]      = B_MEAT;
   _mapBuildingByOutGood[G_MARBLE]    = B_MARBLE;
   _mapBuildingByOutGood[G_IRON]      = B_IRON;
   _mapBuildingByOutGood[G_TIMBER]    = B_TIMBER;
   _mapBuildingByOutGood[G_CLAY]      = B_CLAY;
   _mapBuildingByOutGood[G_OLIVE]     = B_OLIVE;
   _mapBuildingByOutGood[G_GRAPE]     = B_GRAPE;
   _mapBuildingByOutGood[G_WINE]      = B_WINE;
   _mapBuildingByOutGood[G_OIL]       = B_OIL;
   _mapBuildingByOutGood[G_WEAPON]    = B_WEAPON;
   _mapBuildingByOutGood[G_FURNITURE] = B_FURNITURE;
   _mapBuildingByOutGood[G_POTTERY]   = B_POTTERY;
   _mapBuildingByOutGood[G_FISH]      = B_WHARF;
}

