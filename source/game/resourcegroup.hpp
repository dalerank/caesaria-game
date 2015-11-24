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
  __RCG_NAME panelBackground;
  __RCG_NAME menuMiddleIcons;
  __RCG_NAME festivalimg;
  __RCG_NAME land2a;
  __RCG_NAME land3a;
  __RCG_NAME animals;
  __RCG_NAME sprites;
  __RCG_NAME buildingEngineer;
  __RCG_NAME utilitya;
  __RCG_NAME commerce;
  __RCG_NAME security;
  __RCG_NAME transport;
  __RCG_NAME road;
  __RCG_NAME land1a;
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
  //__RCG_NAME lion;
  __RCG_NAME wall;
  __RCG_NAME hippodrome;
  __RCG_NAME celts;
  __RCG_NAME roadBlock;
  __RCG_NAME tradecenter;
};

namespace config
{

struct ID
{
#define STR_ALIAS(a,b) const std::string a=#b;

  struct _Menu
  {
    int house = 123;
    int clear = 131;
    int road = 135;
    int waterSupply = 127;
    int terrain = 183;
    int forest = 186;
    int rowDown = 601;
    int rowUp = 605;
    int water = 189;
    int smRocks = 204;
    int helpInf = 528;
    int arrowDown = 601;
    int arrowUp = 605;
    int exitInf = 532;
    int ok = 239;
    int maximize = 101;
    int cancel = 243;
  };

  struct _Middle
  {
    int house = 1;
    int clear = 12;
    int road = 11;
    int water = 4;
    int health = 6;
    int religion = 2;
    int education = 7;
    int administration = 3;
    int entertainment = 5;
    int engineer = 8;
    int security = 9;
    int comerce = 10;
    int empty = 13;
  };

  struct _Empire
  {
    int	bad = 0;
    int stamp = 543;
    int baseLocalGoodId = 316;
    int baseEmpireGoodId = 10;
    int grassPicsNumber = 57;
    int seaTradeRoute = 69;
    int landTradeRoute = 70;
    int romeCity=8;
    int distantCity=22;
    int rightPanelTx=14;
    int otherCity=15;
    int grassPic=62;
    int coastNE=144;
    int coastSE=148;
  };

  _Menu menu;
  _Middle middle;
  _Empire empire;  
};

struct {
  int constr = 1;
  int skipRightBorder = 1;
  int skipLeftBorder = 2;
  int house = 4;
} tile;

struct {
  STR_ALIAS(aqueduct,land2a)
} rc;

struct
{
  int reservoirRange = 8;
  int haveWater = 16;

  STR_ALIAS(water,land2a)
  STR_ALIAS(food,land2a)
  STR_ALIAS(ground,land2a)
} layer;

static ID id;

}

#endif //__CAESARIA_RESOURCENAMES_H_INCLUDE_
