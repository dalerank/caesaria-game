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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef _CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
#define _CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_

#include "predefinitions.hpp"
#include <set>

namespace object
{

namespace group
{
const Group unknown(0);
const Group any(0);
const Group industry(1);
const Group obtain(2);
const Group food(3);
const Group disaster(4);
const Group religion(5);
const Group military(6);
const Group native(7);
const Group water(8);
const Group administration(9);
const Group bridge(10);
const Group engineering(11);
const Group trade(12);
const Group tower(13);
const Group gate(14);
const Group security(15);
const Group education(16);
const Group health(17);
const Group sight(18);
const Group garden(19);
const Group road(20);
const Group entertainment(21);
const Group house(22);
const Group wall(23);
}

typedef std::set<Group> GroupSet;
typedef std::vector<Group> Groups;

}

namespace constants
{

namespace objects
{

enum Type
{
//unknown building
  unknown=0,
  any=0,
  road,
  plaza,
  garden,

// entertainment
  amphitheater,
  theater,
  hippodrome,
  colloseum,
  actorColony,
  gladiatorSchool,
  lionsNursery,//10
  chariotSchool,

  //watersupply
  reservoir,
  aqueduct,
  fountain,
  well,

// house
  house,

// administration
  senate,
  forum,
  governorHouse,
  governorVilla,//20
  governorPalace,

// security
  prefecture,
  wall,
  fortification,
  tower,
  gatehouse,
  fort_legionaries,
  fort_javelin,
  fort_horse,
  military_academy,//30
  barracks,
  fortArea,

// commerce
  market,
  granery,
  warehouse,

// farms
  wheat_farm,
  fig_farm,
  vegetable_farm,
  olive_farm,
  vinard,//40
  meat_farm,

// raw materials
  quarry,
  iron_mine,
  lumber_mill,
  clay_pit,

// factories
  wine_workshop,
  oil_workshop,
  weapons_workshop,
  furniture_workshop,
  pottery_workshop,//50

// utility
  engineering_post,
  statue_small,
  statue_middle,
  statue_big,
  low_bridge,
  high_bridge,
  shipyard,
  dock,
  wharf,

// health
  clinic,//60
  hospital,
  baths,
  barber,

// education
  school,
  academy,
  library,
  missionaryPost,

// religion: small temples
  small_ceres_temple,
  small_mars_temple,
  small_neptune_temple,//70
  small_mercury_temple,
  small_venus_temple,

// religion: big temples
  big_ceres_temple,
  big_mars_temple,
  big_neptune_temple,
  big_mercury_temple,
  big_venus_temple,

// religion: others
  oracle,

// building of natives
  native_hut,
  native_center,//80
  native_field,

//other
  triumphal_arch,
  burning_ruins,
  burned_ruins,
  collapsed_ruins,
  plague_ruins,

  reserved_2,
  reserved_3,
  elevation,
  rift,//90
  river,
  tree,
  waymark,
  terrain,
  water,
  meadow,
  roadBlock,
  farmtile,

  typeCount,
  userType=1000
}; //end Type

} //end namespace building

} //end namespace constants

#endif  //_CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
