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

#include "pathway_helper.hpp"
#include "astarpathfinding.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"

Pathway PathwayHelper::create( TilePos startPos, TilePos stopPos,
                               WayType type/*=roadOnly */ )
{
  switch( type )
  {
  case allTerrain:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, stopPos, ret, Pathfinder::terrainOnly );

    return ret;
  }
  break;

  case roadOnly:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, stopPos, ret, Pathfinder::roadOnly );

    return ret;
  }

  case roadFirst:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, stopPos, ret, Pathfinder::roadOnly );
    if( !ret.isValid() )
    {
      Pathfinder::getInstance().getPath( startPos, stopPos, ret, Pathfinder::roadOnly );
    }

    return ret;
  }
  break;

  default:
  break;
  }

  return Pathway();
}

Pathway PathwayHelper::create( TilePos startPos,
                               ConstructionPtr construction, PathwayHelper::WayType type)
{
  switch( type )
  {
  case allTerrain:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, construction->getEnterArea(), ret, Pathfinder::terrainOnly );

    return ret;
  }
  break;

  case roadOnly:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, construction->getEnterArea(), ret, Pathfinder::roadOnly );

    return ret;
  }

  case roadFirst:
  {
    Pathway ret;
    Pathfinder::getInstance().getPath( startPos, construction->getEnterArea(), ret, Pathfinder::roadOnly );

    if( !ret.isValid() )
    {
      Pathfinder::getInstance().getPath( startPos, construction->getEnterArea(), ret, Pathfinder::terrainOnly );
    }

    return ret;
  }
  break;

  default:
  break;
  }

  return Pathway();

}

Pathway PathwayHelper::create(TilePos startPos, TilePos stopPos, const TilePossibleCondition& condition)
{
  Pathfinder::getInstance().setCondition( condition );
  Pathway ret;

  Pathfinder::getInstance().getPath( startPos, stopPos, ret, Pathfinder::customCondition );
  return ret;
}

Pathway PathwayHelper::randomWay(PlayerCityPtr city, TilePos startPos, int walkRadius)
{
  int loopCounter = 0; //loop limiter
  do
  {
    const Tilemap& tmap = city->getTilemap();

    TilePos destPos( std::rand() % walkRadius - walkRadius / 2, std::rand() % walkRadius - walkRadius / 2 );
    destPos = (startPos+destPos).fit( TilePos( 0, 0 ), TilePos( tmap.getSize()-1, tmap.getSize()-1 ) );

    if( tmap.at( destPos ).isWalkable( true) )
    {
      Pathway pathway = create( startPos, destPos, PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        return pathway;
      }
    }
  }
  while( ++loopCounter < 20 );

  return Pathway();
}
