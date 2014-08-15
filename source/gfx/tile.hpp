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

#ifndef __CAESARIA_TILE_H_INCLUDED__
#define __CAESARIA_TILE_H_INCLUDED__

#include "core/position.hpp"
#include "gfx/animation.hpp"
#include "game/enums.hpp"
#include "tileoverlay.hpp"
#include "predefinitions.hpp"
#include "core/direction.hpp"

namespace gfx
{

class Picture;

// a Tile in the Tilemap
class Tile
{
public:
  typedef enum { pWellWater=0, pFountainWater, pReservoirWater, pDesirability, pBasicCount } Param;
  typedef enum { tlRoad=0, tlWater, tlTree, tlMeadow, tlRock, tlBuilding,
                 tlGarden, tlElevation, tlWall, tlDeepWater, tlRubble,
                 isConstructible, isDestructible, tlRift, tlCoast, tlGrass, clearAll,
                 wasDrawn } Type;

public:
  explicit Tile(const TilePos& pos);

  // tile coordinates
  int i() const;
  int j() const;
  const TilePos& pos() const;

  inline const TilePos& epos() const { return _epos; }
  void setEPos( const TilePos& epos );

  Point center() const;
  Point mappos() const;

  // displayed picture
  void setPicture( const Picture& picture );
  void setPicture( const char* rc, const int index );
  void setPicture( const std::string& name );
  const Picture& picture() const;

  // used for multi-tile graphics: current displayed picture
  // background of constructible tiles is 1x1 => master used for foreground
  // non-constructible tiles have no foreground => master used for background
  Tile* masterTile() const;
  void setMasterTile(Tile* master);
  bool isMasterTile() const;

  void changeDirection(constants::Direction newDirection);

  bool isFlat() const;  // returns true if the tile is walkable/boatable (for display purpose)

  inline void resetWasDrawn() { _wasDrawn = false; }
  inline void setWasDrawn()   { _wasDrawn = true;  }

  void animate( unsigned int time );

  const gfx::Animation& animation() const;
  void setAnimation( const gfx::Animation& animation );

  bool isWalkable( bool ) const;
  bool getFlag( Type type ) const;
  void setFlag( Type type, bool value );

  TileOverlayPtr overlay() const;
  void setOverlay( TileOverlayPtr overlay );
  unsigned int originalImgId() const;
  void setOriginalImgId( unsigned short int id );

  inline int height() const { return _height; }
  void setHeight( int value ) { _height = value; }

  void setParam( Param param, int value );
  void changeParam( Param param, int value );
  int param( Param param ) const;

private:
  struct Terrain
  {
    bool water;
    bool rock;
    bool tree;
    bool road;
    bool garden;
    bool meadow;
    bool elevation;
    bool rubble;
    bool wall;
    bool coast;
    bool deepWater;

    /*
     * original tile information
     */
    unsigned short int imgid;
    unsigned short int terraininfo;

    void reset();
    void clearFlags();

    std::map<Param, int> params;
  };

  TilePos _pos; // absolute coordinates
  TilePos _epos; // effective coordinates
  Tile* _master;  // left-most tile if multi-tile, or "this" if single-tile
  Terrain _terrain; // infos about the tile (building, tree, road, water, rock...)
  Picture _picture; // main picture
  bool _wasDrawn;
  int _height;
  gfx::Animation _animation;
  TileOverlayPtr _overlay;
};

class TileHelper
{
public:
  static std::string convId2PicName( const unsigned int imgId );
  static int convPicName2Id( const std::string &pic_name);
  static int encode( const Tile& tt );
  static unsigned int hash( const TilePos& pos );
  static Point tilepos2screen( const TilePos& pos );
  static void decode( Tile& tile, const int bitset);
  static Tile& getInvalid();
};

}//end namespace gfx

#endif //__CAESARIA_TILE_H_INCLUDED__
