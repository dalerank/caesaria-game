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

#ifndef __CAESARIA_BUILD_OPTIONS_H_INCLUDED__
#define __CAESARIA_BUILD_OPTIONS_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "game/enums.hpp"
#include "core/variant.hpp"
#include "gfx/tileoverlay.hpp"

class CityBuildOptions : public ReferenceCounted
{
public:
  CityBuildOptions();
  ~CityBuildOptions();

  void setBuildingAvailble( const TileOverlay::Type type, bool mayBuild );
  void setGroupAvailable(const BuildMenuType type, Variant mayBuild );
  bool isGroupAvailable(const BuildMenuType type ) const;

  bool isBuildingAvailble( const TileOverlay::Type type ) const;

  void clear();

  void load( const VariantMap& options );
  VariantMap save() const;

  CityBuildOptions& operator=(const CityBuildOptions& a);

  void setBuildingAvailble(const TileOverlay::Type start, const TileOverlay::Type stop, bool mayBuild);
  bool isBuildingsAvailble(const TileOverlay::Type start, const TileOverlay::Type stop) const;
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_BUILD_OPTIONS_H_INCLUDED__
