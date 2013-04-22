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

#ifndef __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__
#define __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__

#include "oc3_building.hpp"

class WaterSource;
class Aqueduct;
class Reservoir;

class WaterSource
{
public:
  WaterSource() {_north = NULL; _east = NULL; _south = NULL; _west = NULL;}
  
  virtual void updateAqueducts() = 0;
  virtual void link(Aqueduct&) = 0;
  virtual void link(Reservoir&) = 0;

  WaterSource *_north, *_east, *_south, *_west; // public is bad...
};

class Aqueduct : public Construction, public WaterSource
{
public:
  Aqueduct();
  virtual Aqueduct* clone() const;

  virtual void build(const TilePos& pos );
  Picture& computePicture();
  virtual void setTerrain(TerrainTile &terrain);
  virtual bool canBuild(const TilePos& pos ) const;
  void updateAqueducts();
  void updatePicture();
  virtual void link(Aqueduct&);
  virtual void link(Reservoir&);
  virtual void destroy();
};

class Reservoir : public Construction, public WaterSource
{
public:
  Reservoir();
  ~Reservoir();
  virtual Reservoir* clone() const;

  void build(const TilePos& pos );
  bool canBuild(const TilePos& pos ) const;
  Picture& computePicture();
  void setTerrain(TerrainTile &terrain);
  void timeStep(const unsigned long time);
  void updateAqueducts();
  virtual void link(Aqueduct&);
  virtual void link(Reservoir&);
  virtual void destroy();
  
private:
  bool _mayAnimate;
  bool _isNearWater( const TilePos& pos ) const;
};

#endif // __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__
