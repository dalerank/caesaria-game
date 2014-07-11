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

#include "fort.hpp"
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
#include "walker/romehorseman.hpp"
#include "walker/helper.hpp"
#include "walker/romearcher.hpp"

using namespace constants;
using namespace gfx;

namespace {
Renderer::Pass _fpq[] = { Renderer::overlayAnimation, Renderer::animations };
static Renderer::PassQueue fortPassQueue( _fpq, _fpq + 2 );

struct LegionEmblem
{
  std::string name;
  Picture pic;
};

CAESARIA_LITERALCONST(lastPatrolPos)
CAESARIA_LITERALCONST(name)
CAESARIA_LITERALCONST(img)
CAESARIA_LITERALCONST(formation)

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

    newEmblem.name = vm_emblem[ lc_name ].toString();
    newEmblem.pic = Picture::load( vm_emblem[ lc_img ].toString() );
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
  int flagIndex;
  TilePos lastPatrolPos;
  std::map<unsigned int, TilePos> patrolAreaPos;
  Fort::TroopsFormations availableFormations;
  Fort::TroopsFormation formation;

public:
  TilesArray getFreeSlots(PlayerCityPtr city, const TilesArray& tiles ) const;
};

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
  _d->flagIndex = 21;
  _d->maxSoldier = 16;
  _d->formation = frmSquad;

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
  setState( Construction::destroyable, 0 );
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

bool Fort::canDestroy() const { return state( Construction::destroyable ) > 0; }
Fort::TroopsFormation Fort::formation() const {  return _d->formation; }

void Fort::setFormation(Fort::TroopsFormation formation)
{
  _d->formation = formation;
}

TilesArray Fort::enterArea() const
{
  TilesArray tiles = WorkingBuilding::enterArea();

  Tile& rtile = _city()->tilemap().at( pos() + TilePos( 1, -1 ) );
  if( rtile.isWalkable( true ) )
  {
    tiles.insert( tiles.begin(), &rtile );
  }

  return tiles;
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

TilePos Fort::freeSlot() const
{
  TilePos patrolPos;
  if( _d->patrolPoint.isNull()  )
  {
    Logger::warning( "Not patrol point assign in fort [%d,%d]", pos().i(), pos().j() );
    patrolPos = _d->area->pos() + TilePos( 0, 3 );
    _d->patrolAreaPos.clear();
  }
  else
  {
    patrolPos = _d->patrolPoint->pos();
    if( _d->lastPatrolPos != patrolPos )
    {
      _d->lastPatrolPos = patrolPos;
      _d->patrolAreaPos.clear();
    }
  }

  city::Helper helper( _city() );
  TilesArray tiles;

  TroopsFormation formation = (patrolPos == _d->area->pos() + TilePos( 0, 3 )
                                 ? frmParade
                                 : _d->formation);

  TilePos offset;
  switch( formation )
  {
  case frmRandomLocation:
    offset = TilePos( 6, 6 );
    tiles = helper.getArea( patrolPos - offset, patrolPos + offset );
  break;

  case frmSouthLine:
    offset = TilePos( 10, 0 );
    tiles = helper.getArea( patrolPos - offset, patrolPos + offset );
  break;

  case frmSouthDblLine:
    tiles = helper.getArea( patrolPos - TilePos( 6, 2 ), patrolPos + TilePos( 6, 0) );
  break;

  case frmNorthLine:
    offset = TilePos( 16, 0 );
    tiles = helper.getArea( patrolPos + TilePos( 0, 1 )- offset, patrolPos + TilePos( 0, 1 ) + offset );
  break;

  case frmNorthDblLine:
    tiles = helper.getArea( patrolPos + TilePos( -6, 1 ), patrolPos + TilePos( 6, 3) );
  break;

  case frmParade:
    tiles = helper.getArea( patrolPos - TilePos( 0, 3 ), patrolPos + TilePos( 3, 0 ) );
  break;

  case frmSquad:
    offset = TilePos( 2, 2 );
    tiles = helper.getArea( patrolPos - offset, patrolPos + offset );
  break;
  }


  tiles = _d->getFreeSlots( _city(), tiles );
  if( !tiles.empty() )
  {
    TilesArray::iterator it = tiles.begin();
    std::advance( it, math::random( tiles.size() ) );
    _d->patrolAreaPos[ TileHelper::hash( (*it)->pos() ) ] = (*it)->pos();
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
Fort::TroopsFormations Fort::legionFormations() const { return _d->availableFormations; }

unsigned int Fort::legionHealth() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  unsigned int health = 0;
  foreach( it, sldrs) { health += (*it)->health(); }
  return health / sldrs.size();
}

unsigned int Fort::legionTrained() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  unsigned int trained = 0;
  foreach( it, sldrs) { trained += (*it)->health(); }
  return trained / sldrs.size();
}

int Fort::legionMorale() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  int morale = 0;
  foreach( it, sldrs) { morale += (*it)->morale(); }
  return morale / sldrs.size();
}

void Fort::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  if( _d->patrolPoint.isValid() )
  {
    stream[ "patrolPoint" ] =  _d->patrolPoint->pos();
  }
  stream[ "soldierNumber"  ] = _d->maxSoldier;
  stream[ lc_lastPatrolPos ] = _d->lastPatrolPos;
  stream[ lc_formation     ] = (int)_d->formation;
}

void Fort::load(const VariantMap& stream)
{
  WorkingBuilding::load( stream );

  TilePos patrolPos = stream.get( "patrolPoint", pos() + TilePos( 3, 4 ) );
  _d->patrolPoint->setPos( patrolPos );
  _d->lastPatrolPos = stream.get( lc_lastPatrolPos, TilePos( -1, -1 ) );
  _d->maxSoldier = stream.get( "soldierNumber", 16 ).toUInt();
  _d->formation = (TroopsFormation)stream.get( lc_formation, 0 ).toInt();
}

SoldierList Fort::soldiers() const
{
  SoldierList soldiers;
  soldiers << walkers();

  return soldiers;
}

void Fort::returnSoldiers()
{
  if( _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->setPos( _d->area->pos() + TilePos( 0, 3 ) );
    changePatrolArea();
  }
}

void Fort::_setPatrolPoint(PatrolPointPtr patrolPoint) {  _d->patrolPoint = patrolPoint; }
void Fort::_setEmblem(Picture pic) { _d->emblem.pic = pic; }
void Fort::_setName(const std::string& name) { _d->emblem.name = name; }
int  Fort::_setFlagIndex( int index ) { return _d->flagIndex = index; }

void Fort::_addFormation(Fort::TroopsFormation formation)
{
  _d->availableFormations.push_back( formation );
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

  _d->emblem = _findFreeEmblem( city );

  city->addOverlay( _d->area.object() );

  _fgPicturesRef().resize(1);

  BarracksList barracks;
  barracks << city->overlays();

  if( barracks.empty() )
  {
    _setError( "##need_barracks_for_work##" );
  }

  _setPatrolPoint( PatrolPoint::create( city, this,
                                        ResourceGroup::sprites, _d->flagIndex, 8,
                                        pos + TilePos( 3, 3 ) ) );
}

bool Fort::isNeedRoadAccess() const {  return false; }

TilesArray Fort::Impl::getFreeSlots( PlayerCityPtr city, const TilesArray& tiles) const
{
  TilesArray ret;
  ret.reserve( tiles.size() );

  foreach ( it, tiles )
  {
    RomeSoldierList wlist;
    wlist << city->walkers( walker::any, (*it)->pos() );

    std::map<unsigned int, TilePos>::const_iterator busyIt = patrolAreaPos.find( TileHelper::hash( (*it)->pos() ) );

    if( busyIt == patrolAreaPos.end() && wlist.empty() )
    {
      ret.push_back( *it );
    }
  }

  return ret;
}
