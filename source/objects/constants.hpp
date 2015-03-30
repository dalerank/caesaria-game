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

DEFINE_NAMEDTYPE(Type,unknown)
DEFINE_NAMEDTYPE(Group,unknownGroup)
std::string toString( const Type& t);
Type toType( const std::string& name);
std::string toString( const Group& g );

namespace group
{
#define REGISTER_GROUP(name,index) static const Group name = Group(index);

REGISTER_GROUP(any,0)
REGISTER_GROUP(unknown,0)
REGISTER_GROUP(industry,1)
REGISTER_GROUP(obtain,2)
REGISTER_GROUP(food,3)
REGISTER_GROUP(disaster,4)
REGISTER_GROUP(religion,5)
REGISTER_GROUP(military,6)
REGISTER_GROUP(native,7)
REGISTER_GROUP(water,8)
REGISTER_GROUP(administration,9)
REGISTER_GROUP(bridge,10)
REGISTER_GROUP(engineering,11)
REGISTER_GROUP(trade,12)
REGISTER_GROUP(tower,13)
REGISTER_GROUP(gate,14)
REGISTER_GROUP(security,15)
REGISTER_GROUP(education,16)
REGISTER_GROUP(health,17)
REGISTER_GROUP(sight,18)
REGISTER_GROUP(garden,19)
REGISTER_GROUP(road,20)
REGISTER_GROUP(entertainment,21)
REGISTER_GROUP(house,22)
REGISTER_GROUP(wall,23)
}

typedef std::set<Group> GroupSet;
typedef std::vector<Group> Groups;
typedef std::vector<Type> Types;
typedef std::set<Type> TypeSet;

//stuff
#define REGISTER_ID_OBJECT_TYPE(type,a) static const Type type = Type(a);
REGISTER_ID_OBJECT_TYPE(any,0)
REGISTER_ID_OBJECT_TYPE(road,1)
REGISTER_ID_OBJECT_TYPE(plaza,2)
REGISTER_ID_OBJECT_TYPE(garden,3)

// entertainment
REGISTER_ID_OBJECT_TYPE(amphitheater,4)
REGISTER_ID_OBJECT_TYPE(theater,5)
REGISTER_ID_OBJECT_TYPE(hippodrome,6)
REGISTER_ID_OBJECT_TYPE(colloseum,7)
REGISTER_ID_OBJECT_TYPE(actorColony,8)
REGISTER_ID_OBJECT_TYPE(gladiatorSchool,9)
REGISTER_ID_OBJECT_TYPE(lionsNursery,10)
REGISTER_ID_OBJECT_TYPE(chariotSchool,11)

//watersupply
REGISTER_ID_OBJECT_TYPE(reservoir,12)
REGISTER_ID_OBJECT_TYPE(aqueduct,13)
REGISTER_ID_OBJECT_TYPE(fountain,14)
REGISTER_ID_OBJECT_TYPE(well,15)

// house
REGISTER_ID_OBJECT_TYPE(house,16)

// administration
REGISTER_ID_OBJECT_TYPE(senate,17)
REGISTER_ID_OBJECT_TYPE(forum,18)
REGISTER_ID_OBJECT_TYPE(governorHouse,19)
REGISTER_ID_OBJECT_TYPE(governorVilla,20)
REGISTER_ID_OBJECT_TYPE(governorPalace,21)

// security
REGISTER_ID_OBJECT_TYPE(prefecture,22)
REGISTER_ID_OBJECT_TYPE(wall,23)
REGISTER_ID_OBJECT_TYPE(fortification,24)
REGISTER_ID_OBJECT_TYPE(tower,25)
REGISTER_ID_OBJECT_TYPE(gatehouse,26)
REGISTER_ID_OBJECT_TYPE(fort_legionaries,27)
REGISTER_ID_OBJECT_TYPE(fort_javelin,28)
REGISTER_ID_OBJECT_TYPE(fort_horse,29)
REGISTER_ID_OBJECT_TYPE(military_academy,30)
REGISTER_ID_OBJECT_TYPE(barracks,31)
REGISTER_ID_OBJECT_TYPE(fortArea,32)

// commerce
REGISTER_ID_OBJECT_TYPE(market,33)
REGISTER_ID_OBJECT_TYPE(granery,34)
REGISTER_ID_OBJECT_TYPE(warehouse,35)

// farms
REGISTER_ID_OBJECT_TYPE(wheat_farm,36)
REGISTER_ID_OBJECT_TYPE(fig_farm,37)
REGISTER_ID_OBJECT_TYPE(vegetable_farm,38)
REGISTER_ID_OBJECT_TYPE(olive_farm,39)
REGISTER_ID_OBJECT_TYPE(vinard,40)
REGISTER_ID_OBJECT_TYPE(meat_farm,41)

// raw materials
REGISTER_ID_OBJECT_TYPE(quarry,42)
REGISTER_ID_OBJECT_TYPE(iron_mine,43)
REGISTER_ID_OBJECT_TYPE(lumber_mill,44)
REGISTER_ID_OBJECT_TYPE(clay_pit,45)

// factories
REGISTER_ID_OBJECT_TYPE(wine_workshop,46)
REGISTER_ID_OBJECT_TYPE(oil_workshop,47)
REGISTER_ID_OBJECT_TYPE(weapons_workshop,48)
REGISTER_ID_OBJECT_TYPE(furniture_workshop,49)
REGISTER_ID_OBJECT_TYPE(pottery_workshop,50)

// utility
REGISTER_ID_OBJECT_TYPE(engineering_post,51)
REGISTER_ID_OBJECT_TYPE(statue_small,52)
REGISTER_ID_OBJECT_TYPE(statue_middle,53)
REGISTER_ID_OBJECT_TYPE(statue_big,54)
REGISTER_ID_OBJECT_TYPE(low_bridge,55)
REGISTER_ID_OBJECT_TYPE(high_bridge,56)
REGISTER_ID_OBJECT_TYPE(shipyard,57)
REGISTER_ID_OBJECT_TYPE(dock,58)
REGISTER_ID_OBJECT_TYPE(wharf,59)

// health
REGISTER_ID_OBJECT_TYPE(clinic,60)
REGISTER_ID_OBJECT_TYPE(hospital,61)
REGISTER_ID_OBJECT_TYPE(baths,62)
REGISTER_ID_OBJECT_TYPE(barber,63)

// education
REGISTER_ID_OBJECT_TYPE(school,64)
REGISTER_ID_OBJECT_TYPE(academy,65)
REGISTER_ID_OBJECT_TYPE(library,66)
REGISTER_ID_OBJECT_TYPE(missionaryPost,67)

// religion: small temples
REGISTER_ID_OBJECT_TYPE(small_ceres_temple,68)
REGISTER_ID_OBJECT_TYPE(small_mars_temple,69)
REGISTER_ID_OBJECT_TYPE(small_neptune_temple,70)
REGISTER_ID_OBJECT_TYPE(small_mercury_temple,71)
REGISTER_ID_OBJECT_TYPE(small_venus_temple,72)

// religion: big temples
REGISTER_ID_OBJECT_TYPE(big_ceres_temple,73)
REGISTER_ID_OBJECT_TYPE(big_mars_temple,74)
REGISTER_ID_OBJECT_TYPE(big_neptune_temple,75)
REGISTER_ID_OBJECT_TYPE(big_mercury_temple,76)
REGISTER_ID_OBJECT_TYPE(big_venus_temple,77)

// religion: others
REGISTER_ID_OBJECT_TYPE(oracle,78)

// building of natives
REGISTER_ID_OBJECT_TYPE(native_hut,79)
REGISTER_ID_OBJECT_TYPE(native_center,80)
REGISTER_ID_OBJECT_TYPE(native_field,81)

//other
REGISTER_ID_OBJECT_TYPE(triumphal_arch,82)
REGISTER_ID_OBJECT_TYPE(burning_ruins,83)
REGISTER_ID_OBJECT_TYPE(burned_ruins,84)
REGISTER_ID_OBJECT_TYPE(collapsed_ruins,85)
REGISTER_ID_OBJECT_TYPE(plague_ruins,86)
REGISTER_ID_OBJECT_TYPE(reserved_2,87)
REGISTER_ID_OBJECT_TYPE(reserved_3,88)
REGISTER_ID_OBJECT_TYPE(elevation,89)
REGISTER_ID_OBJECT_TYPE(rift,90)
REGISTER_ID_OBJECT_TYPE(river,91)
REGISTER_ID_OBJECT_TYPE(tree,92)
REGISTER_ID_OBJECT_TYPE(waymark,93)
REGISTER_ID_OBJECT_TYPE(terrain,94)
REGISTER_ID_OBJECT_TYPE(water,95)
REGISTER_ID_OBJECT_TYPE(meadow,96)
REGISTER_ID_OBJECT_TYPE(roadBlock,97)
REGISTER_ID_OBJECT_TYPE(farmtile,98)


class Helper : public EnumsHelper<Type>
{
public:
  static Helper& instance();
private:
  Helper();
};

Type findType( const std::string& name );

} //end namespace object

#define REGISTER_CUSTOM_OBJECT_TYPE(type)

#endif  //_CAESARIA_OBJECTS_CONSTANTS_INCLUDE_H_
