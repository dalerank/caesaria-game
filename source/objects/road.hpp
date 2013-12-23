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

#ifndef __CAESARIA_ROAD_H_INCLUDE__
#define __CAESARIA_ROAD_H_INCLUDE__

#include "objects/construction.hpp"

class Road : public Construction
{
public:
  Road();

  virtual Picture computePicture();
  void updatePicture();

  virtual void build( PlayerCityPtr city, const TilePos& pos );
  virtual void initTerrain(Tile &terrain);
  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles) const;
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
  virtual void appendPaved( int value );
  int getPavedValue() const;
  virtual Renderer::PassQueue getPassQueue() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

private:
  int _paved;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual void initTerrain(Tile& terrain);
  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles) const;
  virtual Picture computePicture();
  virtual void appendPaved(int value);
};


#endif //__CAESARIA_ROAD_H_INCLUDE__
