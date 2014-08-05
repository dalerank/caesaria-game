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

#ifndef __CAESARIA_ROAD_H_INCLUDE__
#define __CAESARIA_ROAD_H_INCLUDE__

#include "objects/construction.hpp"

class Road : public Construction
{
public:
  Road();

  virtual const gfx::Picture& picture(PlayerCityPtr city, TilePos pos, const gfx::TilesArray& tmp ) const;
  void updatePicture();

  virtual void build( PlayerCityPtr city, const TilePos& pos );
  virtual void initTerrain( gfx::Tile &terrain);
  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const gfx::TilesArray& aroundTiles) const;
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
  virtual void burn();
  virtual void appendPaved( int value );
  virtual void computeAccessRoads();
  int pavedValue() const;
  virtual gfx::Renderer::PassQueue passQueue() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

private:
  int _paved;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const gfx::TilesArray& aroundTiles) const;
  virtual void appendPaved(int value);
  virtual void build(PlayerCityPtr city, const TilePos &pos);
  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  virtual const gfx::Picture& picture() const;
  void updatePicture();
};


#endif //__CAESARIA_ROAD_H_INCLUDE__
