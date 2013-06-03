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

#include "oc3_service_building.hpp"

class WaterSource;
class Aqueduct;
class Reservoir;

class WaterSource : public Construction
{
public:
  WaterSource( const BuildingType type, const Size& size );
  
  virtual void updateAqueducts() = 0;
  virtual void link(Aqueduct&) = 0;
  virtual void link(Reservoir&) = 0;

  WaterSource *_north, *_east, *_south, *_west; // public is bad...
};

class Aqueduct : public WaterSource
{
public:
  Aqueduct();

  void build(const TilePos& pos );
  Picture& computePicture();
  void setTerrain(TerrainTile &terrain);
  bool canBuild(const TilePos& pos ) const;
  void updateAqueducts();
  void updatePicture();
  void link(Aqueduct&);
  void link(Reservoir&);
  void destroy();
};

class Reservoir : public WaterSource
{
public:
  Reservoir();
  ~Reservoir();

  void build(const TilePos& pos );
  bool canBuild(const TilePos& pos ) const;

  void setTerrain(TerrainTile &terrain);
  void timeStep(const unsigned long time);
  void updateAqueducts();
  void link(Aqueduct&);
  void link(Reservoir&);
  void destroy();
  
private:
  bool _mayAnimate;
  bool _isNearWater( const TilePos& pos ) const;
};

class BuildingFountain : public ServiceBuilding
{
public:
  BuildingFountain();

  void build( const TilePos& pos );
  bool canBuild( const TilePos& pos ) const;
  void deliverService();
  void timeStep(const unsigned long time);
private:
  bool _mayAnimate;
};

#endif // __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__
