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

#ifndef _CAESARIA_DOCK_INCLUDE_H_
#define _CAESARIA_DOCK_INCLUDE_H_

#include "working.hpp"
#include "core/direction.hpp"

class Dock : public WorkingBuilding
{
public:
  Dock();
  ~Dock();

  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles) const;  // returns true if it can be built there
  virtual void build(PlayerCityPtr city, const TilePos &pos);
  virtual void destroy();

  virtual void timeStep(const unsigned long time);

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  bool isBusy() const;

  const Tile& getLandingTile() const;

  void requestGoods( GoodStock& stock );

  void sellGoods( GoodStock& stock );
  void buyGoods( GoodStock& stock, int qty );
  void storeGoods(GoodStock& stock, const int amount);
private:
  void _setDirection( constants::Direction direction );
  virtual void _updatePicture( constants::Direction direction );
  void _tryReceiveGoods();
  void _tryDeliverGoods();

private:
  Dock( PlayerCityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;

};

#endif //_CAESARIA_DOCK_INCLUDE_H_
