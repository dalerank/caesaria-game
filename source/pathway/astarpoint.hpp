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

using namespace gfx;
namespace {
static const TilePos invalidePos;
}

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

  inline const TilePos& getPos()   {    return tile ? tile->pos() : invalidePos;  }

  AStarPoint( const Tile* t ) : tile( t )
  {    
    parent = NULL;
    closed = false;
    opened = false;

    f = g = h = 0;
  }

  inline AStarPoint* getParent()  {    return parent; }
  inline void setParent(AStarPoint* p)  {    parent = p;  }

  int getGScore(AStarPoint* p, bool checkRoad )
  { 
    int roadScore = 0;
    if( checkRoad )
    {
      roadScore = (p->tile
                    ? (p->tile->getFlag( Tile::tlRoad ) ? 0 : +10)
                    : (+100) );
    }

    const TilePos& pos = getPos();
    const TilePos& otherPos = p->getPos();
    return p->g + ((pos.i() == otherPos.i() || pos.j() == otherPos.j()) ? 10 : 14) + roadScore;
  }

  int getHScore(AStarPoint* p)
  {
    const TilePos& pos = tile ? tile->pos() : invalidePos;
    const TilePos& otherPos = p ? p->tile->pos() : invalidePos;
    return (abs(otherPos.i() - pos.i()) + abs(otherPos.j() - pos.j())) * 10;
  }

  void computeScores(AStarPoint* end, bool useRoad )
  {
    g = getGScore(parent, useRoad );
    h = getHScore(end);
    f = g + h;
  }

  inline int getGScore(){    return g;  }
  inline int getHScore(){    return h;  }
  inline int getFScore(){    return f;  }
  inline bool hasParent(){    return parent != NULL;  }
};

#endif //__CAESARIA_ASTARPOINT_H_INCLUDED__
