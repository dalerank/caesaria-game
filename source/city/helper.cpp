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

#include "city/helper.hpp"
#include "gfx/tilemap.hpp"

using namespace constants;

const TilePos CityHelper::invalidPos = TilePos( -1, -1 );

void CityHelper::updateDesirability( ConstructionPtr construction, bool onBuild )
{
  Tilemap& tilemap = _city->getTilemap();

  const Desirability& dsrbl = construction->getDesirability();
  int mul = ( onBuild ? 1 : -1);

  //change desirability in selfarea
  TilesArray area = tilemap.getArea( construction->getTilePos(), construction->getSize() );
  foreach( Tile* tile, area )
  {
    tile->appendDesirability( mul * dsrbl.base );
  }

  //change deisirability around
  int current = mul * dsrbl.base;
  for( int curRange=1; curRange <= dsrbl.range; curRange++ )
  {
    TilesArray perimetr = tilemap.getRectangle( construction->getTilePos() - TilePos( curRange, curRange ),
                                                 construction->getSize() + Size( 2 * curRange ) );
    foreach( Tile* tile, perimetr )
    {
      tile->appendDesirability( current );
    }

    current += mul * dsrbl.step;
  }
}

TilesArray CityHelper::getArea(TileOverlayPtr overlay)
{
  return _city->getTilemap().getArea( overlay->getTilePos(), overlay->getSize() );
}

TilesArray CityHelper::getAroundTiles(TileOverlayPtr overlay)
{
  return _city->getTilemap().getArea( overlay->getTilePos()-TilePos(1,1), overlay->getSize()+Size(2) );
}

TilesArray CityHelper::getArea(TilePos start, TilePos stop)
{
  return _city->getTilemap().getArea( start, stop );
}

float CityHelper::getBalanceKoeff()
{
  return atan( _city->getPopulation() / 5000.f );
}
