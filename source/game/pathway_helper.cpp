// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "pathway_helper.hpp"
#include "astarpathfinding.hpp"
#include "city.hpp"
#include "tilemap.hpp"

PathWay PathwayHelper::create( CityPtr city, TilePos startPos, TilePos stopPos,
                               WayType type/*=roadOnly */ )
{
  switch( type )
  {
  case allTerrain:
  {
    const Tilemap& tmap = city->getTilemap();
    PathWay ret;
    Pathfinder::getInstance().getPath( tmap.at( startPos ), tmap.at( stopPos ), ret, 0, Size(1) );

    return ret;
  }
  break;

  default:
  break;
  }

  return PathWay();
}

PathWay PathwayHelper::randomWay(CityPtr city, TilePos startPos, int walkRadius)
{
  int loopCounter = 0; //loop limiter
  do
  {
    const Tilemap& tmap = city->getTilemap();

    TilePos destPos( std::rand() % walkRadius - walkRadius / 2, std::rand() % walkRadius - walkRadius / 2 );
    destPos = (startPos+destPos).fit( TilePos( 0, 0 ), TilePos( tmap.getSize()-1, tmap.getSize()-1 ) );

    if( tmap.at( destPos ).isWalkable( true) )
    {
      PathWay pathway = create( city, startPos, destPos, PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        return pathway;
      }
    }
  }
  while( ++loopCounter < 20 );

  return PathWay();
}
