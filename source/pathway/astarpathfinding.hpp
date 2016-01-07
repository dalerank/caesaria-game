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
  SET_STATICSINGLETON_FRIEND_FOR(Pathfinder)
public:
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
