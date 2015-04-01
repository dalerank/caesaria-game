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

#ifndef __CAESARIA_LOW_BRIDGE_H_INCLUDED__
#define __CAESARIA_LOW_BRIDGE_H_INCLUDED__

#include "objects/construction.hpp"
#include "core/scopedptr.hpp"
#include "core/direction.hpp"

class LowBridge;

class LowBridgeSubTile : public Construction
{
  friend class LowBridge;
public:
  enum { liftingWest=67, spanWest=68, descentWest=69, liftingNorth=70, spanNorth=71, descentNorth=72 };
  LowBridgeSubTile( const TilePos& pos, int index );

  virtual ~LowBridgeSubTile();

  std::string errorDesc() const;
  bool isWalkable() const;
  bool isNeedRoad() const;

  bool build( const city::AreaInfo& info );

  virtual void setState( Param name, double value);

  void hide();

  void initTerrain( gfx::Tile& terrain );

  bool canDestroy() const;

  void destroy();

  void save(VariantMap &stream) const;

  Point offset( const gfx::Tile& , const Point& subpos ) const;

private:
  TilePos _pos;
  int _index;
  int _info;
  int _imgId;
  gfx::Picture _picture;
  LowBridge* _parent;
};

class LowBridge : public Construction
{
public:
  LowBridge();

  virtual bool canBuild(const city::AreaInfo& areaInfo) const;
  virtual void initTerrain( gfx::Tile& terrain );
  virtual bool build( const city::AreaInfo& info );
  virtual bool canDestroy() const;
  virtual void destroy();
  virtual std::string errorDesc() const;
  virtual bool isNeedRoad() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  void hide();

private:
  void _computePictures( PlayerCityPtr city, const TilePos& startPos, const TilePos& endPos, Direction dir );
  void _checkParams( PlayerCityPtr city, Direction& direction, TilePos& start, TilePos& stop, const TilePos& curPos ) const;

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_LOW_BRIDGE_H_INCLUDED__
