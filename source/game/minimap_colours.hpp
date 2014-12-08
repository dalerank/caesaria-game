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

#ifndef __CAESARIA_MINIMAP_COLOURS_H_INCLUDE_
#define __CAESARIA_MINIMAP_COLOURS_H_INCLUDE_

#include "enums.hpp"
#include "game/climate.hpp"

namespace constants
{

namespace minimap
{

class Colors {
	public:
		/**
		* Constants for the different terrain and building elements
		*/
		const static int
			background = 0,
			MAP_EMPTY1     = 1,
			MAP_EMPTY2     = 2,
			MAP_TREE1      = 3,
			MAP_TREE2      = 4,
			MAP_ROCK1      = 5,
			MAP_ROCK2      = 6,
			MAP_WATER1     = 7,
			MAP_WATER2     = 8,
			MAP_FERTILE1   = 9,
			MAP_FERTILE2   = 10,
			MAP_ROAD       = 11,
			MAP_WALL       = 12,
			MAP_AQUA       = 13,
			MAP_HOUSE      = 14,
			MAP_BUILDING   = 15,
			MAP_SPRITES    = 16,
			MAP_SIZE       = 17;

		/**
		* Constants for the different walkers / sprites
		*/
		const static int
			SPRITE_WOLF      = 0,
			SPRITE_SOLDIER   = 1,
			SPRITE_BARBARIAN = 2,
			SPRITE_ENEMY     = 3;
	private:
		int map[MAP_SIZE][8];

	public:
		/**
		* Constructor - creates a new colour set with the given climate
		* @param climate - one of the CLIMATE_* constants
		*/
		Colors(ClimateType climate);

		/**
		* Retrieves a specified colour from this set.
		* @param type - one of the MAP_* constants
		* @param number - 0-7, or one of the SPRITE_* constants
		* @return int representing a colour
		*/
		int colour(int type, int number);
};

}//end namespace minimap

}//end namespace constants

#endif // __CAESARIA_MINIMAP_COLOURS_H_INCLUDE_
