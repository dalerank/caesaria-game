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


#ifndef __OPENCAESAR3_RESOURCENAMES_H_INCLUDE_
#define __OPENCAESAR3_RESOURCENAMES_H_INCLUDE_

class ResourceGroup
{
public:
  static const char* panelBackground;
  static const char* menuMiddleIcons;
  static const char* festivalimg;
  static const char* land2a;
  static const char* land3a;
  static const char* animals;
  static const char* sprites;
  static const char* buildingEngineer;
  static const char* utilitya;
  static const char* commerce;
  static const char* security;
  static const char* transport;
  static const char* aqueduct;
  static const char* waterbuildings;
  static const char* road;
  static const char* land1a;
  static const char* entertaiment;
  static const char* warehouse;
  static const char* housing;
  static const char* govt;
  static const char* carts;
  static const char* wharf;
  static const char* waterOverlay;
  static const char* foodOverlay;
  static const char* empirebits;
  static const char* empirepnls;
  static const char* citizen1;
  static const char* citizen2;
  static const char* citizen3;
  static const char* citizen4;
  static const char* citizen5;
  static const char* lion;
  static const char* wall;\
  static const char* celts;
};

class ResourceMenu
{
public:
  static const int maximizeBtn = 101;
  static const int houseBtnPicId = 123;
  static const int helpInfBtnPicId = 528;
  static const int exitInfBtnPicId = 532;
  static const int okBtnPicId = 239;
  static const int cancelBtnPicId = 243;

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
  static const int bad = 0;
  static const int empireStamp = 543;
  static const int seaTradeRoute = 69;
  static const int landTradeRoute = 70;
  static const int baseLocalGoodId = 316;
  static const int baseEmpireGoodId = 10;
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

#endif
