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

#ifndef __CAESARIA_HIGH_BRIDGE_H_INCLUDED__
#define __CAESARIA_HIGH_BRIDGE_H_INCLUDED__

#include "objects/construction.hpp"
#include "core/scopedptr.hpp"
#include "core/direction.hpp"

class HighBridge : public Construction
{
public:
  HighBridge();

  virtual bool canBuild(const CityAreaInfo& areaInfo) const;
  virtual void initTerrain( gfx::Tile& terrain );
  virtual bool build(const CityAreaInfo &info);
  virtual bool canDestroy() const;
  virtual void destroy();
  virtual std::string errorDesc() const;
  virtual bool isNeedRoadAccess() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  void hide();

private:
  bool _checkOnlyWaterUnderBridge(PlayerCityPtr city, const TilePos& start, const TilePos& stop) const;
  void _computePictures( PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, constants::Direction dir );
  void _checkParams( PlayerCityPtr city, constants::Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const;

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_HIGH_BRIDGE_H_INCLUDED__
