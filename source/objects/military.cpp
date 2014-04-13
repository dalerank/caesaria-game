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
#include "game/gamedate.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include "walker/helper.hpp"

using namespace constants;
using namespace gfx;

namespace {
Renderer::Pass _fpq[] = { Renderer::building, Renderer::animations };
static Renderer::PassQueue fortPassQueue( _fpq, _fpq + 2 );

struct LegionEmblem
{
  std::string name;
  Picture pic;
};

}

static LegionEmblem _findFreeEmblem( PlayerCityPtr city )
{
  FortList forts;
  forts << city->overlays();

  std::vector<LegionEmblem> availableEmblems;
  VariantMap emblemsModel = SaveAdapter::load( GameSettings::rcpath( GameSettings::emblemsModel ) );
  foreach( it, emblemsModel )
  {
    VariantMap vm_emblem = it->second.toMap();
    LegionEmblem newEmblem;

    newEmblem.name = vm_emblem[ "name" ].toString();
    newEmblem.pic = Picture::load( vm_emblem[ "img" ].toString() );
    if( !newEmblem.name.empty() && newEmblem.pic.isValid() )
    {
      availableEmblems.push_back( newEmblem );
    }
  }

  foreach( f, forts )
  {
    foreach( it, availableEmblems )
    {
      if( (*f)->legionEmblem().name() == (*it).pic.name() )
      {
        availableEmblems.erase( it );
        break;
      }
    }
  }

  return availableEmblems.size() > 0
                       ? availableEmblems[ math::random( availableEmblems.size() ) ]
                       : LegionEmblem();
}

class Fort::Impl
{
public:
  FortAreaPtr area;
  unsigned int maxSoldier;
  PatrolPointPtr patrolPoint;
  LegionEmblem emblem;
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

  BarracksList barracks;
  barracks << city->overlays();

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
  area.setOffset( Tile( TilePos(3,0) ).mapPos() + Point(0,-30) );

  _fgPicturesRef().resize(2);
  _fgPicture( 0 ) = logo;
  _fgPicture( 1 ) = area;

  _d->area = new FortArea();
  _d->area->drop();

  _d->maxSoldier = 16;

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

float Fort::evaluateTrainee(walker::Type traineeType)
{
  int currentForce = walkers().size() * 100;
  int traineeForce = WorkingBuilding::evaluateTrainee( traineeType );
  int maxForce = _d->maxSoldier * 100;

  return ( maxForce - currentForce - traineeForce ) / 16;
}

Fort::~Fort() {}

void Fort::timeStep( const unsigned long time )
{
  if( GameDate::isWeekChanged() )
  {
    int traineeLevel = traineeValue( walker::soldier );
    // all trainees are there for the show!
    if( traineeLevel / 100 >= 1 )
    {
      if( walkers().size() < _d->maxSoldier )
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
    TilesArray tmpTiles = _city()->tilemap().getRectangle( patrolPos - offset, patrolPos + offset );
    tiles.insert( tiles.end(), tmpTiles.begin(), tmpTiles.end() );
  }

  for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); )
  {
    WalkerList wlist = _city()->getWalkers( walker::any, (*it)->pos() );
    if( !wlist.empty() ) { it = tiles.erase( it ); }
    else { ++it; }
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
  RomeSoldierList sldrs;
  sldrs << walkers();

  foreach( it, sldrs )
  {
    (*it)->send2patrol();
  }
}

Picture Fort::legionEmblem() const { return _d->emblem.pic; }
std::string Fort::legionName() const{  return _d->emblem.name; }

int Fort::legionMorale() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  SoldierList::iterator it=sldrs.begin();
  int morale = (*it)->morale(); ++it;
  for( ; it != sldrs.begin(); ++it )
  {
    morale = ( morale + (*it)->morale() ) / 2;
  }

  return morale;
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

SoldierList Fort::soldiers() const
{
  SoldierList soldiers;
  soldiers << walkers();

  return soldiers;
}

void Fort::_setPatrolPoint(PatrolPointPtr patrolPoint) {  _d->patrolPoint = patrolPoint; }
void Fort::_setEmblem(Picture pic) { _d->emblem.pic = pic; }
void Fort::_setName(const std::string& name) { _d->emblem.name = name; }

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

  _d->emblem = _findFreeEmblem( city );

  city->addOverlay( _d->area.object() );

  _fgPicturesRef().resize(1);
}

bool Fort::isNeedRoadAccess() const {  return false; }
