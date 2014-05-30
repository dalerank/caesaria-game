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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

const TilePos Helper::invalidPos = TilePos( -1, -1 );

void Helper::updateDesirability( TileOverlayPtr overlay, bool onBuild )
{
  Tilemap& tilemap = _city->tilemap();

  const Desirability& dsrbl = overlay->desirability();
  int mul = ( onBuild ? 1 : -1);

  //change desirability in selfarea
  TilesArray area = tilemap.getArea( overlay->pos(), overlay->size() );
  foreach( tile, area )
  {
    (*tile)->appendDesirability( mul * dsrbl.base );
  }

  //change deisirability around
  int current = mul * dsrbl.base;
  for( int curRange=1; curRange <= dsrbl.range; curRange++ )
  {
    TilesArray perimetr = tilemap.getRectangle( overlay->pos() - TilePos( curRange, curRange ),
                                                 overlay->size() + Size( 2 * curRange ) );
    foreach( tile, perimetr )
    {
      (*tile)->appendDesirability( current );
    }

    current += mul * dsrbl.step;
  }
}

TilesArray Helper::getArea(TileOverlayPtr overlay)
{
  return _city->tilemap().getArea( overlay->pos(), overlay->size() );
}

TilesArray Helper::getAroundTiles(TileOverlayPtr overlay)
{
  return _city->tilemap().getArea( overlay->pos()-TilePos(1,1), overlay->size()+Size(2) );
}

TilesArray Helper::getArea(TilePos start, TilePos stop)
{
  return _city->tilemap().getArea( start, stop );
}

float Helper::getBalanceKoeff()
{
  return atan( _city->population() / 5000.f );
}

void Helper::updateTilePics()
{
  TilesArray tiles = _city->tilemap().getArea( TilePos( 0, 0 ), Size( _city->tilemap().size() ) );
  foreach( it, tiles)
  {
    (*it)->setPicture( Picture::load( (*it)->picture().name() ) );
  }
}

}//end namespace city
