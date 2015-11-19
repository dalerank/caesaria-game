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

namespace gfx
{

static Tilemap invalidTmap;

namespace tilemap
{

static int x_tileBase = caCellWidth;
static int y_tileBase = x_tileBase / 2;
static Size tilePicSize( x_tileBase * 2 - 2, x_tileBase );
static Size tileCellSize( x_tileBase, y_tileBase );
static Point centerOffset( y_tileBase / 2, y_tileBase / 2 );
static TilePos tileInvalidLocation( -1, -1 );
static TilePos tilePosLocation( 1, 1 );

void initTileBase(int width)
{
  x_tileBase = width;
  y_tileBase = x_tileBase / 2;
  tilePicSize = Size( x_tileBase * 2 - 2, x_tileBase );
  tileCellSize = Size( x_tileBase, y_tileBase );
  centerOffset = Point( y_tileBase / 2, y_tileBase / 2 );
}

const Point& cellCenter() { return centerOffset;}
const Size& cellPicSize() { return tilePicSize; }
const Size& cellSize() { return tileCellSize; }

Direction getDirection(const TilePos& b, const TilePos& e)
{
  float t = (e - b).getAngleICW();
  int angle = (int)ceil( t / 45.f);

  Direction directions[] = { east, southEast, south, southWest,
                             west, northWest, north, northEast, northEast };

  return directions[ angle ];
}

const TilePos& invalidLocation() { return tileInvalidLocation; }
bool isValidLocation(const TilePos &pos) { return pos.i() >= 0 && pos.j() >=0; }
const TilePos& unitLocation(){ return tilePosLocation; }
Tilemap& getInvalid() { return invalidTmap; }

unsigned int picWidth2CellSize(int width)
{
  return width > 0
            ? (width+2) / tilemap::tilePicSize.width()
            : 0;
}

}//end namespace tilemap

}//end namespace gfx
