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

#ifndef __CAESARIA_ASTARPATHFINDING_H_INCLUDED__
#define __CAESARIA_ASTARPATHFINDING_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "gfx/tilesarray.hpp"
#include "core/singleton.hpp"
#include "core/delegate.hpp"
#include "pathway.hpp"

class Pathfinder : public StaticSingleton<Pathfinder>
{
  friend class StaticSingleton;
public:
  typedef enum { noFlags=0x0,
                 checkStart=0x1, checkStop=0x2,
                 roadOnly=0x4, deepWaterOnly=0x8, terrainOnly=0x10,
                 waterOnly=0x20, traversePath=0x40,
                 everyWhere=0x80, fourDirection=0x100,
                 customCondition=0x200, ignoreRoad=0x400 } Flags;

  void update( const gfx::Tilemap& tmap );

  Pathway getPath( TilePos start, gfx::TilesArray arrivedArea, int flags );

  Pathway getPath( TilePos start, TilePos stop, int flags );

  Pathway getPath(const gfx::Tile& start, const gfx::Tile& stop, int flags);

  void setCondition( const TilePossibleCondition& condition );
  void resetCondition();
  
  unsigned int maxLoopCount() const;
  void setMaxLoopCount( unsigned int count );

  void setVerboseMode( int level );

  ~Pathfinder();
private:
  Pathfinder();

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__CAESARIA_ASTARPATHFINDING_H_INCLUDED__
