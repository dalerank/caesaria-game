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


#include "military.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "walker/romesoldier.hpp"
#include "core/logger.hpp"
#include "events/event.hpp"
#include "walker/patrolpoint.hpp"
#include "barracks.hpp"

using namespace constants;

namespace {
Renderer::Pass _fpq[] = { Renderer::building, Renderer::animations };
static Renderer::PassQueue fortPassQueue( _fpq, _fpq + 2 );
}

class Fort::Impl
{
public:
  FortAreaPtr area;
  unsigned int maxSoldier;
  PatrolPointPtr patrolPoint;
};

FortLegionnaire::FortLegionnaire() : Fort( building::fortLegionaire, 16 )
{
  setPicture( ResourceGroup::security, 12 );
}

void FortLegionnaire::build(PlayerCityPtr city, const TilePos& pos)
{
  Fort::build( city, pos );

  _setPatrolPoint( PatrolPoint::create( city, this,
                                        ResourceGroup::sprites, 21, 8,
                                        pos + TilePos( 3, 3 ) ) );

  city::Helper helper( city );
  BarracksList barracks = helper.find<Barracks>( building::barracks );

  if( barracks.empty() )
  {
    _setError( "##need_barracks_for_work##" );
  }
}

void FortLegionnaire::_readyNewSoldier()
{
  RomeSoldierPtr soldier = RomeSoldier::create( _city(), walker::legionary );

  city::Helper helper( _city() );
  TilesArray tiles = helper.getAroundTiles( this );

  foreach( tile, tiles)
  {
    if( (*tile)->isWalkable( true ) )
    {
      soldier->send2city( this, (*tile)->pos() );
      addWalker( soldier.object() );
      return;
    }
  }
}

FortMounted::FortMounted() : Fort( constants::building::fortMounted, 15 )
{
  setPicture( ResourceGroup::security, 12 );
}

FortJaveline::FortJaveline() : Fort( building::fortJavelin, 14 )
{
  setPicture( ResourceGroup::security, 12 );
}

class FortArea::Impl
{
public:
  TilePos basePos;
};

FortArea::FortArea() : Building( building::fortArea, Size(4) ),
  _d( new Impl )
{
  setPicture( ResourceGroup::security, 13 );

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

FortArea::~FortArea() {}

bool FortArea::isFlat() const {  return true; }
bool FortArea::isWalkable() const{  return true;}

void FortArea::destroy()
{
  Building::destroy();

  FortPtr fort = ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) );
  if( fort.isValid() )
  {
    events::GameEventPtr e = events::ClearLandEvent::create( _d->basePos );
    e->dispatch();
  }
}

void FortArea::setBase(FortPtr base)
{
  if( base.isValid() )
  {
    _d->basePos = base->pos();
  }
}

Fort::Fort(building::Type type, int picIdLogo) : WorkingBuilding( type, Size(3) ),
  _d( new Impl )
{
  Picture logo = Picture::load(ResourceGroup::security, picIdLogo );
  logo.setOffset( Point( 80, 10 ) );

  Picture area = Picture::load(ResourceGroup::security, 13 );
  area.setOffset(Tile( TilePos(3,0)).mapPos() + Point(0,-30));

  _fgPicturesRef().resize(2);
  _fgPicturesRef()[ 0 ] = logo;
  _fgPicturesRef()[ 1 ] = area;

  _d->area = new FortArea();
  _d->area->drop();

  _d->maxSoldier = 16;

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

float Fort::evaluateTrainee(walker::Type traineeType)
{
  int currentForce = getWalkers().size() * 100;
  int traineeForce = WorkingBuilding::evaluateTrainee( traineeType );
  int maxForce = _d->maxSoldier * 100;

  return ( maxForce - currentForce - traineeForce ) / 16;
}

Fort::~Fort() {}

void Fort::timeStep( const unsigned long time )
{
  if( time % 15 == 1 )
  {
    if( numberWorkers() <= 0 )
    {
      if( _animationRef().isRunning() )
      {
        _animationRef().stop();
        return;
      }
    }
    else
    {
      if( _animationRef().isStopped() )
      {
        _animationRef().start();
      }
    }

    int traineeLevel = getTraineeValue( walker::soldier );
    // all trainees are there for the show!
    if( traineeLevel / 100 >= 1 )
    {
      if( getWalkers().size() < _d->maxSoldier )
      {
        _readyNewSoldier();
        setTraineeValue( walker::soldier, math::clamp<int>( traineeLevel - 100, 0, _d->maxSoldier * 100 ) );
      }
    }
  }

  WorkingBuilding::timeStep( time );
}

void Fort::destroy()
{
  WorkingBuilding::destroy();

  if( _d->area.isValid()  )
  {
    events::GameEventPtr e = events::ClearLandEvent::create( _d->area->pos() );
    e->dispatch();
    _d->area = 0;
  }

  if( _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->deleteLater();
    _d->patrolPoint = 0;
  }
}

TilePos Fort::getFreeSlot() const
{
  TilePos patrolPos;
  if( _d->patrolPoint.isNull()  )
  {
    Logger::warning( "Not patrol point assign in fort [%d,%d]", pos().i(), pos().j() );
    patrolPos = _d->area->pos() + TilePos( 0, 3 );
  }
  else
  {
    patrolPos = _d->patrolPoint->pos();
  }


  city::Helper helper( _city() );
  TilesArray tiles = helper.getArea( patrolPos - TilePos( 0, 3), patrolPos );

  for( int range=1; range < 5; range++ )
  {
    TilePos offset( range, range );
    TilesArray tmpTiles = _city()->getTilemap().getRectangle( patrolPos - offset, patrolPos + offset );
    tiles.insert( tiles.end(), tmpTiles.begin(), tmpTiles.end() );
  }

  for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); )
  {
    WalkerList wlist = _city()->getWalkers( walker::any, (*it)->pos() );
    if( !wlist.empty() ) { it = tiles.erase( it ); }
    else { it++; }
  }

  if( !tiles.empty() )
  {
    int step = rand() % std::min<int>( tiles.size(), _d->maxSoldier );
    TilesArray::iterator it = tiles.begin();
    std::advance( it, step );
    return (*it)->pos();
  }

  return TilePos( -1, -1 );
}

void Fort::changePatrolArea()
{
  WalkerList walkers = getWalkers();

  foreach( it, walkers )
  {
    RomeSoldierPtr soldier = ptr_cast<RomeSoldier>( *it );
    if( soldier.isValid() )
    {
      soldier->send2patrol();
    }
  }
}

void Fort::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  stream[ "patrolPoint" ] = _d->patrolPoint->pos();
  stream[ "soldierNumber"] = _d->maxSoldier;
}

void Fort::load(const VariantMap& stream)
{
  WorkingBuilding::load( stream );

  TilePos patrolPos = stream.get( "patrolPoint" );
  if(  _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->setPos( patrolPos );
  }

  _d->maxSoldier = stream.get( "soldierNumber", 16 ).toUInt();
}

void Fort::_setPatrolPoint(PatrolPointPtr patrolPoint)
{
  _d->patrolPoint = patrolPoint;
}

bool Fort::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool isFreeFort = Building::canBuild( city, pos, aroundTiles );
  bool isFreeArea = _d->area->canBuild( city, pos + TilePos( 3, 0 ), aroundTiles );

  return (isFreeFort && isFreeArea);
}

void Fort::build(PlayerCityPtr city, const TilePos& pos)
{
  Building::build( city, pos );

  _d->area->build( city, pos + TilePos( 3, 0 ) );
  _d->area->setBase( this );

  city->addOverlay( _d->area.object() );

  _fgPicturesRef().resize(1);
}

bool Fort::isNeedRoadAccess() const {  return false; }
