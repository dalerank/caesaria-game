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


#ifndef _OPENCAESAR3_BUILDING_CONSTANTS_INCLUDE_H_
#define _OPENCAESAR3_BUILDING_CONSTANTS_INCLUDE_H_

namespace constants
{

namespace construction
{

enum Type
{
  unknown=0,
  road,
  B_PLAZA,
  B_GARDEN,
  count
};

}

namespace building
{

enum Group
{
  unknownGroup=0,
  industryGroup,
  obtainGroup,
  foodGroup,
  disasterGroup,
  religionGroup,
  militaryGroup,
  nativeGroup,
  water,
  administration,
  BC_BRIDGE,
  BC_ENGINEERING,
  BC_TRADE,
  BC_TOWER,
  BC_GATE,
  BC_SECURITY,
  BC_EDUCATUION,
  BC_HEALTH,
  BC_SIGHT,
  BC_GARDEN,
  BC_ROAD,
  BC_ENTERTAINMENT,
  BC_HOUSE,
  BC_WALL,
  BC_MAX,
  anyGroup=BC_MAX
};

enum Type
{
//unknown building
  unknown=construction::unknown,
  any=unknown,

// entertainment
  amphitheater=construction::count,
  theater,
  hippodrome,
  colloseum,
  actorColony,
  gladiatorSchool,
  lionHouse,
  chariotSchool,

// house
  house,

// administration
  senate,
  forum,
  governorHouse,
  governorVilla,
  governorPalace,

// water
  B_RESERVOIR,
  B_AQUEDUCT,
  B_FOUNTAIN,
  B_WELL,

// security
  prefecture,
  B_WALL,
  B_TOWER,
  B_GATEHOUSE,
  B_FORT_LEGIONNAIRE,
  B_FORT_JAVELIN,
  B_FORT_MOUNTED,
  B_MILITARY_ACADEMY,
  B_BARRACKS,

// commerce
  market,
  granary,
  warehouse,

// farms
  wheatFarm,
  B_FRUIT_FARM,
  B_VEGETABLE_FARM,
  B_OLIVE_FARM,
  grapeFarm,
  B_PIG_FARM,

// raw materials
  marbleQuarry,
  ironMine,
  timberLogger,
  clayPit,

// factories
  B_WINE_WORKSHOP,
  B_OIL_WORKSHOP,
  B_WEAPONS_WORKSHOP,
  B_FURNITURE,
  pottery,

// utility
  engineerPost,
  B_STATUE1,
  B_STATUE2,
  B_STATUE3,
  lowBridge,
  highBridge,
  B_SHIPYARD,
  B_DOCK,
  wharf,

// health
  B_DOCTOR,
  B_HOSPITAL,
  B_BATHS,
  B_BARBER,

// education
  B_SCHOOL,
  B_COLLEGE,
  B_LIBRARY,
  B_MISSION_POST,

// religion: small temples
  templeCeres,
  B_TEMPLE_MARS,
  B_TEMPLE_NEPTUNE,
  B_TEMPLE_MERCURE,
  B_TEMPLE_VENUS,

// religion: big temples
  B_BIG_TEMPLE_CERES,
  B_BIG_TEMPLE_MARS,
  B_BIG_TEMPLE_NEPTUNE,
  B_BIG_TEMPLE_MERCURE,
  B_BIG_TEMPLE_VENUS,

// religion: others
  B_TEMPLE_ORACLE,

// building of natives
  B_NATIVE_HUT,
  B_NATIVE_CENTER,
  B_NATIVE_FIELD,

//other
  B_TRIUMPHAL_ARCH,
  burningRuins,
  B_BURNED_RUINS,
  B_COLLAPSED_RUINS,
  B_PLAGUE_RUINS,

  B_FORUM_2,
  B_SENATE_2,
  userType=1000
}; //end Type

} //end namespace building

} //end namespace constants

#endif  //_OPENCAESAR3_BUILDING_CONSTANTS_INCLUDE_H_
