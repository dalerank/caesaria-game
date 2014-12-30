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
#include "gfx/helper.hpp"
#include "city/city.hpp"
#include "gfx/decorator.hpp"

#include "gfx/picture.hpp"

namespace gfx
{

void WalkerDebugInfo::showPath( WalkerPtr walker, gfx::Engine& engine, gfx::Camera* camera, NColor color )
{
  Point camOffset = camera->offset();
  const Pathway& pathway = walker->pathway();

  const TilesArray& tiles = pathway.allTiles();   

  NColor pathColor = color;

  if( color == 0)
  {
    if( walker->agressive() > 0 )
    {
      pathColor = DefaultColors::red;
    }
    else
    {
      pathColor = pathway.isReverse() ? DefaultColors::blue : DefaultColors::green;
    }
  }

  Point pos = walker->mappos();
  Point xOffset( tilemap::cellSize().width(), 0 );
  if( pathway.isReverse() )
  {
    int rStart = pathway.length() - pathway.curStep();
    for( int step=rStart-1; step >= 0; step-- )
    {
      engine.drawLine(  pathColor, pos + camOffset, tiles[ step ]->mappos() + camOffset + xOffset );
      pos = tiles[ step ]->mappos() + xOffset;
    }
  }
  else
  {
    for( unsigned int step=pathway.curStep()+1; step < tiles.size(); step++ )
    {
      Tile* tile = tiles[ step ];
      engine.drawLine(  pathColor, pos + camOffset, tile->mappos() + camOffset + xOffset );
      pos = tile->mappos() + xOffset;
    }
  }
}

}//end namespace gfx
