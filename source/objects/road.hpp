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

  virtual const gfx::Picture& picture( const city::AreaInfo& areaInfo ) const;
  void updatePicture();

  virtual bool build(const city::AreaInfo &info);
  virtual void initTerrain( gfx::Tile &terrain);
  virtual bool canBuild(const city::AreaInfo& areaInfo) const;
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isNeedRoad() const;
  virtual void destroy();
  virtual void burn();
  virtual void appendPaved( int value );
  virtual Variant getProperty(const std::string &name) const;
  virtual void computeRoadside();
  virtual void changeDirection( gfx::Tile* masterTile, Direction direction);
  int pavedValue() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

private:
  void _changeIndexIfAqueduct( const city::AreaInfo& areaInfo, int& m, int index) const;
  int _paved;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual bool canBuild(const city::AreaInfo& areaInfo) const;
  virtual std::string errorDesc() const;
  virtual const gfx::Picture& picture(const city::AreaInfo& areaInfo) const;
  virtual void appendPaved(int value);
  virtual bool build(const city::AreaInfo &info);
  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  virtual const gfx::Picture& picture() const;
  void updatePicture();
};


#endif //__CAESARIA_ROAD_H_INCLUDE__
