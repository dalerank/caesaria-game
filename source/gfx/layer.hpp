// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_LAYER_H_INCLUDED__
#define __OPENCAESAR3_LAYER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "engine.hpp"
#include "tile.hpp"
#include "renderer.hpp"
#include "core/predefinitions.hpp"
#include <set>

class Layer : public ReferenceCounted
{
public:
  typedef std::set<int> VisibleWalkers;

  virtual int getType() const = 0;
  virtual VisibleWalkers getVisibleWalkers() const = 0;
  virtual void drawTile( GfxEngine& engine, Tile& tile, Point offset ) = 0;
  virtual void drawTilePass(GfxEngine& engine, Tile& tile, Point offset, Renderer::Pass pass );
  virtual void drawArea( GfxEngine& engine, const TilemapArea& area, Point offset,
                             std::string resourceGroup, int tileId );

  virtual void drawColumn(GfxEngine& engine, const Point& pos, const int startPicId, const int percent );
};

typedef SmartPtr<Layer> LayerPtr;

#endif //__OPENCAESAR3_LAYER_H_INCLUDED__
