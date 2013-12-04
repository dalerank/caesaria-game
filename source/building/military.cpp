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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com


#include "military.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "walker/romesoldier.hpp"

using namespace constants;

class Fort::Impl
{
public:
  FortArea* area;
  unsigned int maxSoldier;
};

FortLegionnaire::FortLegionnaire() : Fort( building::fortLegionaire, 16 )
{
  setPicture( ResourceGroup::security, 12 );
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
  Fort* base;
};

FortArea::FortArea( Fort* fort ) : Building( building::fortArea, Size(4) ),
  _d( new Impl )
{
  setPicture( ResourceGroup::security, 13 );
  _d->base = fort;
  _fireIncrement = _damageIncrement = 0;
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
  if( _d->base )
  {
    _d->base->deleteLater();
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

  _d->area = new FortArea( this );
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
  if( _d->area )
  {
    _d->area->deleteLater();
  }
}

TilePos Fort::getFreeSlot() const
{
  CityHelper helper( _getCity() );
  TilesArray tiles = helper.getArea( _d->area );

  foreach(Tile* tile, tiles)
  {
    WalkerList wlist = _getCity()->getWalkers( walker::any, tile->getIJ() );
    if( wlist.empty() )
    {
      return tile->getIJ();
    }
  }

  return TilePos( -1, -1 );
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
  _fgPicturesRef().resize(1);
}

bool Fort::isNeedRoadAccess() const
{
  return false;
}
