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
  case allTerrain: return p.getPath( startPos, stopPos, Pathway::terrainOnly );
  case roadOnly: return p.getPath( startPos, stopPos, Pathway::roadOnly );

  case roadFirst:
  {
    Pathway ret = p.getPath( startPos, stopPos, Pathway::roadOnly );
    if( !ret.isValid() )
    {
      ret = p.getPath( startPos, stopPos, Pathway::terrainOnly );
    }

    return ret;
  }
  break;

  case deepWater: return p.getPath( startPos, stopPos, Pathway::deepWaterOnly );
  case water: return p.getPath( startPos, stopPos, Pathway::waterOnly );

  case deepWaterFirst:
  {
    Pathway ret = p.getPath( startPos, stopPos, Pathway::deepWaterOnly );
    if( !ret.isValid() )
    {
      ret = p.getPath( startPos, stopPos, Pathway::waterOnly );
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
    case allTerrain: way = p.getPath( startPos, construction->enterArea(), Pathway::terrainOnly );
    case roadOnly: way = p.getPath( startPos, construction->roadside(), Pathway::roadOnly );

    case roadFirst:
    {
      way = p.getPath( startPos, construction->roadside(), Pathway::roadOnly );

      if( !way.isValid() )
      {
        way = p.getPath( startPos, construction->enterArea(), Pathway::terrainOnly );
      }
    }
    break;

    default:
    break;
    }

    if( !way.isValid() )
    {
      Logger::warning( "PathwayHelper: can't find way from [{0},{1}] to construction: name={2} pos=[{3},{4}]",
                       startPos.i(), startPos.j(), construction->name().c_str(),
                       construction->pos().i(), construction->pos().j() );
    }
  }

  return way;
}

Pathway PathwayHelper::create(TilePos startPos, TilePos stopPos, const TilePossibleCondition& condition, int flags)
{
  Pathfinder::instance().setCondition( condition );
  return Pathfinder::instance().getPath( startPos, stopPos, Pathway::customCondition|flags );
}

DirectRoute PathwayHelper::shortWay(const TilePos& startPos, ConstructionList buildings, WayType type)
{
  DirectRoute route;
  for (auto it : buildings)
  {
    Pathway path = create(startPos, it, type);
    if (path.isValid())
    {
      if( !route.isValid() )
      {
        route.set(it, path);
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

DirectRoute PathwayHelper::shortWay(PlayerCityPtr city, const Locations& area, object::Type buildingType, WayType type)
{
  Locations locations( area );
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
  TilesArray tiles = city->tilemap().area( startPos - offset, startPos + offset );
  tiles = tiles.walkables( true );
  tiles.remove(startPos);

  int loopCounter = 0; //loop limiter
  if( !tiles.empty() )
  {
    do
    {
      Tile* destPos = tiles.random();
      Pathway pathway = create(startPos, destPos->pos(), PathwayHelper::allTerrain);

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

    TilesArray border = tmap.rect( start, stop );
    border = border.walkables( true );
    for( auto& tile : border )
    {
      Pathway pw = create( pos, tile->pos(), PathwayHelper::allTerrain );

      if( pw.isValid() )
        return pw;
    }
  }

  return Pathway();
}

bool PathwayCondition::append(OverlayPtr overlay)
{
  if( overlay.isNull() )
    return false;

  for( auto tile : overlay->area() )
    insert( tile );

  return true;
}

void PathwayCondition::checkRoads(const Tile* tile, bool& ret)
{
  ret = false;
  if( tile->getFlag( Tile::tlRoad ) )  { ret = true;  }
  else                                 { ret = count( tile ) > 0; }
}

void PathwayCondition::allTiles(const Tile*, bool& ret)
{
  ret = true;
}

TilePossibleCondition PathwayCondition::byRoads() { return makeDelegate( this, &PathwayCondition::checkRoads ); }
TilePossibleCondition PathwayCondition::bySomething() { return makeDelegate( this, &PathwayCondition::allTiles ); }
