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

#include "fountain.hpp"

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "objects/road.hpp"
#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "walker/workerhunter.hpp"
#include "events/returnworkers.hpp"

using namespace constants;
using namespace gfx;

namespace {
CAESARIA_LITERALCONST(lastPicId)
CAESARIA_LITERALCONST(haveWater)
static const unsigned int fillDistance = 4;
}

typedef enum { prettyFountain=2, fontainEmpty = 3, fontainFull = 4, simpleFountain = 10, fontainSizeAnim = 7,
               awesomeFountain=18, patricianFountain=26 } FontainConstant;

Fountain::Fountain()
  : ServiceBuilding(Service::fountain, building::fountain, Size(1))
{  
  _haveReservoirWater = false;
  _waterIncreaseInterval = GameDate::days2ticks( 7 );
  _lastPicId = simpleFountain;
  _fgPicturesRef().resize(1);
  _initAnimation();

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

void Fountain::deliverService()
{
  if( !_haveReservoirWater )
    return;

  ServiceWalkerPtr walker = ServiceWalker::create( _city(), serviceType() );
  walker->setBase( BuildingPtr( this ) );
  walker->setReachDistance( 4 );
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( tile().pos() );

  foreach( b, reachedBuildings ) { (*b)->applyService( walker ); }
}

void Fountain::timeStep(const unsigned long time)
{
  //filled area, that fontain present and work
  if( time % _waterIncreaseInterval == 1 )
  {
    _haveReservoirWater = tile().param( Tile::pReservoirWater ) > 0;

    if( mayWork() )
    {
      Tilemap& tmap = _city()->tilemap();
      TilesArray reachedTiles = tmap.getArea( fillDistance, pos() );

      foreach( tile, reachedTiles )
      {
        int value = (*tile)->param( Tile::pFountainWater );
        (*tile)->setParam( Tile::pFountainWater, math::clamp( value+1, 0, 20 ) );
      }
    }
  }

  if( GameDate::isWeekChanged() )
  {
    int desPic[] = { simpleFountain, prettyFountain, awesomeFountain, patricianFountain };
    int currentId = desPic[ math::clamp<int>( tile().param( Tile::pDesirability ) / 25, 0, 3 ) ];
    if( currentId != _lastPicId )
    {
      _lastPicId = currentId;
      setPicture( ResourceGroup::utilitya, currentId );
      _initAnimation();
    }

    if( needWorkers() > 0 )
    {
      RecruterPtr recruter = Recruter::create( _city() );
      recruter->once( this, needWorkers(), fillDistance * 2);
    }
  }  

  ServiceBuilding::timeStep( time );
}

bool Fountain::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool ret = Construction::canBuild( city, pos, aroundTiles );

  Tilemap& tmap = city->tilemap();
  const Tile& tile = tmap.at( pos );
  int picid = (tile.param( Tile::pReservoirWater ) > 0 ? fontainFull : fontainEmpty );
  const_cast< Fountain* >( this )->setPicture( ResourceGroup::waterbuildings, picid );

  return ret;
}

void Fountain::build(PlayerCityPtr city, const TilePos& pos )
{
  ServiceBuilding::build( city, pos );

  setPicture( ResourceGroup::waterbuildings, fontainEmpty );
  _lastPicId = simpleFountain;
  _initAnimation();
}

bool Fountain::isNeedRoadAccess() const { return false; }

bool Fountain::haveReservoirAccess() const
{
  TilesArray reachedTiles = _city()->tilemap().getArea( 10, pos() );
  foreach( tile, reachedTiles )
  {
    TileOverlayPtr overlay = (*tile)->overlay();
    if( overlay.isValid() && (building::reservoir == overlay->type()) )
    {
      return true;
    }
  }

  return false;
}

void Fountain::destroy()
{
  ServiceBuilding::destroy();

  Tilemap& tmap = _city()->tilemap();
  TilesArray reachedTiles = tmap.getArea( fillDistance, pos() );

  foreach( tile, reachedTiles ) { (*tile)->setParam( Tile::pFountainWater, 0 ); }

  if( numberWorkers() > 0 )
  {
    events::GameEventPtr e = events::ReturnWorkers::create( pos(), numberWorkers() );
    e->dispatch();
  }
}

bool Fountain::mayWork() const {  return ServiceBuilding::mayWork() && ServiceBuilding::isActive() && _haveReservoirWater; }

unsigned int Fountain::fillRange() const { return fillDistance; }

void Fountain::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  _lastPicId = stream.get( lc_lastPicId, simpleFountain );
  _haveReservoirWater = stream.get( lc_haveWater );
  setPicture( ResourceGroup::utilitya, _lastPicId );
  _initAnimation();
  //check animation
  timeStep( 1 );
}

void Fountain::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  stream[ lc_lastPicId ] = _lastPicId;
  stream[ lc_haveWater ] = _haveReservoirWater;
}

void Fountain::_initAnimation()
{
  _animationRef().clear();
  _animationRef().load( ResourceGroup::utilitya, _lastPicId+1, fontainSizeAnim );
  _animationRef().setDelay( 2 );
  _fgPicture( 0 ) = Picture::getInvalid();
  _animationRef().stop();

  switch ( _lastPicId )
  {
  case simpleFountain: _animationRef().setOffset( Point( 12, 24 ) ); break;
  case prettyFountain: _animationRef().setOffset( Point( 9, 41 ) ); break;
  case awesomeFountain: _animationRef().setOffset( Point( 12, 24 ) ); break;
  case patricianFountain: _animationRef().setOffset( Point( 14, 26 ) ); break;
  default: break;
  }
}
