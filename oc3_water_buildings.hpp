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

class WaterSource
{
public:
  virtual void updateAqueducts() = 0;
protected:
  WaterSource *_north, *_east, *_south, *_west;
};

class Aqueduct : public Construction, public WaterSource
{
public:
  Aqueduct();
  virtual Aqueduct* clone() const;

  virtual void build(const int i, const int j);
  Picture& computePicture();
  virtual void setTerrain(TerrainTile &terrain);
  virtual bool canBuild(const int i, const int j) const;
  void updateAqueducts();
};


class Reservoir : public Construction, public WaterSource
{
public:
  Reservoir();
  ~Reservoir();
  virtual Reservoir* clone() const;

  void build(const int i, const int j);
  Picture& computePicture();
  void setTerrain(TerrainTile &terrain);
  void timeStep(const unsigned long time);
  void updateAqueducts();
private:
  bool _mayAnimate;
  int _lastTimeAnimate;
};

#endif // __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__
