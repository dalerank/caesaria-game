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

BEGIN_NAMEDTYPE(Group)
  std::string toString() const;
END_NAMEDTYPE(Group)

BEGIN_NAMEDTYPE(Type)
  std::string toString() const;
END_NAMEDTYPE(Type)

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
typedef std::set<Type> TypeSet;
typedef std::vector<Type> Types;

//unknown building
const Type unknown(0);
const Type any(0);
const Type road(1);
const Type plaza(2);
const Type garden(3);

// entertainment
const Type amphitheater(4);
const Type theater(5);
const Type hippodrome(6);
const Type colloseum(7);
const Type actorColony(8);
const Type gladiatorSchool(9);
const Type lionsNursery(10);
const Type chariotSchool(11);

//watersupply
const Type reservoir(12);
const Type aqueduct(13);
const Type fountain(14);
const Type well(15);

// house
const Type house(16);

// administration
const Type senate(17);
const Type forum(18);
const Type governorHouse(19);
const Type governorVilla(20);//20
const Type governorPalace(21);

// security
const Type prefecture(22);
const Type wall(23);
const Type fortification(24);
const Type tower(25);
const Type gatehouse(26);
const Type fort_legionaries(27);
const Type fort_javelin(28);
const Type fort_horse(29);
const Type military_academy(30);
const Type barracks(31);
const Type fortArea(32);

// commerce
const Type market(33);
const Type granery(34);
const Type warehouse(35);

// farms
const Type wheat_farm(36);
const Type fig_farm(37);
const Type vegetable_farm(38);
const Type olive_farm(39);
const Type vinard(40);
const Type meat_farm(41);

// raw materials
const Type quarry(42);
const Type iron_mine(43);
const Type lumber_mill(44);
const Type clay_pit(45);

// factories
const Type wine_workshop(46);
const Type oil_workshop(47);
const Type weapons_workshop(48);
const Type furniture_workshop(49);
const Type pottery_workshop(50);

// utility
const Type engineering_post(51);
const Type statue_small(52);
const Type statue_middle(53);
const Type statue_big(54);
const Type low_bridge(55);
const Type high_bridge(56);
const Type shipyard(57);
const Type dock(58);
const Type wharf(59);

// health
const Type clinic(60);
const Type hospital(61);
const Type baths(62);
const Type barber(63);

// education
const Type school(64);
const Type academy(65);
const Type library(66);
const Type missionaryPost(67);

// religion: small temples
const Type small_ceres_temple(68);
const Type small_mars_temple(69);
const Type small_neptune_temple(70);
const Type small_mercury_temple(71);
const Type small_venus_temple(72);

// religion: big temples
const Type big_ceres_temple(73);
const Type big_mars_temple(74);
const Type big_neptune_temple(75);
const Type big_mercury_temple(76);
const Type big_venus_temple(77);

// religion: others
const Type oracle(78);

// building of natives
const Type native_hut(79);
const Type native_center(80);
const Type native_field(81);

//other
const Type triumphal_arch(82);
const Type burning_ruins(83);
const Type burned_ruins(84);
const Type collapsed_ruins(85);
const Type plague_ruins(86);

const Type reserved_2(87);
const Type reserved_3(88);
const Type elevation(89);
const Type rift(91);
const Type river(92);
const Type tree(93);
const Type waymark(94);
const Type terrain(95);
const Type water(96);
const Type meadow(97);
const Type roadBlock(98);
const Type farmtile(99);

} //end namespace object

#endif  //_CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
