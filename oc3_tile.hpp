#ifndef __OPENCAESAR3_TILE_H_INCLUDED__
#define __OPENCAESAR3_TILE_H_INCLUDED__

#include "oc3_terraininfo.hpp"
#include "oc3_positioni.hpp"

#include <list>
#include <vector>

class Picture;

// a Tile in the Tilemap
class Tile
{
public:
  Tile(const int i, const int j);
  Tile(const Tile& clone);

  // tile coordinates
  int getI() const;
  int getJ() const;
  TilePos getIJ() const;

  // displayed picture
  void set_picture(Picture *picture);
  Picture& get_picture() const;

  // used for multi-tile graphics: current displayed picture
  // background of constructible tiles is 1x1 => master used for foreground
  // non-constructible tiles have no foreground => master used for background
  Tile* get_master_tile() const;
  void set_master_tile(Tile* master);
  bool is_master_tile() const;;

  const TerrainTile& get_terrain() const;
  TerrainTile& get_terrain();
  bool is_flat() const;  // returns true if the tile is walkable/boatable (for display purpose)

private:
  int _i; // coordinates of the tile
  int _j; // coordinates of the tile
  Tile* _master_tile;  // left-most tile if multi-tile, or "this" if single-tile
  TerrainTile _terrain;    // infos about the tile (building, tree, road, water, rock...)

  Picture *_picture; // displayed picture   
};

typedef std::list< const Tile* > ConstWayOnTiles;
typedef std::list< Tile* > WayOnTiles;
typedef std::list< Tile* > PtrTilesArea;
typedef std::list< Tile* > PtrTilesList;

typedef std::vector< Tile* > PtrTilesVector;
typedef std::vector< Tile  > TilesVector;
typedef std::vector< TilesVector > TileGrid;

#endif //__OPENCAESAR3_TILE_H_INCLUDED__