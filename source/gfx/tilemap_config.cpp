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

#include "tilemap_config.hpp"
#include "imgid.hpp"
#include "core/logger.hpp"
#include "game/resourcegroup.hpp"
#include "objects/overlay.hpp"
#include "tilemap.hpp"

using namespace direction;

namespace config
{

static gfx::Tilemap invalidTmap;

static int x_tileBase = tilemap.cell.width.neww;
static int y_tileBase = x_tileBase / 2;
static Size tilePicSize( x_tileBase * 2 - 2, x_tileBase );
static Size tileCellSize( x_tileBase, y_tileBase );
static Point centerOffset( y_tileBase / 2, y_tileBase / 2 );
static TilePos tilePosLocation( 1, 1 );

void config::_Tilemap::_Cell::setWidth(int width)
{
  x_tileBase = width;
  y_tileBase = x_tileBase / 2;
  tilePicSize = Size( x_tileBase * 2 - 2, x_tileBase );
  tileCellSize = Size( x_tileBase, y_tileBase );
  centerOffset = Point( y_tileBase / 2, y_tileBase / 2 );
}

const Point& _Tilemap::_Cell::center() { return centerOffset;}
const Size& _Tilemap::_Cell::picSize() { return tilePicSize; }
const Size& _Tilemap::_Cell::size() { return tileCellSize; }
bool _Tilemap::isValidLocation(const TilePos &pos) { return pos.i() >= 0 && pos.j() >=0; }
const TilePos& _Tilemap::unitLocation(){ return tilePosLocation; }
gfx::Tilemap& _Tilemap::invalid() { return invalidTmap; }

unsigned int _Tilemap::picWidth2CellSize(int width)
{
  return width > 0
            ? (width+2) / tilePicSize.width()
            : 0;
}

}//end namespace config
