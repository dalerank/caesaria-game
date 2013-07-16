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

#include "oc3_cityservice_shoreline.hpp"
#include "oc3_city.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_positioni.hpp"
#include "oc3_tile.hpp"
#include "oc3_time.hpp"

class CityServiceShoreline::Impl
{
public:
  PtrTilesList slTiles;
  int lastTimeUpdate;
  CityPtr city;

  void checkMap( CityPtr city );
};

void CityServiceShoreline::Impl::checkMap( CityPtr city )
{
  int mapSize = city->getTilemap().getSize();
  PtrTilesList tiles = city->getTilemap().getFilledRectangle( TilePos( 0, 0), Size( mapSize ) );

  for( PtrTilesList::iterator it=tiles.begin(); it != tiles.end(); it++ )
  {
    int imgId = (*it)->getTerrain().getOriginalImgId();
    if( (imgId >= 372 && imgId <= 403) || (imgId>=414 && imgId<=418) )
    {
      slTiles.push_back( *it );
    }
  }
}

CityServicePtr CityServiceShoreline::create( CityPtr city )
{
  CityServicePtr ret( new CityServiceShoreline( city ) );
  ret->drop();

  return ret;
}

CityServiceShoreline::CityServiceShoreline( CityPtr city )
  : CityService( "shoreline" ), _d( new Impl )
{
  _d->city = city;
  _d->lastTimeUpdate = 0;  
}

void CityServiceShoreline::update( const unsigned int time )
{
  if( (time - _d->lastTimeUpdate) < 50 )
    return;

  _d->lastTimeUpdate = time;

  if( _d->slTiles.empty() )
  {
    _d->checkMap( _d->city );
  }

  for( PtrTilesList::iterator it=_d->slTiles.begin(); it != _d->slTiles.end(); it++ )
  {
    TerrainTile& info = (*it)->getTerrain();

    if( info.getOverlay().isValid() )
      continue;

    int picId = info.getOriginalImgId();
    if( info.getDesirability() > 10 )
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

    std::string picName = TerrainTileHelper::convId2PicName( picId ); 
    if( picName != ((*it)->getPicture().getName()+".png") )
    {
      (*it)->setPicture( &Picture::load( picName ) );
    }

  }
}