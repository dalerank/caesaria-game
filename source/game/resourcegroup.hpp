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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_RESOURCENAMES_H_INCLUDE_
#define __CAESARIA_RESOURCENAMES_H_INCLUDE_

#include <string>

class ResourceGroup
{
public:
#define __RCG_NAME static const char*
  __RCG_NAME sprites;
  __RCG_NAME utilitya;
  __RCG_NAME commerce;
  __RCG_NAME security;
  __RCG_NAME transport;
  __RCG_NAME road;
  __RCG_NAME meadow;
  __RCG_NAME entertainment;
  __RCG_NAME warehouse;
  __RCG_NAME housing;
  __RCG_NAME govt;
  __RCG_NAME carts;
  __RCG_NAME plateau;
  __RCG_NAME wharf;
  __RCG_NAME shipyard;
  __RCG_NAME empirebits;
  __RCG_NAME empirepnls;
  __RCG_NAME citizen1;
  __RCG_NAME citizen2;
  __RCG_NAME citizen3;
  __RCG_NAME citizen4;
  __RCG_NAME citizen5;
  __RCG_NAME wall;
  __RCG_NAME hippodrome;
  __RCG_NAME celts;
  __RCG_NAME roadBlock;
  __RCG_NAME tradecenter;
};

namespace config
{

#define STR_ALIAS(a,b) std::string a{#b};
#define INT_ALIAS(a,b) int a=b;
struct ID
{
  struct _Empire
  {
    INT_ALIAS(bad, 0)
    INT_ALIAS(stamp,543)
    INT_ALIAS(baseLocalGoodId,316)
    INT_ALIAS(baseEmpireGoodId,10)
    INT_ALIAS(grassPicsNumber,57)
    INT_ALIAS(seaTradeRoute,69)
    INT_ALIAS(landTradeRoute,70)
    INT_ALIAS(romeCity,8)
    INT_ALIAS(distantCity,22)
    INT_ALIAS(rightPanelTx,14)
    INT_ALIAS(otherCity,15)
    INT_ALIAS(grassPic,62)
    INT_ALIAS(coastNE,144)
    INT_ALIAS(coastSE,148)
  };

  _Empire empire;
};

struct _Tile {
  INT_ALIAS(constr,1)
  INT_ALIAS(skipRightBorder,1)
  INT_ALIAS(skipLeftBorder,2)
  INT_ALIAS(house,5)
};

struct _RC {
  STR_ALIAS(aqueduct,land2a)
  STR_ALIAS(animals,citizen04)
  STR_ALIAS(land1a,land1a)
  STR_ALIAS(land2a,land2a)
  STR_ALIAS(land3a,land3a)
  STR_ALIAS(bigpeople,bigpeople)
};

struct _Layer
{
  INT_ALIAS(reservoirRange,8)
  INT_ALIAS(haveWater,16)

  STR_ALIAS(water,land2a)
  STR_ALIAS(food,land2a)
  STR_ALIAS(ground,land2a)
};

static _Tile tile;
static _RC rc;
static _Layer layer;
static ID id;

};

namespace gui
{

struct _RC{
  STR_ALIAS(panel,paneling)
};

struct _Miniature{
  INT_ALIAS(house, 1)
  INT_ALIAS(clear,12)
  INT_ALIAS(road,11)
  INT_ALIAS(water,4)
  INT_ALIAS(health,6)
  INT_ALIAS(religion,2)
  INT_ALIAS(education,7)
  INT_ALIAS(administration,3)
  INT_ALIAS(entertainment,5)
  INT_ALIAS(engineering,8)
  INT_ALIAS(security,9)
  INT_ALIAS(commerce,10)
  INT_ALIAS(empty,13)
  STR_ALIAS(rc,panelwindows)
};

struct _Column {
  INT_ALIAS(footer,544)
  INT_ALIAS(header,546)
  INT_ALIAS(body,545)
};

struct _Message {
  INT_ALIAS(simple, 111)
  INT_ALIAS(critial, 113)
};

struct _Button {
  INT_ALIAS(house, 123)
  INT_ALIAS(clear, 131)
  INT_ALIAS(road, 135)
  INT_ALIAS(govt, 139)
  INT_ALIAS(engineering, 167)
  INT_ALIAS(entertainment, 143)
  INT_ALIAS(education, 147)
  INT_ALIAS(meadow, 201)
  INT_ALIAS(river, 189)
  INT_ALIAS(temple, 151)
  INT_ALIAS(broad, 213)
  INT_ALIAS(waterSupply, 127)
  INT_ALIAS(terrain, 183)
  INT_ALIAS(health, 163)
  INT_ALIAS(rift, 192)
  INT_ALIAS(forest, 186)
  INT_ALIAS(plateau, 204)
  INT_ALIAS(rowDown, 601)
  INT_ALIAS(rowUp, 605)
  INT_ALIAS(water, 189)
  INT_ALIAS(rocks, 198)
  INT_ALIAS(help, 528)
  INT_ALIAS(arrowDown, 601)
  INT_ALIAS(arrowUp, 605)
  INT_ALIAS(exit, 532)
  INT_ALIAS(ok, 239)
  INT_ALIAS(indigene, 210)
  INT_ALIAS(maximize, 101)
  INT_ALIAS(cancel, 243)
  INT_ALIAS(security, 159)
  INT_ALIAS(waymark, 216)
  INT_ALIAS(commerce, 155)
  INT_ALIAS(attacks, 225)
  INT_ALIAS(undo, 171)
};

struct _ID{ 
  INT_ALIAS(chiefIcon,48)
  INT_ALIAS(lockpick,238)
  INT_ALIAS(gotoLegion,563)
  INT_ALIAS(return2fort,564)
  INT_ALIAS(serv2empire,566)
  INT_ALIAS(wrathIcon,334)
};

static _RC rc;
static _Miniature miniature;
static _Button button;
static _Message message;
static _ID id;
static _Column column;

}//end namespace gui

#endif //__CAESARIA_RESOURCENAMES_H_INCLUDE_
