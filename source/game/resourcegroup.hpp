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

#define __RCG_NAME static const char*
class ResourceGroup
{
public:
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
  __RCG_NAME aqueduct;
  __RCG_NAME road;
  __RCG_NAME land1a;
  __RCG_NAME meadow;
  __RCG_NAME entertaiment;
  __RCG_NAME warehouse;
  __RCG_NAME housing;
  __RCG_NAME govt;
  __RCG_NAME carts;
  __RCG_NAME plateau;
  __RCG_NAME wharf;
  __RCG_NAME shipyard;
  __RCG_NAME waterOverlay;
  __RCG_NAME foodOverlay;
  __RCG_NAME empirebits;
  __RCG_NAME empirepnls;
  __RCG_NAME citizen1;
  __RCG_NAME citizen2;
  __RCG_NAME citizen3;
  __RCG_NAME citizen4;
  __RCG_NAME citizen5;
  __RCG_NAME lion;
  __RCG_NAME wall;
  __RCG_NAME hippodrome;
  __RCG_NAME celts;
  __RCG_NAME roadBlock;
};

class ResourceMenu
{
public:
  enum {
    okBtnPicId = 239,
    cancelBtnPicId = 243
  };
  static const int maximizeBtn = 101;
  static const int houseBtnPicId = 123;
  static const int helpInfBtnPicId = 528;
  static const int exitInfBtnPicId = 532;
  static const int houseMidPicId = 1;
  static const int clearMidPicId = 12;
  static const int roadMidPicId = 11;
  static const int waterMidPicId = 4;
  static const int healthMidPicId = 6;
  static const int religionMidPicId = 2;
  static const int educationMidPicId = 7;
  static const int administrationMidPicId = 3;
  static const int entertainmentMidPicId = 5;
  static const int engineerMidPicId = 8;
  static const int securityMidPicId = 9;
  static const int comerceMidPicId = 10;
  static const int emptyMidPicId = 13;
};

class PicID
{
public:
  enum {
  	bad = 0,
    seaTradeRoute = 69,
    landTradeRoute = 70,
    romeCity=8,
    distantCity=22,
    rightPanelTx=14,
    otherCity=15,
    grassPic=62,
    coastNE=144, coastSE=148
  };

  static const int empireStamp = 543;
  static const int baseLocalGoodId = 316;
  static const int baseEmpireGoodId = 10;
  static const int grassPicsNumber = 57;
};

class OverlayPic
{
public:
  static const int base = 1;
  static const int skipRightBorder = 1;
  static const int skipLeftBorder = 2;
  static const int inHouse = 4;
  static const int inHouseBase = 5;
  static const int reservoirRange = 8;
  static const int haveWater = 16;
};

#endif //__CAESARIA_RESOURCENAMES_H_INCLUDE_
