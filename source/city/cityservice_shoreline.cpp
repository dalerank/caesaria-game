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

#include "cityservice_shoreline.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "gfx/helper.hpp"
#include "gfx/tilemap.hpp"
#include "core/time.hpp"
#include "core/foreach.hpp"
#include "objects/overlay.hpp"
#include "walker/watergarbage.hpp"
#include "game/gamedate.hpp"
#include "walker/river_wave.hpp"
#include "cityservice_factory.hpp"

using namespace gfx;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Shoreline, shoreline)

class Shoreline::Impl
{
public:
  TilesArray slTiles;
  TilesArray dwTiles;

  int lastTimeUpdate;
  unsigned int nextWaterGarbage;

  void checkMap(PlayerCityPtr city );
};

void Shoreline::Impl::checkMap( PlayerCityPtr city )
{
  const TilesArray& tiles = city->tilemap().allTiles();

  foreach( it, tiles )
  {
    int imgId = (*it)->originalImgId();
    if( (imgId >= 372 && imgId <= 403) || (imgId>=414 && imgId<=418) || (*it)->getFlag( Tile::tlCoast ) )
    {
      slTiles.push_back( *it );
    }

    if( (*it)->getFlag( Tile::tlDeepWater ) )
    {
      dwTiles.push_back( *it );
    }
  }
}

SrvcPtr Shoreline::create( PlayerCityPtr city )
{
  city::SrvcPtr ret( new Shoreline( city ) );
  ret->drop();

  return ret;
}

std::string Shoreline::defaultName(){ return CAESARIA_STR_EXT(Shoreline); }

Shoreline::Shoreline( PlayerCityPtr city )
  : city::Srvc( city, Shoreline::defaultName() ), _d( new Impl )
{
  _d->lastTimeUpdate = 0;  
  _d->nextWaterGarbage = 0;
}

void Shoreline::timeStep( const unsigned int time )
{
  if( !game::Date::isWeekChanged() )
    return;

  if( _d->slTiles.empty() )
  {
    _d->checkMap( _city() );
  }

  if( time > _d->nextWaterGarbage )
  {
    WaterGarbagePtr wg = WaterGarbage::create( _city() );
    wg->send2City( _city()->borderInfo().boatEntry );

    _d->nextWaterGarbage = time + math::random( game::Date::days2ticks( 10 ) );

    for( int k=0; k < 20; k++ )
    {
      Tile* t = _d->dwTiles.random();
      RiverWavePtr rw = RiverWave::create( _city() );
      rw->send2City( t->pos() );
    }
  }

  _d->lastTimeUpdate = time;

  foreach( it, _d->slTiles )
  {
    Tile* tile = *it;
    if( tile->overlay().isValid() )
      continue;

    int picId = tile->originalImgId();
    if( tile->param( Tile::pDesirability ) > 10 )
    {
      switch( picId )
      {
      case 372: case 373: case 374: case 375: picId = 57 + 778; break;
      case 376: case 377: case 378: case 379: picId = 58 + 778; break;
      case 380: case 381: case 382: case 383: picId = 56 + 778; break;
      case 384: case 385: case 386: case 387: picId = 55 + 778; break;
      case 388: case 389: case 390: case 391: picId = 65 + 778; break;
      case 392: case 393: case 394: case 395: picId = 63 + 778; break;
      case 396: case 397: case 398: case 399: picId = 66 + 778; break;
      case 400: case 401: case 402: case 403: picId = 64 + 778; break;
      case 414: picId = 59 + 778; break;
      case 415: picId = 61 + 778; break;
      case 416: picId = 60 + 778; break;
      case 417: picId = 62 + 778; break;
      }
    }

    std::string picName = imgid::toResource( picId );
    if( picName != tile->picture().name())
    {
      tile->setPicture( picName );
    }
  }
}

}//end namespace city
