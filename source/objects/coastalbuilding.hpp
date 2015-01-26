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

#ifndef _CAESARIA_COASTALBUILDING_INCLUDE_H_
#define _CAESARIA_COASTALBUILDING_INCLUDE_H_

#include "factory.hpp"
#include "walker/ship.hpp"
#include "core/direction.hpp"

class CoastalFactory : public Factory
{
public:
  CoastalFactory( const good::Product consume, const good::Product produce,
                  const TileOverlay::Type type, Size size );
  virtual bool canBuild(const CityAreaInfo& areaInfo) const;  // returns true if it can be built there
  virtual bool build(const CityAreaInfo &info);
  virtual void destroy();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual void assignBoat( ShipPtr ship );

  const gfx::Tile& landingTile() const;

  virtual ~CoastalFactory();
private:
  void _setDirection( constants::Direction direction );
  virtual void _updatePicture( constants::Direction direction ) = 0;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_COASTALBUILDING_INCLUDE_H_
