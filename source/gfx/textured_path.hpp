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

#ifndef __CAESARIA_TEXTURED_PATH_H_INCLUDED__
#define __CAESARIA_TEXTURED_PATH_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "core/color.hpp"
#include "render_info.hpp"
#include "gfx/tilesarray.hpp"
#include "pathway/pathway.hpp"

namespace gfx { class Camera; class Engine; }

namespace gfx
{

class TexturedPath
{
public:
  static void draw(const Pathway& way, const RenderInfo& rinfo, NColor color=NColor(), const Point& doff=Point());
  static void draw(const TilesArray& tiles, const RenderInfo& rinfo, NColor color=NColor(), const Point& doff=Point());
};

struct ColoredWay
{
  TilesArray tiles;
  NColor color;
  Point offset;
};

}//end namespace gfx

#endif //__CAESARIA_TEXTURED_PATH_H_INCLUDED__
