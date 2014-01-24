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

#ifndef __CAESARIA_ASTARPOINT_H_INCLUDED__
#define __CAESARIA_ASTARPOINT_H_INCLUDED__

#include "core/position.hpp"
#include "gfx/tile.hpp"

class AStarPoint
{

public:
  AStarPoint* parent;
  bool closed;
  bool opened;
  int f, g, h;
  const Tile* tile;

  AStarPoint()
  {
    parent = NULL;
    closed = false;
    opened = false;
    tile = 0;

    f = g = h = 0;
  }

  TilePos getPos()
  {
    return tile ? tile->pos() : TilePos( 0, 0 );
  }  

  AStarPoint( const Tile* t ) : tile( t )
  {    
    parent = NULL;
    closed = false;
    opened = false;

    f = g = h = 0;
  }

  AStarPoint* getParent()  {    return parent; }
  void setParent(AStarPoint* p)  {    parent = p;  }

  int getGScore(AStarPoint* p, bool useRoad )
  { 
    int offset = (p->tile
                  ? (p->tile->getFlag( Tile::tlRoad ) ? 0 : +50)
                  : (+100) ) * ( useRoad ? 1 : 0 );
    TilePos pos = tile ? tile->pos() : TilePos( 0, 0 );
    TilePos otherPos = p->tile ? p->tile->pos() : getPos();
    return p->g + ((pos.i() == otherPos.i() || pos.j() == otherPos.j()) ? 10 : 14) + offset;
  }

  int getHScore(AStarPoint* p)
  {
    TilePos pos = tile ? tile->pos() : TilePos( 0, 0 );
    TilePos otherPos = p ? p->tile->pos() : TilePos( 0, 0 );
    return (abs(otherPos.i() - pos.i()) + abs(otherPos.j() - pos.j())) * 10;
  }

  void computeScores(AStarPoint* end, bool useRoad )
  {
    g = getGScore(parent, useRoad );
    h = getHScore(end);
    f = g + h;
  }

  int getGScore(){    return g;  }
  int getHScore(){    return h;  }
  int getFScore(){    return f;  }
  bool hasParent(){    return parent != NULL;  }
};

#endif //__CAESARIA_ASTARPOINT_H_INCLUDED__
