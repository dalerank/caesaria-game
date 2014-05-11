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

using namespace constants;
using namespace gfx;

typedef enum { prettyFountain=2, fontainEmpty = 3, fontainFull = 4, simpleFountain = 10, fontainSizeAnim = 7,
               awesomeFountain=18, patricianFountain=26 } FontainConstant;

Fountain::Fountain() : ServiceBuilding(Service::fountain, building::fountain, Size(1))
{  
  //std::srand( DateTime::getElapsedTime() );

  //setPicture( ResourceGroup::utilitya, 10 );

  _haveReservoirWater = false;
  _waterIncreaseInterval = GameDate::days2ticks( 7 );
  _lastPicId = simpleFountain;
  _fgPicturesRef().resize(1);
  _initAnimation();

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setWorkers( 1 );
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
    if( tile().getWaterService( WTR_RESERVOIR ) > 0 /*&& getWorkersCount() > 0*/ )
    {
      _haveReservoirWater = true;
    }
    else
    {
      //remove fontain service from tiles
      _haveReservoirWater = false;
    }

    if( !mayWork() )
    {
      return;
    }

    TilePos offset( 4, 4 );
    Tilemap& tmap = _city()->tilemap();
    TilesArray reachedTiles = tmap.getArea( pos() - offset, pos() + offset );

    foreach( tile, reachedTiles ) { (*tile)->fillWaterService( WTR_FONTAIN ); }
  }

  if( GameDate::isWeekChanged() )
  {
    int desPic[] = { simpleFountain, prettyFountain, awesomeFountain, patricianFountain };
    int currentId = desPic[ math::clamp<int>( tile().getDesirability() / 25, 0, 3 ) ];
    if( currentId != _lastPicId )
    {
      _lastPicId = currentId;
      setPicture( ResourceGroup::utilitya, currentId );
      _initAnimation();
    }
  }

  ServiceBuilding::timeStep( time );
}

bool Fountain::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool ret = Construction::canBuild( city, pos, aroundTiles );

  Tilemap& tmap = city->tilemap();
  const Tile& tile = tmap.at( pos );
  int picid = (tile.getWaterService( WTR_RESERVOIR ) > 0 ? fontainFull : fontainEmpty );
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

bool Fountain::isNeedRoadAccess() const {  return false; }

bool Fountain::haveReservoirAccess() const
{
  TilePos offset( 10, 10 );
  TilesArray reachedTiles = _city()->tilemap().getArea( pos() - offset, pos() + offset );
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
  TilesArray reachedTiles = tmap.getArea( pos() - TilePos( 10, 10 ), Size( 10 + 10 ) + size() );

  foreach( tile, reachedTiles ) { (*tile)->decreaseWaterService( WTR_FONTAIN, 20 ); }
}

bool Fountain::mayWork() const {  return ServiceBuilding::isActive() && _haveReservoirWater; }

void Fountain::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  _lastPicId = stream.get( "lastPicId", simpleFountain );
  setPicture( ResourceGroup::utilitya, _lastPicId );
  _initAnimation();
  //check animation
  timeStep( 1 );
}

void Fountain::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  stream[ "lastPicId" ] = _lastPicId;
  stream[ "haveWater" ] = _haveReservoirWater;
}

void Fountain::_initAnimation()
{
  _animationRef().clear();
  _animationRef().load( ResourceGroup::utilitya, _lastPicId+1, fontainSizeAnim );
  _animationRef().setDelay( 2 );
  _fgPicturesRef()[0] = Picture::getInvalid();
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
