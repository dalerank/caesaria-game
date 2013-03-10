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


#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>

enum DirectionType { D_NONE, D_NORTH, D_NORTH_WEST, D_WEST, D_SOUTH_WEST, D_SOUTH, D_SOUTH_EAST, D_EAST, D_NORTH_EAST, D_MAX };
enum WalkerActionType { WA_NONE, WA_MOVE, WA_FIGHT, WA_DIE, WA_MAX };
enum WalkerGraphicType { WG_NONE, WG_POOR, WG_BATH, WG_PRIEST, WG_ACTOR, WG_TAMER, WG_TAX, WG_CHILD, WG_MARKETLADY, WG_PUSHER, WG_PUSHER2, WG_ENGINEER, WG_GLADIATOR, WG_GLADIATOR2, WG_RIOTER, WG_BARBER, WG_PREFECT, WG_HOMELESS, WG_RICH, WG_RICH2, WG_DOCTOR, WG_LIBRARIAN, WG_JAVELINEER, WG_SOLDIER, WG_HORSEMAN, WG_HORSE_CARAVAN, WG_CAMEL_CARAVAN, WG_LITTLE_HELPER, WG_MAX };
enum WalkerType { WT_NONE, WT_IMMIGRANT, WT_SOLDIER, WT_CART_PUSHER, WT_MARKET_BUYER, WT_SERVICE, WT_TRAINEE, WT_MAX };
enum GoodType { G_NONE, G_WHEAT, G_FISH, G_MEAT, G_FRUIT, G_VEGETABLE, G_OLIVE, G_OIL, G_GRAPE, G_WINE, G_TIMBER, G_FURNITURE, G_CLAY, G_POTTERY, G_IRON, G_WEAPON, G_MARBLE, G_MAX };
enum ServiceType { S_WELL, S_FOUNTAIN, S_MARKET, S_ENGINEER, S_SENATE, S_FORUM, S_PREFECT, S_TEMPLE_NEPTUNE, S_TEMPLE_CERES, S_TEMPLE_VENUS, S_TEMPLE_MARS, S_TEMPLE_MERCURE, S_TEMPLE_ORACLE, S_DOCTOR, S_BARBER, S_BATHS, S_HOSPITAL, S_SCHOOL, S_LIBRARY, S_COLLEGE, S_THEATER, S_AMPHITHEATER, S_COLLOSSEUM, S_HIPPODROME, S_MAX };
enum ClimateType { C_CENTRAL, C_NORTHERN, C_DESERT, C_MAX };
enum ScreenType { SCREEN_NONE, SCREEN_MENU, SCREEN_GAME, SCREEN_QUIT, SCREEN_MAX };
enum WalkerTraineeType { WTT_NONE, WTT_ACTOR, WTT_GLADIATOR, WTT_TAMER, WTT_CHARIOT, WTT_MAX };

enum BuildingType
{
   B_NONE,
   B_AMPHITHEATER, B_THEATER, B_HIPPODROME, B_COLLOSSEUM, B_ACTOR, B_GLADIATOR, B_LION, B_CHARIOT,   // entertainment
   B_HOUSE, B_ROAD, B_PLAZA, B_GARDEN,  // road & house
   B_SENATE, B_FORUM,  // administration
   B_WELL, B_FOUNTAIN,  // water
   B_PREFECT,   // security
   B_MARKET, B_GRANARY, B_WAREHOUSE,   // commerce
   B_WHEAT, B_FRUIT, B_VEGETABLE, B_OLIVE, B_GRAPE, B_MEAT,   // farms
   B_MARBLE, B_IRON, B_TIMBER, B_CLAY,   // raw materials
   B_WINE, B_OIL, B_WEAPON, B_FURNITURE, B_POTTERY,  // factories
   B_ENGINEER, B_STATUE1, B_STATUE2, B_STATUE3, B_LOW_BRIDGE, B_HIGH_BRIDGE,  // utility
   B_DOCTOR, B_HOSPITAL, B_BATHS, B_BARBER,   // health
   B_SCHOOL, B_COLLEGE, B_LIBRARY,  // education
   B_TEMPLE_CERES, B_TEMPLE_MARS, B_TEMPLE_NEPTUNE, B_TEMPLE_MERCURE, B_TEMPLE_VENUS, // religion: small temples
   B_BIG_TEMPLE_CERES, B_BIG_TEMPLE_MARS, B_BIG_TEMPLE_NEPTUNE, B_BIG_TEMPLE_MERCURE, B_BIG_TEMPLE_VENUS, // religion: big temples
   B_TEMPLE_ORACLE, // religion: others
   B_MAX
};
enum BuildMenuType { BM_NONE, BM_WATER, BM_HEALTH, BM_SECURITY, BM_EDUCATION, BM_ENGINEERING, BM_ADMINISTRATION, BM_ENTERTAINMENT, BM_COMMERCE, BM_FARM, BM_RAW_MATERIAL, BM_FACTORY, BM_TEMPLE, BM_BIGTEMPLE, BM_MAX };

enum WidgetEventType
{
   WE_NONE,
   WE_NewGame, WE_SaveGame, WE_LoadGame, WE_QuitGame,
   WE_OptionsMenu, // game options
   WE_InGameMenu,  // screen_game: options/saveGame/quitGame
   WE_BuildMenu,   // screen_game: select a group of buildings (security buildings/...)
   WE_Building,    // screen_game: select a building (prefecture/...)
   WE_ClearLand,   // screen_game: clear some land (building, tree, ...)
   WE_MAX
};


#endif
