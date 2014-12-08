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

#include "minimap_colours.hpp"

namespace constants
{

namespace minimap
{

Colors::Colors(ClimateType climate) {
	if (climate == game::climate::central)
	{
		int colours[][8] = {
			{0x000000}, // background
			{0x6b8a31, 0x738231, 0x6b7129, 0x527929, 0x6b8231, 0x6b7931, 0x636918, 0x737931}, // empty 1
			{0x6b7929, 0x6b7929, 0x7b8a39, 0x6b7121, 0x737931, 0x737929, 0x526921, 0x737929}, // empty 2
			{0x6B8231, 0x103808, 0x103008, 0x737931}, // tree 1
			{0x102008, 0x737929, 0x526921, 0x084910}, // tree 2
			{0x948284, 0xA59A94, 0xB5AAA5, 0x63594A}, // rock 1
			{0x63594A, 0xB5AAA5, 0x8C8284, 0xA59A94}, // rock 2
			{0x39497B, 0x39497B, 0x313873, 0x31417B}, // water 1
			{0x31417B, 0x314173, 0x314173, 0x39497B}, // water 2
			{0xD6BA63, 0x948A39, 0xD6BA63, 0x848231}, // fertile 1
			{0x9C8A39, 0xD6BA63, 0x9C9239, 0xADA24A}, // fertile 2
			{0x736963, 0x4A3021}, // road
			{0xD6D3C6, 0xF7F3DE}, // wall
			{0x84BAFF, 0x5282BD}, // aqua buildings
			{0xFFA273, 0xEF824A, 0xFFB28C, 0xD65110}, // housing
			{0xFFFBDE, 0xEFD34A, 0xFFEBB5, 0xFFF3C6}, // buildings
			{0x000000, 0xF70000, 0x7B0000, 0x1800FF}, // sprites: wolves, soldiers, enemy1, enemy2
		};

		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				map[i][j] = colours[i][j];
			}
		}
	}
	else if (climate == game::climate::northen)
	{
		int colours[][8] = {{0x000000}, // background
			{0x4A8231, 0x527929, 0x526929, 0x397121, 0x527931, 0x4A7129, 0x4A6918, 0x527929}, // empty 1
			{0x4A7129, 0x4A7129, 0x5A8239, 0x4A6921, 0x5A7131, 0x5A7129, 0x316921, 0x527129}, // empty 2
			{0x527931, 0x083808, 0x082808, 0x527929}, // tree 1
			{0x082008, 0x5A7129, 0x316921, 0x084921}, // tree 2
			{0x8C8284, 0x9C9A94, 0xA5A2A5, 0x5A5152}, // rock 1
			{0x5A5152, 0xA5A2A5, 0x848284, 0x9C9A94}, // rock 2
			{0x39497B, 0x39497B, 0x313873, 0x31417B}, // water 1
			{0x31417B, 0x314173, 0x314173, 0x39497B}, // water 2
			{0x427118, 0xB5AA4A, 0x8C8A39, 0x527131}, // fertile 1
			{0x8C9242, 0x738A39, 0x6B7929, 0x5A8242}, // fertile 2
			{0x736963, 0x4A3021}, // road
			{0xD6D3C6, 0xF7F3DE}, // wall
			{0x84BAFF, 0x5282BD}, // aqua buildings
			{0xFFA273, 0xEF824A, 0xFFB28C, 0xD65110}, // housing
			{0xFFFBDE, 0xEFD34A, 0xFFEBB5, 0xFFF3C6}, // buildings
			{0x000000, 0xF70000, 0x7B0000, 0x1800FF}, // sprites: wolves, soldiers, enemy1, enemy2
		};

		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				map[i][j] = colours[i][j];
			}
		}
	}
	else if (climate == game::climate::desert)
	{
		int colours[][8] = {{0x000000}, // background
			{0xBDBA9C, 0xC6BAA5, 0xBDBA9C, 0xD6CBAD, 0xA59A7B, 0xCECBAD, 0xC6C3A5, 0xCECBAD}, // empty 1
			{0xB5B294, 0xBDBAA5, 0xC6C3AD, 0xC6BA9C, 0xBDB294, 0xB5AA94, 0xDEDBBD, 0xD6D3B5}, // empty 2
			{0xA59A7B, 0x214110, 0x526921, 0xA59A7B}, // tree 1
			{0x6B7918, 0xADA273, 0xCEC3A5, 0x316121}, // tree 2
			{0xA59294, 0xA59A94, 0xB5AAA5, 0x736152}, // rock 1
			{0x736152, 0xB5AAA5, 0x8C827B, 0xBDAAA5}, // rock 2
			{0x4A82C6, 0x4A82C6, 0x4A82C6, 0x4A79BD}, // water 1
			{0x4A79C6, 0x4A79C6, 0x5282C6, 0x4A79C6}, // water 2
			{0x739A31, 0x7BB229, 0x9CBA52, 0x7BA239}, // fertile 1
			{0x9CBA52, 0x9CBA52, 0x8C924A, 0x739A31}, // fertile 2
			{0x736963, 0x4A3021}, // road
			{0xD6D3C6, 0xF7F3DE}, // wall
			{0x84BAFF, 0x5282BD}, // aqua buildings
			{0xFFA273, 0xEF824A, 0xFFB28C, 0xD65110}, // housing
			{0xFFFBDE, 0xEFD34A, 0xFFEBB5, 0xFFF3C6}, // buildings
			{0x000000, 0xF70000, 0x7B0000, 0x1800FF}, // sprites: wolves, soldiers, enemy1, enemy2
		};

		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				map[i][j] = colours[i][j];
			}
		}
	}
}

int Colors::colour(int type, int number)
{
	return map[type][number];
}


}//end namespace minimap

}//end namespace constants
