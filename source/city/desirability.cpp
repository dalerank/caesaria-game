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
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#include "desirability.hpp"
#include "objects/overlay.hpp"
#include "city.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tilearea.hpp"

using namespace gfx;

void Desirability::update( PlayerCityPtr r, OverlayPtr overlay, bool onBuild )
{
  if( r.isNull() )
    return;

  Tilemap& tilemap = r->tilemap();

  const Desirability& dsrbl = overlay->desirability();
  int mul = ( onBuild ? 1 : -1);

  //change desirability in selfarea
  TilesArea area( tilemap, overlay->pos(), overlay->size() );
  for( auto tile : area )
  {
    tile->changeParam( Tile::pDesirability, mul * dsrbl.base );
  }

  //change deisirability around
  int current = mul * dsrbl.base;
  for( int curRange=1; curRange <= dsrbl.range; curRange++ )
  {
    TilesArray perimetr = tilemap.rect( overlay->pos() - TilePos( curRange, curRange ),
                                                overlay->size() + Size( 2 * curRange ) );
    for( auto tile : perimetr )
    {
      tile->changeParam( Tile::pDesirability, current );
    }

    current += mul * dsrbl.step;
  }
}

const Desirability&Desirability::invalid()
{
  static Desirability inv;
  return inv;
}

