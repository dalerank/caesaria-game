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
}

void FortLegionnaire::_readyNewSoldier()
{
  RomeSoldierPtr soldier = RomeSoldier::create( _getCity(), walker::legionary );

  CityHelper helper( _getCity() );
  TilesArray tiles = helper.getAroundTiles( this );

  foreach( Tile* tile, tiles)
  {
    if( tile->isWalkable( true ) )
    {
      soldier->send2city( this, tile->getIJ() );
      addWalker( soldier.as<Walker>() );
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
  _fireIncrement = _damageIncrement = 0;
}

FortArea::~FortArea()
{
}

bool FortArea::isFlat() const
{
  return true;
}

bool FortArea::isWalkable() const
{
  return true;
}

void FortArea::destroy()
{
  Building::destroy();

  FortPtr fort = _getCity()->getOverlay( _d->basePos ).as< Fort >();
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
    _d->basePos = base->getTilePos();
  }
}

Fort::Fort(building::Type type, int picIdLogo) : WorkingBuilding( type, Size(3) ),
  _d( new Impl )
{
  Picture logo = Picture::load(ResourceGroup::security, picIdLogo );
  logo.setOffset(80,10);

  Picture area = Picture::load(ResourceGroup::security, 13 );
  area.setOffset(Tile( TilePos(3,0)).getXY() + Point(0,-30));

  _fgPicturesRef().resize(2);
  _fgPicturesRef().at( 0 ) = logo;
  _fgPicturesRef().at( 1 ) = area;

  _d->area = new FortArea();
  _d->area->drop();

  _d->maxSoldier = 16;
  _fireIncrement = _damageIncrement = 0;
}

float Fort::evaluateTrainee(walker::Type traineeType)
{
  int currentForce = getWalkers().size() * 100;
  int traineeForce = WorkingBuilding::evaluateTrainee( traineeType );
  int maxForce = _d->maxSoldier * 100;

  return ( maxForce - currentForce - traineeForce ) / 16;
}

Fort::~Fort()
{

}

void Fort::timeStep( const unsigned long time )
{
  if( time % 15 == 1 )
  {
    if( getWorkersCount() <= 0 )
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

    int traineeLevel = _traineeMap[ walker::soldier ] / 100;
    // all trainees are there for the show!
    if( traineeLevel >= 1 )
    {
      if( getWalkers().size() < _d->maxSoldier )
      {
        _readyNewSoldier();
        _traineeMap[ walker::soldier ] = math::clamp<int>( _traineeMap[ walker::soldier ] - 100, 0, _d->maxSoldier * 100 );
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
    events::GameEventPtr e = events::ClearLandEvent::create( _d->area->getTilePos() );
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
    Logger::warning( "Not patrol point assign in fort [%d,%d]", getTilePos().getI(), getTilePos().getJ() );
    patrolPos = _d->area->getTilePos() + TilePos( 0, 3 );
  }
  else
  {
    patrolPos = _d->patrolPoint->getIJ();
  }


  CityHelper helper( _getCity() );
  TilesArray tiles = helper.getArea( patrolPos - TilePos( 0, 3), patrolPos );

  for( int range=1; range < 5; range++ )
  {
    TilePos offset( range, range );
    TilesArray tmpTiles = _getCity()->getTilemap().getRectangle( patrolPos - offset, patrolPos + offset );
    tiles.insert( tiles.end(), tmpTiles.begin(), tmpTiles.end() );
  }

  for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); )
  {
    WalkerList wlist = _getCity()->getWalkers( walker::any, (*it)->getIJ() );
    if( !wlist.empty() ) { it = tiles.erase( it ); }
    else { it++; }
  }

  if( !tiles.empty() )
  {
    int step = rand() % std::min( tiles.size(), _d->maxSoldier );
    TilesArray::iterator it = tiles.begin();
    std::advance( it, step );
    return (*it)->getIJ();
  }

  return TilePos( -1, -1 );
}

void Fort::changePatrolArea()
{
  WalkerList walkers = getWalkers();

  foreach( WalkerPtr w, walkers )
  {
    RomeSoldierPtr soldier = w.as<RomeSoldier>();
    if( soldier.isValid() )
    {
      soldier->send2patrol();
    }
  }
}

void Fort::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  stream[ "patrolPoint" ] = _d->patrolPoint->getIJ();
  stream[ "soldierNumber"] = _d->maxSoldier;
}

void Fort::load(const VariantMap& stream)
{
  WorkingBuilding::load( stream );

  TilePos patrolPos = stream.get( "patrolPoint" );
  if(  _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->setIJ( patrolPos );
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

  city->addOverlay( _d->area.as<TileOverlay>() );

  _fgPicturesRef().resize(1);
}

bool Fort::isNeedRoadAccess() const
{
  return false;
}

class PatrolPoint::Impl
{
public:
  Animation animation;
  Picture standart;
  TilePos basePos;
};

PatrolPointPtr PatrolPoint::create( PlayerCityPtr city, FortPtr base,
                                    std::string prefix, int startPos, int stepNumber, TilePos position)
{
  PatrolPoint* pp = new PatrolPoint( city );
  pp->_d->standart = Picture::load( ResourceGroup::sprites, 58 );
  pp->_d->basePos = base->getTilePos();

  Point extOffset( 15, 0 );
  Animation anim;
  anim.load( prefix, startPos, stepNumber );
  anim.setOffset( anim.getOffset() + Point( 0, 52 )  + extOffset );
  pp->_d->standart.addOffset( extOffset.getX(), extOffset.getY() );

  pp->_d->animation = anim;
  pp->setIJ( position );
  PatrolPointPtr ptr( pp );
  ptr->drop();

  city->addWalker( ptr.as<Walker>() );
  return ptr;
}

void PatrolPoint::getPictureList(PicturesArray& oPics)
{
  oPics.push_back( _d->standart );
  oPics.push_back( _d->animation.getFrame() );
}

PatrolPoint::PatrolPoint( PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::patrolPoint );
}

void PatrolPoint::timeStep(const unsigned long time)
{
  _d->animation.update( time );
}

void PatrolPoint::acceptPosition()
{
  FortPtr fort = _getCity()->getOverlay( _d->basePos ).as< Fort >();
  if( fort.isValid() )
  {
    fort->changePatrolArea();
  }
}
