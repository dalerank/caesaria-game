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

#include "oc3_positioni.hpp"
#include "oc3_animation.hpp"
#include "oc3_enums.hpp"
#include "oc3_predefinitions.hpp"

class Picture;

// a Tile in the Tilemap
class Tile
{
  struct Terrain
  {
    bool water;
    bool rock;
    bool tree;
    bool building;
    bool road;
    bool garden;
    bool aqueduct;
    bool meadow;
    bool elevation;
    bool wall;
    bool gatehouse;
    int  desirability;
    int  watersrvc;

    /*
     * original tile information
     */
    unsigned short int imgid;
    unsigned short int terraininfo;

    void reset();
    void clearFlags();
  };

public:
  typedef enum { tlRoad=0, tlWater, tlTree, tlMeadow, tlRock, tlBuilding, tlAqueduct,
                 tlGarden, tlElevation, tlWall, tlGateHouse,
                 isConstructible, isDestructible, clearAll,
                 wasDrawn } Type;

  Tile(const TilePos& pos);

  // tile coordinates
  int getI() const;
  int getJ() const;
  TilePos getIJ() const;
  Point getXY() const;

  // displayed picture
  void setPicture( const Picture* picture );
  void setPicture( const char* rc, const int index );
  void setPicture( const std::string& name );
  const Picture& getPicture() const;

  // used for multi-tile graphics: current displayed picture
  // background of constructible tiles is 1x1 => master used for foreground
  // non-constructible tiles have no foreground => master used for background
  Tile* getMasterTile() const;
  void setMasterTile(Tile* master);
  bool isMasterTile() const;

  //const TerrainTile& getTerrain() const;
  //TerrainTile& getTerrain();
  bool isFlat() const;  // returns true if the tile is walkable/boatable (for display purpose)

  void resetWasDrawn() { _wasDrawn = false; }
  void setWasDrawn()   { _wasDrawn = true;  }

  void animate( unsigned int time );

  const Animation& getAnimation() const;
  void setAnimation( const Animation& animation );

  bool isWalkable( bool ) const;
  bool getFlag( Type type ) const;
  void setFlag( Type type, bool value );

  void appendDesirability( int value );
  int getDesirability() const;
  LandOverlayPtr getOverlay() const;
  void setOverlay( LandOverlayPtr overlay );
  unsigned int getOriginalImgId() const;
  void setOriginalImgId( unsigned short int id );

  void fillWaterService( const WaterService type );
  void decreaseWaterService( const WaterService type );
  int getWaterService( const WaterService type ) const;

private:
  TilePos _pos; // coordinates of the tile
  Tile* _master;  // left-most tile if multi-tile, or "this" if single-tile
  Terrain _terrain;    // infos about the tile (building, tree, road, water, rock...)
  Picture const* _picture; // displayed picture
  bool _wasDrawn;
  Animation _animation;
  LandOverlayPtr _overlay;
};

class TileHelper
{
public:
  static std::string convId2PicName( const unsigned int imgId );
  static int convPicName2Id( const std::string &pic_name);
  static int encode( const Tile& tt );

  static void decode( Tile& tile, const int bitset);

};

#endif //__OPENCAESAR3_TILE_H_INCLUDED__
