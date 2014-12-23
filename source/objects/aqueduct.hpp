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

#ifndef __CAESARIA_AQUEDUCT_H_INCLUDED__
#define __CAESARIA_AQUEDUCT_H_INCLUDED__

#include "watersupply.hpp"

class Aqueduct : public WaterSource
{
public:
  Aqueduct();

  virtual bool build( const CityAreaInfo& info );
  virtual void addWater(const WaterSource &source);
  virtual void initTerrain( gfx::Tile& terrain);
  virtual bool canBuild(const CityAreaInfo& areaInfo ) const;
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
  virtual void timeStep(const unsigned long time);
  virtual bool isWalkable() const; 
  virtual void changeDirection( gfx::Tile* masterTile, constants::Direction direction);
  virtual std::string sound() const;
  virtual const gfx::Picture& picture() const;

  void updatePicture(PlayerCityPtr city);
  void addRoad();
  bool canAddRoad( PlayerCityPtr city, TilePos pos ) const;

  virtual const gfx::Picture& picture(const CityAreaInfo& info) const;
protected:
  virtual void _waterStateChanged();
};

#endif // __CAESARIA_AQUEDUCT_H_INCLUDED__
