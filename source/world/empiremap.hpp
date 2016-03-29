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

#ifndef __CAESARIA_EMPIREMAP_H_INCLUDED__
#define __CAESARIA_EMPIREMAP_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "core/predefinitions.hpp"
#include "movableobject.hpp"

namespace world
{

class EmpireMap
{
public:
  typedef enum { trUnknown=0, trSea=0x1, trLand=0x2, trCity=trSea|trLand, trRock=0x4, trAny=0x7 } TerrainType;

  EmpireMap();
  ~EmpireMap();

  void load( const VariantMap& stream );
  VariantMap save() const;
  void setCity( const Point& pos );

  Size size() const;

  TerrainType getTerrainType(const TilePos& ij) const;
  TilePos point2location( const Point& point ) const;
  void setTerrainType(const TilePos& ij, TerrainType type);
  Rect area( const TilePos& ij ) const;

  Route findRoute( const Point& start, const Point& stop, int flags ) const;
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //__CAESARIA_EMPIREMAP_H_INCLUDED__
