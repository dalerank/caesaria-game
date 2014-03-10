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

#ifndef __CAESARIA_WATER_BUILDGINDS_INCLUDED__
#define __CAESARIA_WATER_BUILDGINDS_INCLUDED__

#include "core/position.hpp"
#include "service.hpp"

class WaterSource : public Construction
{
public:
  WaterSource( const TileOverlay::Type type, const Size& size );
  ~WaterSource();
  
  virtual void addWater( const WaterSource& source );
  virtual bool haveWater() const;
  virtual void timeStep(const unsigned long time);
  int getId() const;

  virtual std::string getError() const;

protected:
  void _setError( const std::string& error );
  virtual void _waterStateChanged() {}
  virtual void _produceWater( const TilePos* points, const int size );
  void _setIsRoad( bool value );
  void _setResolved( bool value );
  bool _isResolved() const;
  int _getWater() const;
  bool _isRoad() const;
  
  class Impl;
  ScopedPtr< Impl > _d;
};

class Reservoir : public WaterSource
{
public:
  Reservoir();
  ~Reservoir();

  virtual void build(PlayerCityPtr city, const TilePos& pos );
  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles) const;
  virtual bool isNeedRoadAccess() const;
  virtual void initTerrain(Tile& terrain);
  virtual void timeStep(const unsigned long time);
  virtual void destroy();
  virtual std::string troubleDesc() const;

private:
  bool _isWaterSource;
  bool _isNearWater( PlayerCityPtr city, const TilePos& pos ) const;
};

#endif // __CAESARIA_WATER_BUILDGINDS_INCLUDED__
