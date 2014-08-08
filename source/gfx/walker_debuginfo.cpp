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

#include "walker_debuginfo.hpp"
#include "pathway/pathway.hpp"
#include "walker/walker.hpp"
#include "city/city.hpp"
#include "gfx/decorator.hpp"

#include "gfx/picture.hpp"

namespace gfx
{

void WalkerDebugInfo::showPath( WalkerPtr walker, gfx::Engine& engine, gfx::Camera* camera )
{
  Point camOffset = camera->offset();
  const Pathway& pathway = walker->pathway();

  const TilesArray& tiles = pathway.allTiles();

  Point pos = walker->mappos();
  if( pathway.isReverse() )
  {
    int rStart = pathway.length() - pathway.curStep();
    for( int step=rStart-1; step >= 0; step-- )
    {
      engine.drawLine(  0xff0000ff, pos + camOffset, tiles[ step ]->mappos() + camOffset + Point( 30, 0 ) );
      pos = tiles[ step ]->mappos() + Point( 30, 0 );
    }
  }
  else
  {
    for( int step=pathway.curStep()+1; step < tiles.size(); step++ )
    {
      engine.drawLine(  0xff00ff00, pos + camOffset, tiles[ step ]->mappos() + camOffset + Point( 30, 0 ) );
      pos = tiles[ step ]->mappos() + Point( 30, 0 );
    }
  }
}

}//end namespace gfx
