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

#include "objects/construction.hpp"
#include "pathway_helper.hpp"
#include "astarpathfinding.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "core/logger.hpp"

using namespace gfx;

Pathway PathwayHelper::create( TilePos startPos, TilePos stopPos, WayType type/*=roadOnly */ )
{
  Pathfinder& p = Pathfinder::instance();
  switch( type )
  {
  case allTerrain: return p.getPath( startPos, stopPos, Pathfinder::terrainOnly );
  case roadOnly: return p.getPath( startPos, stopPos, Pathfinder::roadOnly );

  case roadFirst:
  {
    Pathway ret = p.getPath( startPos, stopPos, Pathfinder::roadOnly );
    if( !ret.isValid() )
    {
      ret = p.getPath( startPos, stopPos, Pathfinder::terrainOnly );
    }

    return ret;
  }
  break;

  case deepWater: return p.getPath( startPos, stopPos, Pathfinder::deepWaterOnly );
  case water: return p.getPath( startPos, stopPos, Pathfinder::waterOnly );

  case deepWaterFirst:
  {
    Pathway ret = p.getPath( startPos, stopPos, Pathfinder::deepWaterOnly );
    if( !ret.isValid() )
    {
      ret = p.getPath( startPos, stopPos, Pathfinder::waterOnly );
    }

    return ret;
  }
  break;

  default:
  break;
  }

  return Pathway();
}

Pathway PathwayHelper::create( TilePos startPos, ConstructionPtr construction, PathwayHelper::WayType type)
{
  Pathway way;
  if( construction.isValid() )
  {
    Pathfinder& p = Pathfinder::instance();

    switch( type )
    {
    case allTerrain: way = p.getPath( startPos, construction->enterArea(), Pathfinder::terrainOnly );
    case roadOnly: way = p.getPath( startPos, construction->roadside(), Pathfinder::roadOnly );

    case roadFirst:
    {
      way = p.getPath( startPos, construction->roadside(), Pathfinder::roadOnly );

      if( !way.isValid() )
      {
        way = p.getPath( startPos, construction->enterArea(), Pathfinder::terrainOnly );
      }
    }
    break;

    default:
    break;
    }

    if( !way.isValid() )
    {
      Logger::warning( "PathwayHelper: can't find way from [%d,%d] to construction: name=%s pos=[%d,%d]",
                       startPos.i(), startPos.j(), construction->name().c_str(),
                       construction->pos().i(), construction->pos().j() );
    }
  }  

  return way;
}

Pathway PathwayHelper::create(TilePos startPos, TilePos stopPos, const TilePossibleCondition& condition)
{
  Pathfinder::instance().setCondition( condition );
  return Pathfinder::instance().getPath( startPos, stopPos, Pathfinder::customCondition );
}

DirectRoute PathwayHelper::shortWay(const TilePos& startPos, ConstructionList buildings, WayType type)
{
  DirectRoute route;
  for( auto it : buildings )
  {
    Pathway path = create( startPos, it, type );
    if( path.isValid() )
    {
      if( !route.isValid() )
      {
        route.set( it, path );
      }
      else
      {
        if( route.length() > path.length() )
        {
          route.set( it, path );
        }
      }
    }
  }

  return route;
}

DirectRoute PathwayHelper::shortWay(PlayerCityPtr city, const TilePos& startPos, object::Type buildingType, WayType type)
{  
  ConstructionList constructions = city->statistic().objects.find<Construction>( buildingType );
  return shortWay( startPos, constructions, type );
}

DirectRoute PathwayHelper::shortWay(PlayerCityPtr city, const TilePosArray& area, object::Type buildingType, WayType type)
{
  TilePosArray locations( area );
  ConstructionList constructions = city->statistic().objects.find<Construction>( buildingType );

  DirectRoute shortestWay;
  while( !locations.empty() )
  {
    DirectRoute route = shortWay( locations.front(), constructions, type );
    if( !shortestWay.isValid() )
    {
      shortestWay = route;
    }
    else if( shortestWay.length() > route.length() )
    {
      shortestWay = route;
    }

    locations.pop_front();
  }

  return shortestWay;
}

Pathway PathwayHelper::randomWay(PlayerCityPtr city, const TilePos& startPos, int walkRadius)
{
  TilePos offset( walkRadius / 2, walkRadius / 2 );
  TilesArray tiles = city->tilemap().getArea( startPos - offset, startPos + offset );
  tiles = tiles.walkables( true );

  int loopCounter = 0; //loop limiter
  if( !tiles.empty() )
  {
    do
    {
      Tile* destPos = tiles.random();
      Pathway pathway = create( startPos, destPos->pos(), PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        return pathway;
      }
    }
    while( ++loopCounter < 20 );
  }

  return Pathway();
}

Pathway PathwayHelper::way2border(PlayerCityPtr city, const TilePos& pos)
{
  Tilemap& tmap = city->tilemap();

  TilePos lastStart, lastStop;
  for( int k=0; k < 80; k++ )
  {
    TilePos start, stop;
    start = tmap.fit( pos - TilePos( k, k ) );
    stop = tmap.fit( pos + TilePos( k, k ) );

    if( start == lastStart && stop == lastStop )
      break;

    TilesArray border = tmap.getRectangle( start, stop );
    border = border.walkables( true );
    foreach( it, border )
    {
      Tile* tile = *it;
      Pathway pw = create( pos, tile->pos(), PathwayHelper::allTerrain );

      if( pw.isValid() )
        return pw;
    }
  }

  return Pathway();
}
