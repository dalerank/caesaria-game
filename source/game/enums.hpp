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


#ifndef _OPENCAESAR3_ENUMS_INCLUDE_H_
#define _OPENCAESAR3_ENUMS_INCLUDE_H_

#include <string>

enum WalkerGraphicType 
{
  WG_NONE, WG_POOR, WG_BATH, WG_PRIEST, WG_ACTOR, WG_TAMER, WG_TAX, WG_CHILD, WG_MARKETLADY, 
  WG_PUSHER, WG_PUSHER2, 
  WG_ENGINEER, 
  WG_GLADIATOR, WG_GLADIATOR2, 
  WG_RIOTER, WG_BARBER,
  WG_PREFECT, WG_PREFECT_DRAG_WATER, WG_PREFECT_FIGHTS_FIRE, WG_PREFECT_FIGHT,
  WG_HOMELESS, 
  WG_RICH, WG_RICH2,
  WG_DOCTOR, WG_TEACHER, WG_JAVELINEER,
  WG_SOLDIER, 
  WG_HORSEMAN, WG_HORSE_CARAVAN, WG_CAMEL_CARAVAN, WG_MARKETLADY_HELPER, 
  WG_ANIMAL_SHEEP_WALK,
  WG_FISHING_BOAT, WG_FISHING_BOAT_WORK,
  WG_HOMELESS_SIT,
  WG_LION,
  WG_CHARIOT,
  WG_MAX 
};

enum WaterService { WTR_WELL=0, WTR_FONTAIN, WTR_RESERVOIR, WTR_COUNT };

enum ClimateType { C_CENTRAL, C_NORTHERN, C_DESERT, C_MAX };

enum ScreenType { SCREEN_NONE, SCREEN_MENU, SCREEN_GAME, SCREEN_QUIT, SCREEN_MAX };

enum BuildMenuType 
{ 
  BM_NONE, 
  BM_WATER, 
  BM_HEALTH, 
  BM_SECURITY, 
  BM_EDUCATION, 
  BM_ENGINEERING, 
  BM_ADMINISTRATION, 
  BM_ENTERTAINMENT, 
  BM_COMMERCE, 
  BM_FARM, 
  BM_RAW_MATERIAL, 
  BM_FACTORY, 
  BM_RELIGION, 
  BM_TEMPLE, 
  BM_BIGTEMPLE, 
  BM_MAX 
};

enum AdvisorType
{
  ADV_EMPLOYERS=0,
  ADV_LEGION,
  ADV_EMPIRE,
  ADV_RATINGS,
  ADV_TRADING,
  ADV_POPULATION,
  ADV_HEALTH,
  ADV_EDUCATION,
  ADV_ENTERTAINMENT,
  ADV_RELIGION,
  ADV_FINANCE,
  ADV_MAIN,
  ADV_COUNT
};

#endif  //_OPENCAESAR3_ENUMS_INCLUDE_H_
