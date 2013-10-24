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

#include "core/position.hpp"
#include "oc3_building_service.hpp"

class WaterSource;
class Aqueduct;
class Reservoir;

class WaterSource : public Construction
{
public:
  WaterSource( const TileOverlayType type, const Size& size );
  
  virtual void addWater( const WaterSource& source );
  virtual bool haveWater() const;
  virtual void timeStep(const unsigned long time);
  int getId() const;

  virtual std::string getError() const;

protected:
  void _setError( const std::string& error );
  virtual void _waterStateChanged() {}
  virtual void _produceWater( const TilePos* points, const int size );
  
  class Impl;
  ScopedPtr< Impl > _d;
};



class Aqueduct : public WaterSource
{
public:
  Aqueduct();

  virtual void build(CityPtr city, const TilePos& pos );
  Picture& computePicture(CityPtr city,
                          const TilemapTiles * tmp = NULL,
                          const TilePos pos = TilePos(0, 0));
  virtual void initTerrain(Tile& terrain);
  virtual bool canBuild(CityPtr city, const TilePos& pos ) const;
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
  virtual bool isWalkable() const; 
  virtual bool isRoad() const;

  void updatePicture(CityPtr city);
  void addWater( const WaterSource& source );

protected:
  virtual void _waterStateChanged();
};

class Reservoir : public WaterSource
{
public:
  Reservoir();
  ~Reservoir();

  virtual void build(CityPtr city, const TilePos& pos );
  virtual bool canBuild(CityPtr city, const TilePos& pos ) const;
  virtual bool isNeedRoadAccess() const;
  virtual void initTerrain(Tile& terrain);
  virtual void timeStep(const unsigned long time);
  virtual void destroy();

private:
  bool _isWaterSource;
  bool _isNearWater( CityPtr city, const TilePos& pos ) const;
};

class Fountain : public ServiceBuilding
{
public:
  Fountain();

  virtual void build( CityPtr city, const TilePos& pos );
  virtual bool canBuild(CityPtr city, const TilePos& pos ) const;
  virtual void deliverService();
  virtual void timeStep(const unsigned long time);
  virtual bool isNeedRoadAccess() const;

  virtual bool isActive() const;
  virtual bool haveReservoirAccess() const;

  virtual void load( const VariantMap& stream);
private:
  bool _haveReservoirWater;
  void _initAnimation();
};

#endif // __OPENCAESAR3_WATER_BUILDGINDS_INCLUDED__
