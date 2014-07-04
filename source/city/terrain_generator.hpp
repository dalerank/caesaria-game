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

#ifndef _CAESARIA_TERRAIN_GENERATOR_INCLUDE_H_
#define _CAESARIA_TERRAIN_GENERATOR_INCLUDE_H_

#include <vector>
#include <limits>
#include <iostream>

#include "terrain_generator_random.hpp"

class Game;

class MidpointDisplacement
{
 public:
  MidpointDisplacement(int n, int wmult, int hmult, float smoothness, float fineness);
  ~MidpointDisplacement() {}
  std::vector<int> map();
  int width() const { return width_; }
  int height() const { return height_; }

  typedef enum { unknown=0, deepWater=1, water=2, coast=3, grass=4, trees=5, shallowMountain=6, highMountain=7 } TerrainType;
private:
  int CoordinatesToVectorIndex(int x, int y);
  std::pair<int, int> VectorIndexToCoordinates(int i);
  float grass_threshold_;
  float water_threshold_;
  float sand_threshold_;
  float deep_water_threshold_;
  float hills_threshold_;
  float shallow_mountains_threshold_;
  float high_mountains_threshold_;
  int n_;
  int wmult_;
  int hmult_;
  float smoothness_;
  float fineness_;
  int width_;
  int height_;
  Random random_;
};

class TerrainGenerator
{
public:
  static void create( Game& game );
};

#endif //_CAESARIA_TERRAIN_GENERATOR_INCLUDE_H_
