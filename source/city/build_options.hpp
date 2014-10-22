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

#ifndef __CAESARIA_BUILD_OPTIONS_H_INCLUDED__
#define __CAESARIA_BUILD_OPTIONS_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "game/enums.hpp"
#include "core/variant.hpp"
#include "gfx/tileoverlay.hpp"

namespace city
{

class BuildOptions : public ReferenceCounted
{
public:
  BuildOptions();
  virtual ~BuildOptions();

  void setBuildingAvailble( const gfx::TileOverlay::Type type, bool mayBuild );
  void setGroupAvailable(const BuildMenuType type, Variant mayBuild );
  bool isGroupAvailable(const BuildMenuType type ) const;
  unsigned int getBuildingsQuote( const gfx::TileOverlay::Type type ) const;

  bool isBuildingAvailble( const gfx::TileOverlay::Type type ) const;

  void clear();

  void load( const VariantMap& options );
  VariantMap save() const;

  BuildOptions& operator=(const BuildOptions& a);

  void setBuildingAvailble(const gfx::TileOverlay::Type start, const gfx::TileOverlay::Type stop, bool mayBuild);
  bool isBuildingsAvailble(const gfx::TileOverlay::Type start, const gfx::TileOverlay::Type stop) const;
  bool isCheckDesirability() const;
  int getMaximumForts() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace city
#endif //__CAESARIA_BUILD_OPTIONS_H_INCLUDED__
