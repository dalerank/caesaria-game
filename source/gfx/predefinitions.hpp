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

#ifndef __CAESARIA_GFX_PREDEFINITIONS_H_INCLUDED__
#define __CAESARIA_GFX_PREDEFINITIONS_H_INCLUDED__

#include "core/predefinitions.hpp"

namespace gfx
{

class Tile;
class Picture;
class Tilemap;
class TilemapCamera;
class TilesArray;
class Renderer;

PREDEFINE_CLASS_SMARTLIST(TileOverlay,List)

namespace layer
{
PREDEFINE_CLASS_SMARTLIST(Layer,List)
}

}

#endif //__CAESARIA_GFX_PREDEFINITIONS_H_INCLUDED__
