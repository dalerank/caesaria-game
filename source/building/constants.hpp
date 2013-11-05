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

#include "building.hpp"

namespace constants
{

namespace construction
{
// road
const TileOverlay::Type B_ROAD=10;
const TileOverlay::Type B_PLAZA=11;
const TileOverlay::Type B_GARDEN=12;
}

namespace building
{
const TileOverlay::Group unknownGroup=0;
const TileOverlay::Group industryGroup=1;
const TileOverlay::Group obtainGroup=2;
const TileOverlay::Group foodGroup=3;
const TileOverlay::Group disaster=4;
const TileOverlay::Group religionGroup=5;
const TileOverlay::Group militaryGroup=6;
const TileOverlay::Group nativeGroup=7;
const TileOverlay::Group water=8;
const TileOverlay::Group administration=9;
const TileOverlay::Group BC_BRIDGE=10;
const TileOverlay::Group BC_ENGINEERING=11;
const TileOverlay::Group BC_TRADE=12;
const TileOverlay::Group BC_TOWER=13;
const TileOverlay::Group BC_GATE=14;
const TileOverlay::Group BC_SECURITY=15;
const TileOverlay::Group BC_EDUCATUION=16;
const TileOverlay::Group BC_HEALTH=17;
const TileOverlay::Group BC_SIGHT=18;
const TileOverlay::Group BC_GARDEN=19;
const TileOverlay::Group BC_ROAD=20;
const TileOverlay::Group BC_ENTERTAINMENT=21;
const TileOverlay::Group BC_HOUSE=22;
const TileOverlay::Group BC_WALL=23;
const TileOverlay::Group BC_MAX=24;

//unknown building
const TileOverlay::Type unknown=0;

// entertainment
const TileOverlay::Type amphitheater=1;
const TileOverlay::Type theater=2;
const TileOverlay::Type hippodrome=3;
const TileOverlay::Type colloseum=4;
const TileOverlay::Type actorColony=5;
const TileOverlay::Type gladiatorSchool=6;
const TileOverlay::Type lionHouse=7;
const TileOverlay::Type chariotSchool=8;

// house
const TileOverlay::Type house=9;

// administration
const TileOverlay::Type senate=13;
const TileOverlay::Type forum=14;
const TileOverlay::Type governorHouse=15;
const TileOverlay::Type governorVilla=16;
const TileOverlay::Type governorPalace=17;

// water
const TileOverlay::Type B_RESERVOIR=18;
const TileOverlay::Type B_AQUEDUCT=19;
const TileOverlay::Type B_FOUNTAIN=20;
const TileOverlay::Type B_WELL=21;

// security
const TileOverlay::Type B_PREFECTURE=22;
const TileOverlay::Type B_WALL=23;
const TileOverlay::Type B_TOWER=24;
const TileOverlay::Type B_GATEHOUSE=25;
const TileOverlay::Type B_FORT_LEGIONNAIRE=26;
const TileOverlay::Type B_FORT_JAVELIN=27;
const TileOverlay::Type B_FORT_MOUNTED=28;
const TileOverlay::Type B_MILITARY_ACADEMY=29;
const TileOverlay::Type B_BARRACKS=30;

// commerce
const TileOverlay::Type B_MARKET=31;
const TileOverlay::Type B_GRANARY=32;
const TileOverlay::Type B_WAREHOUSE=33;

// farms
const TileOverlay::Type B_WHEAT_FARM=34;
const TileOverlay::Type B_FRUIT_FARM=35;
const TileOverlay::Type B_VEGETABLE_FARM=36;
const TileOverlay::Type B_OLIVE_FARM=37;
const TileOverlay::Type B_GRAPE_FARM=38;
const TileOverlay::Type B_PIG_FARM=39;

// raw materials
const TileOverlay::Type B_MARBLE_QUARRY=40;
const TileOverlay::Type B_IRON_MINE=41;
const TileOverlay::Type B_TIMBER_YARD=42;
const TileOverlay::Type B_CLAY_PIT=43;

// factories
const TileOverlay::Type B_WINE_WORKSHOP=44;
const TileOverlay::Type B_OIL_WORKSHOP=45;
const TileOverlay::Type B_WEAPONS_WORKSHOP=46;
const TileOverlay::Type B_FURNITURE=47;
const TileOverlay::Type B_POTTERY=48;

// utility
const TileOverlay::Type B_ENGINEER_POST=49;
const TileOverlay::Type B_STATUE1=50;
const TileOverlay::Type B_STATUE2=51;
const TileOverlay::Type B_STATUE3=52;
const TileOverlay::Type B_LOW_BRIDGE=53;
const TileOverlay::Type B_HIGH_BRIDGE=54;
const TileOverlay::Type B_SHIPYARD=55;
const TileOverlay::Type B_DOCK=56;
const TileOverlay::Type B_WHARF=57;

// health
const TileOverlay::Type B_DOCTOR=58;
const TileOverlay::Type B_HOSPITAL=59;
const TileOverlay::Type B_BATHS=60;
const TileOverlay::Type B_BARBER=61;

// education
const TileOverlay::Type B_SCHOOL=62;
const TileOverlay::Type B_COLLEGE=63;
const TileOverlay::Type B_LIBRARY=64;
const TileOverlay::Type B_MISSION_POST=65;

// religion: small temples
const TileOverlay::Type B_TEMPLE_CERES=66;
const TileOverlay::Type B_TEMPLE_MARS=67;
const TileOverlay::Type B_TEMPLE_NEPTUNE=68;
const TileOverlay::Type B_TEMPLE_MERCURE=69;
const TileOverlay::Type B_TEMPLE_VENUS=70;

// religion: big temples
const TileOverlay::Type B_BIG_TEMPLE_CERES=71;
const TileOverlay::Type B_BIG_TEMPLE_MARS=72;
const TileOverlay::Type B_BIG_TEMPLE_NEPTUNE=73;
const TileOverlay::Type B_BIG_TEMPLE_MERCURE=74;
const TileOverlay::Type B_BIG_TEMPLE_VENUS=75;

// religion: others
const TileOverlay::Type B_TEMPLE_ORACLE=76;

// building of natives
const TileOverlay::Type B_NATIVE_HUT=77;
const TileOverlay::Type B_NATIVE_CENTER=78;
const TileOverlay::Type B_NATIVE_FIELD=79;

//other
const TileOverlay::Type B_TRIUMPHAL_ARCH=80;
const TileOverlay::Type B_BURNING_RUINS=81;
const TileOverlay::Type B_BURNED_RUINS=82;
const TileOverlay::Type B_COLLAPSED_RUINS=83;
const TileOverlay::Type B_PLAGUE_RUINS=84;

const TileOverlay::Type B_FORUM_2=85;
const TileOverlay::Type B_SENATE_2=86;
}

}

#endif  //_OPENCAESAR3_BUILDING_CONSTANTS_INCLUDE_H_
