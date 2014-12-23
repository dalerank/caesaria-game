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

#ifndef __CAESARIA_TOWER_H_INCLUDED__
#define __CAESARIA_TOWER_H_INCLUDED__

#include "service.hpp"
#include "pathway/pathway.hpp"

class Tower : public ServiceBuilding
{
public:
  Tower();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual bool canBuild(const CityAreaInfo& areaInfo) const;
  virtual std::string workersProblemDesc() const;
  virtual void deliverService();

  virtual gfx::TilesArray enterArea() const;

  void resetPatroling();

  virtual Point offset(const gfx::Tile& tile, const Point& subpos ) const;
  PathwayList getWays(TilePos start, FortificationList dest );
  Pathway getWay(TilePos start, TilePos stop);
  virtual float evaluateTrainee(constants::walker::Type traineeType);

protected:
  void _rebuildWays();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_TOWER_H_INCLUDED__
