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
#include "core/enumerator.hpp"
#include "core/utils.hpp"
#include <set>

namespace object
{

namespace group
{
BEGIN_NAMEDTYPE(ID,unknownGroup)
APPEND_NAMEDTYPE_ID(any,            0)
APPEND_NAMEDTYPE_ID(unknown,        0)
APPEND_NAMEDTYPE_ID(industry,       1)
APPEND_NAMEDTYPE_ID(obtain,         2)
APPEND_NAMEDTYPE_ID(food,           3)
APPEND_NAMEDTYPE_ID(disaster,       4)
APPEND_NAMEDTYPE_ID(religion,       5)
APPEND_NAMEDTYPE_ID(military,       6)
APPEND_NAMEDTYPE_ID(native,         7)
APPEND_NAMEDTYPE_ID(water,          8)
APPEND_NAMEDTYPE_ID(administration, 9)
APPEND_NAMEDTYPE_ID(bridge,         10)
APPEND_NAMEDTYPE_ID(engineering,    11)
APPEND_NAMEDTYPE_ID(trade,          12)
APPEND_NAMEDTYPE_ID(tower,          13)
APPEND_NAMEDTYPE_ID(gate,           14)
APPEND_NAMEDTYPE_ID(security,       15)
APPEND_NAMEDTYPE_ID(education,      16)
APPEND_NAMEDTYPE_ID(health,         17)
APPEND_NAMEDTYPE_ID(sight,          18)
APPEND_NAMEDTYPE_ID(garden,         19)
APPEND_NAMEDTYPE_ID(road,           20)
APPEND_NAMEDTYPE_ID(entertainment,  21)
APPEND_NAMEDTYPE_ID(house,          22)
APPEND_NAMEDTYPE_ID(wall,           23)
END_NAMEDTYPE(ID)
}

typedef group::ID Group;
std::string toString( const Group& g );

BEGIN_NAMEDTYPE(Type,unknown)
//stuff
APPEND_NAMEDTYPE_ID(any,                  0)
APPEND_NAMEDTYPE_ID(road,                 1)
APPEND_NAMEDTYPE_ID(plaza,                2)
APPEND_NAMEDTYPE_ID(garden,               3)

// entertainment
APPEND_NAMEDTYPE_ID(amphitheater,         4)
APPEND_NAMEDTYPE_ID(theater,              5)
APPEND_NAMEDTYPE_ID(hippodrome,           6)
APPEND_NAMEDTYPE_ID(colloseum,            7)
APPEND_NAMEDTYPE_ID(actorColony,          8)
APPEND_NAMEDTYPE_ID(gladiatorSchool,      9)
APPEND_NAMEDTYPE_ID(lionsNursery,         10)
APPEND_NAMEDTYPE_ID(chariotSchool,        11)

//watersupply
APPEND_NAMEDTYPE_ID(reservoir,            12)
APPEND_NAMEDTYPE_ID(aqueduct,             13)
APPEND_NAMEDTYPE_ID(fountain,             14)
APPEND_NAMEDTYPE_ID(well,                 15)

// house
APPEND_NAMEDTYPE_ID(house,                16)

// administration
APPEND_NAMEDTYPE_ID(senate,               17)
APPEND_NAMEDTYPE_ID(forum,                18)
APPEND_NAMEDTYPE_ID(governorHouse,        19)
APPEND_NAMEDTYPE_ID(governorVilla,        20)
APPEND_NAMEDTYPE_ID(governorPalace,       21)

// security
APPEND_NAMEDTYPE_ID(prefecture,           22)
APPEND_NAMEDTYPE_ID(wall,                 23)
APPEND_NAMEDTYPE_ID(fortification,        24)
APPEND_NAMEDTYPE_ID(tower,                25)
APPEND_NAMEDTYPE_ID(gatehouse,            26)
APPEND_NAMEDTYPE_ID(fort_legionaries,     27)
APPEND_NAMEDTYPE_ID(fort_javelin,         28)
APPEND_NAMEDTYPE_ID(fort_horse,           29)
APPEND_NAMEDTYPE_ID(military_academy,     30)
APPEND_NAMEDTYPE_ID(barracks,             31)
APPEND_NAMEDTYPE_ID(fortArea,             32)

// commerce
APPEND_NAMEDTYPE_ID(market,               33)
APPEND_NAMEDTYPE_ID(granery,              34)
APPEND_NAMEDTYPE_ID(warehouse,            35)

// farms
APPEND_NAMEDTYPE_ID(wheat_farm,           36)
APPEND_NAMEDTYPE_ID(fig_farm,             37)
APPEND_NAMEDTYPE_ID(vegetable_farm,       38)
APPEND_NAMEDTYPE_ID(olive_farm,           39)
APPEND_NAMEDTYPE_ID(vinard,               40)
APPEND_NAMEDTYPE_ID(meat_farm,            41)

// raw materials
APPEND_NAMEDTYPE_ID(quarry,               42)
APPEND_NAMEDTYPE_ID(iron_mine,            43)
APPEND_NAMEDTYPE_ID(lumber_mill,          44)
APPEND_NAMEDTYPE_ID(clay_pit,             45)

// factories
APPEND_NAMEDTYPE_ID(wine_workshop,        46)
APPEND_NAMEDTYPE_ID(oil_workshop,         47)
APPEND_NAMEDTYPE_ID(weapons_workshop,     48)
APPEND_NAMEDTYPE_ID(furniture_workshop,   49)
APPEND_NAMEDTYPE_ID(pottery_workshop,     50)

// utility
APPEND_NAMEDTYPE_ID(engineering_post,     51)
APPEND_NAMEDTYPE_ID(statue_small,         52)
APPEND_NAMEDTYPE_ID(statue_middle,        53)
APPEND_NAMEDTYPE_ID(statue_big,           54)
APPEND_NAMEDTYPE_ID(low_bridge,           55)
APPEND_NAMEDTYPE_ID(high_bridge,          56)
APPEND_NAMEDTYPE_ID(shipyard,             57)
APPEND_NAMEDTYPE_ID(dock,                 58)
APPEND_NAMEDTYPE_ID(wharf,                59)

// health
APPEND_NAMEDTYPE_ID(clinic,               60)
APPEND_NAMEDTYPE_ID(hospital,             61)
APPEND_NAMEDTYPE_ID(baths,                62)
APPEND_NAMEDTYPE_ID(barber,               63)

// education
APPEND_NAMEDTYPE_ID(school,               64)
APPEND_NAMEDTYPE_ID(academy,              65)
APPEND_NAMEDTYPE_ID(library,              66)
APPEND_NAMEDTYPE_ID(missionaryPost,       67)

// religion: small temples
APPEND_NAMEDTYPE_ID(small_ceres_temple,   68)
APPEND_NAMEDTYPE_ID(small_mars_temple,    69)
APPEND_NAMEDTYPE_ID(small_neptune_temple, 70)
APPEND_NAMEDTYPE_ID(small_mercury_temple, 71)
APPEND_NAMEDTYPE_ID(small_venus_temple,   72)

// religion: big temples
APPEND_NAMEDTYPE_ID(big_ceres_temple,     73)
APPEND_NAMEDTYPE_ID(big_mars_temple,      74)
APPEND_NAMEDTYPE_ID(big_neptune_temple,   75)
APPEND_NAMEDTYPE_ID(big_mercury_temple,   76)
APPEND_NAMEDTYPE_ID(big_venus_temple,     77)

// religion: others
APPEND_NAMEDTYPE_ID(oracle,               78)

// building of natives
APPEND_NAMEDTYPE_ID(native_hut,           79)
APPEND_NAMEDTYPE_ID(native_center,        80)
APPEND_NAMEDTYPE_ID(native_field,         81)

//other
APPEND_NAMEDTYPE_ID(triumphal_arch,       82)
APPEND_NAMEDTYPE_ID(burning_ruins,        83)
APPEND_NAMEDTYPE_ID(burned_ruins,         84)
APPEND_NAMEDTYPE_ID(collapsed_ruins,      85)
APPEND_NAMEDTYPE_ID(plague_ruins,         86)
APPEND_NAMEDTYPE_ID(reserved_2,           87)
APPEND_NAMEDTYPE_ID(wolves_den,           88)
APPEND_NAMEDTYPE_ID(elevation,            89)
APPEND_NAMEDTYPE_ID(rift,                 90)
APPEND_NAMEDTYPE_ID(river,                91)
APPEND_NAMEDTYPE_ID(tree,                 92)
APPEND_NAMEDTYPE_ID(waymark,              93)
APPEND_NAMEDTYPE_ID(terrain,              94)
APPEND_NAMEDTYPE_ID(water,                95)
APPEND_NAMEDTYPE_ID(meadow,               96)
APPEND_NAMEDTYPE_ID(roadBlock,            97)
APPEND_NAMEDTYPE_ID(farmtile,             98)
APPEND_NAMEDTYPE_ID(coast,                99)
APPEND_NAMEDTYPE_ID(rock,                100)
APPEND_NAMEDTYPE_ID(plateau,             101)
APPEND_NAMEDTYPE_ID(attackTrigger,       102)

APPEND_NAMEDTYPE(userType)

END_NAMEDTYPE(Type)


typedef std::set<Group> GroupSet;
typedef std::vector<Group> Groups;
typedef std::vector<Type> Types;
std::string toString( const Type& t);

class TypeSet : public std::set<Type>
{
public:
  VariantList save() const;
  void load( const VariantList& stream );
};

Type toType( const std::string& name);
Type findType( const std::string& name );

template<class ObjectPtr>
inline Type typeOrDefault( ObjectPtr obj, Type defaultValue=unknown )
{
  return obj.isValid() ? obj->type() : defaultValue;
}

} //end namespace object

#define REGISTER_CUSTOM_OBJECT_TYPE(name) static const object::Type name = object::Type(Hash(name));

#endif  //_CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
