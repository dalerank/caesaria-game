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

#ifndef __OPENCAESAR3_ROAD_H_INCLUDE__
#define __OPENCAESAR3_ROAD_H_INCLUDE__

#include "oc3_building.hpp"

class Road : public Construction
{
public:
  Road();

  virtual Picture& computePicture();
  void updatePicture();

  virtual void build(const TilePos& pos );
  virtual void setTerrain(TerrainTile &terrain);
  virtual bool canBuild(const TilePos& pos ) const;
  virtual bool isWalkable() const;
  virtual bool isNeedRoadAccess() const;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual void setTerrain(TerrainTile &terrain);  
  virtual bool canBuild(const TilePos& pos ) const;
  virtual Picture& computePicture();
};


#endif //__OPENCAESAR3_ROAD_H_INCLUDE__
