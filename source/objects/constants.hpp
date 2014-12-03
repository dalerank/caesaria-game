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

namespace constants
{

namespace objects
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
  waterGroup,
  administrationGroup,
  bridgeGroup,
  engineeringGroup,
  tradeGroup,
  towerGroup,
  gateGroup,
  securityGroup,
  educationGroup,
  healthGroup,
  sightGroup,
  gardenGroup,
  roadGroup,
  entertainmentGroup,
  houseGroup,
  wallGroup,
  countGroup,
  anyGroup=countGroup
};

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
  fortLegionaire,
  fortJavelin,
  fortMounted,
  militaryAcademy,//30
  barracks,
  fortArea,

// commerce
  market,
  granary,
  warehouse,

// farms
  wheatFarm,
  fruitFarm,
  vegetableFarm,
  oliveFarm,
  grapeFarm,//40
  pigFarm,

// raw materials
  marbleQuarry,
  ironMine,
  timberLogger,
  clayPit,

// factories
  winery,
  creamery,
  weaponsWorkshop,
  furnitureWorkshop,
  pottery,//50

// utility
  engineerPost,
  smallStatue,
  middleStatue,
  bigStatue,
  lowBridge,
  highBridge,
  shipyard,
  dock,
  wharf,

// health
  doctor,//60
  hospital,
  baths,
  barber,

// education
  school,
  academy,
  library,
  missionaryPost,

// religion: small temples
  templeCeres,
  templeMars,
  templeNeptune,//70
  templeMercury,
  templeVenus,

// religion: big temples
  cathedralCeres,
  cathedralMars,
  cathedralNeptune,
  cathedralMercury,
  cathedralVenus,

// religion: others
  oracle,

// building of natives
  nativeHut,
  nativeCenter,//80
  nativeField,

//other
  triumphalArch,
  burningRuins,
  burnedRuins,
  collapsedRuins,
  plagueRuins,

  forum2,
  senate2,
  elevation,
  rift,//90
  river,
  tree,
  waymark,
  terrain,
  water,

  typeCount,
  userType=1000
}; //end Type

} //end namespace building

} //end namespace constants

#endif  //_CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
