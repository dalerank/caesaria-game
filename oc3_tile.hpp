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

#ifndef __OPENCAESAR3_TILE_H_INCLUDED__
#define __OPENCAESAR3_TILE_H_INCLUDED__

#include "oc3_terraininfo.hpp"
#include "oc3_positioni.hpp"

class Picture;

// a Tile in the Tilemap
class Tile
{
public:
  Tile(const TilePos& pos);
  Tile(const Tile& clone);

  // tile coordinates
  int getI() const;
  int getJ() const;
  TilePos getIJ() const;
  Point getScreenPos() const;

  // displayed picture
  void setPicture(Picture *picture);
  Picture& getPicture() const;

  // used for multi-tile graphics: current displayed picture
  // background of constructible tiles is 1x1 => master used for foreground
  // non-constructible tiles have no foreground => master used for background
  Tile* getMasterTile() const;
  void setMasterTile(Tile* master);
  bool isMasterTile() const;;

  const TerrainTile& getTerrain() const;
  TerrainTile& getTerrain();
  bool isFlat() const;  // returns true if the tile is walkable/boatable (for display purpose)

  void resetWasDrawn() { _wasDrawn = false; }
  void setWasDrawn()   { _wasDrawn = true;  }
  bool wasDrawn()      { return _wasDrawn;  }

private:
  TilePos _pos; // coordinates of the tile
  Tile* _master_tile;  // left-most tile if multi-tile, or "this" if single-tile
  TerrainTile _terrain;    // infos about the tile (building, tree, road, water, rock...)

  Picture* _picture; // displayed picture  
  bool _wasDrawn;
};

#endif //__OPENCAESAR3_TILE_H_INCLUDED__