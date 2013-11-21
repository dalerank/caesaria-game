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

#ifndef __CAESARIA_LANDOVERLAYFACTORY_H_INCLUDE_
#define __CAESARIA_LANDOVERLAYFACTORY_H_INCLUDE_

#include "enums.hpp"
#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "gfx/tileoverlay.hpp"

class TileOverlayConstructor
{
public:
  virtual TileOverlayPtr create() = 0;
};

class TileOverlayFactory
{
public:
    static TileOverlayFactory& getInstance();
    TileOverlayPtr create( const TileOverlay::Type type ) const;
    TileOverlayPtr create( const std::string& typeName ) const;

    bool canCreate( const TileOverlay::Type type ) const;

    void addCreator( const TileOverlay::Type type, const std::string& typeName, TileOverlayConstructor* ctor );
private:
    TileOverlayFactory();

    class Impl;
    ScopedPtr< Impl > _d;
};

#endif  //__CAESARIA_LANDOVERLAYFACTORY_H_INCLUDE_
