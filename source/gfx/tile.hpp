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
#include "objects/predefinitions.hpp"
#include "predefinitions.hpp"
#include "core/direction.hpp"

namespace gfx
{

// a Tile in the Tilemap
class Tile
{
public:
  typedef enum { pWellWater=0, pFountainWater, pReservoirWater, pDesirability, pDirt, pBasicCount } Param;
  typedef enum { tlRoad=0, tlWater, tlTree, tlMeadow, tlRock, tlOverlay,
                 tlGarden, tlElevation, tlWall, tlDeepWater, tlRubble,
                 isConstructible, isDestructible, tlRift, tlCoast, tlGrass, clearAll,
                 isRendered, tlUnknown } Type;

public:
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
    ImgID imgid;
    unsigned short int terraininfo;

    void clear();
  };

  explicit Tile(const TilePos& pos);

  // tile coordinates
  inline int i() const { return _pos.i(); }
  inline int j() const { return _pos.j(); }
  inline const TilePos& pos() const{ return _pos; }
  inline const TilePos& epos() const { return _epos; }
  inline const Point& mappos() const { return _mappos; }

  inline const OverlayPtr& rov() const { return _overlay; }
  void setEPos( const TilePos& epos );

  Point center() const;

  // displayed picture
  void setPicture( const Picture& picture );
  void setPicture( const char* rc, const int index );
  void setPicture( const std::string& name );
  inline const Picture& picture() const { return _picture; }

  // used for multi-tile graphics: current displayed picture
  // background of constructible tiles is 1x1 => master used for foreground
  // non-constructible tiles have no foreground => master used for background
  inline Tile* master() const { return _master;}
  void setMaster(Tile* master);
  bool isMaster() const;

  void changeDirection( Tile* master, Direction newDirection);

  bool isFlat() const;  // returns true if the tile is walkable/boatable (for display purpose)

  inline void resetRendered()  { _rendered = false; }
  inline void setRendered()    { _rendered = true;  }
  inline bool rendered() const { return _rendered; }

  void animate( unsigned int time );

  const gfx::Animation& animation() const;
  void setAnimation( const gfx::Animation& animation );

  bool isWalkable( bool ) const;
  bool getFlag( Type type ) const;
  void setFlag( Type type, bool value );

  Terrain& terrain() { return _terrain; }
  const Terrain& terrain() const { return _terrain; }

  void setOverlay( OverlayPtr overlay );
  inline ImgID imgId() const { return _terrain.imgid;}
  void setImgId( ImgID id );

  inline int height() const { return _height; }
  void setHeight( int value ) { _height = value; }

  void setParam( Param param, int value );
  void changeParam( Param param, int value );
  int param( Param param ) const;

  template<class T>
  SmartPtr<T> overlay() const { return ptr_cast<T>( _overlay ); }
  OverlayPtr overlay() const;

private:
  std::map<Param, int> _params;
  TilePos _pos; // absolute coordinates
  TilePos _epos; // effective coordinates
  Point _mappos;
  Tile* _master;  // left-most tile if multi-tile, or "this" if single-tile
  Terrain _terrain; // infos about the tile (building, tree, road, water, rock...)
  Picture _picture; // main picture
  bool _rendered;
  int _height;
  Animation _animation;
  OverlayPtr _overlay;

private:
  Tile( const Tile& base );
};

}//end namespace gfx

#endif //__CAESARIA_TILE_H_INCLUDED__
