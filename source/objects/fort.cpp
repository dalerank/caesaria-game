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
#include "gfx/helper.hpp"
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
#include "core/stacktrace.hpp"
#include "world/playerarmy.hpp"
#include "world/empire.hpp"
#include "core/variant_map.hpp"
#include "events/clearland.hpp"
#include "city/build_options.hpp"

using namespace constants;
using namespace gfx;

namespace {
Renderer::Pass _fpq[] = { Renderer::overlayAnimation };
static Renderer::PassQueue fortPassQueue( _fpq, _fpq + 2 );

struct LegionEmblem
{
  std::string name;
  Picture pic;
};

CAESARIA_LITERALCONST(name)
CAESARIA_LITERALCONST(img)

}

static LegionEmblem _findFreeEmblem( PlayerCityPtr city )
{
  FortList forts;
  forts << city->overlays();

  std::vector<LegionEmblem> availableEmblems;
  VariantMap emblemsModel = config::load( SETTINGS_RC_PATH( emblemsModel ) );
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
  std::string expeditionName;
  bool attackAnimals;
};

class FortArea::Impl
{
public:
  TilePos basePos;
};

FortArea::FortArea() : Building( objects::fortArea, Size(4) ),
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
  if( base().isValid() )
  {
    events::GameEventPtr e = events::ClearTile::create( _d->basePos );
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

FortPtr FortArea::base() const
{
  return ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) );
}

Fort::Fort(objects::Type type, int picIdLogo) : WorkingBuilding( type, Size(3) ),
  _d( new Impl )
{
  Picture logo = Picture::load(ResourceGroup::security, picIdLogo );
  logo.setOffset( Point( 80, 10 ) );

  Picture area = Picture::load(ResourceGroup::security, 13 );
  area.setOffset( Tile( TilePos(3,0) ).mappos() + Point(0,-30) );

  _fgPicturesRef().resize(2);
  _fgPicture( 0 ) = logo;
  _fgPicture( 1 ) = area;

  _d->area = new FortArea();
  _d->area->drop();
  _d->flagIndex = 21;
  _d->maxSoldier = 16;
  _d->formation = frmSquad;
  _d->attackAnimals = false;

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
  if( game::Date::isWeekChanged() )
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
void Fort::setFormation(Fort::TroopsFormation formation){  _d->formation = formation; }

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
    events::GameEventPtr e = events::ClearTile::create( _d->area->pos() );
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
  Tilemap& tmap = _city()->tilemap();
  switch( formation )
  {
  case frmOpen:
    offset = TilePos( 3, 3 );
    tiles = helper.getArea( patrolPos - offset, patrolPos + offset );
  break;

  case frmWestLine:
    tiles.push_back( &tile() );
    for( int range=1; range < 10; range++ )
    {
      tiles.push_back( &tmap.at( patrolPos - TilePos( 0, range ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( 0, range ) ) );
    }
  break;

  case frmWestDblLine:
    tiles.push_back( &tile() );
    tiles.push_back( &tmap.at( patrolPos + TilePos( 0, 1 ) ) );
    for( int range=1; range < 10; range++ )
    {
      tiles.push_back( &tmap.at( patrolPos - TilePos( 0, range ) ) );
      tiles.push_back( &tmap.at( patrolPos - TilePos( -1, range ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( 0, range ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( 1, range ) ) );
    }
  break;

  case frmNorthLine:
    tiles.push_back( &tile() );
    for( int range=1; range < 10; range++ )
    {
      tiles.push_back( &tmap.at( patrolPos - TilePos( range, 0 ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( range, 0 ) ) );
    }
  break;

  case frmNorthDblLine:
    tiles.push_back( &tile() );
    tiles.push_back( &tmap.at( patrolPos - TilePos( 1, 0 ) ) );
    for( int range=1; range < 10; range++ )
    {
      tiles.push_back( &tmap.at( patrolPos - TilePos( range,0 ) ) );
      tiles.push_back( &tmap.at( patrolPos - TilePos( range,1 ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( range,0 ) ) );
      tiles.push_back( &tmap.at( patrolPos + TilePos( range,-1 ) ) );
    }
  break;

  case frmParade:
    tiles = helper.getArea( patrolPos - TilePos( 0, 3 ), patrolPos + TilePos( 3, 0 ) );
  break;

  case frmSquad:
    offset = TilePos( 2, 2 );
    tiles = helper.getArea( patrolPos - offset, patrolPos + offset );
  break;
  }

  tiles = tiles.walkableTiles( true );
  if( !tiles.empty() )
  {
    foreach( it, tiles )
    {
      unsigned int tilehash = tile::hash((*it)->pos());

      if( _d->patrolAreaPos.find( tilehash ) == _d->patrolAreaPos.end() )
      {
        _d->patrolAreaPos[ tilehash ] = (*it)->pos();
        return (*it)->pos();
      }
    }
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

TilePos Fort::patrolLocation() const
{
  TilePos patrolPos;
  if( _d->patrolPoint.isNull()  )
  {
    Logger::warning( "!!!!WARNING: Fort::patrolLocation(): not patrol point assign in fort [%d,%d]", pos().i(), pos().j() );
    patrolPos = _d->area->pos() + TilePos( 0, 3 );
    Stacktrace::print();
  }
  else
  {
    patrolPos = _d->patrolPoint->pos();
  }

  return patrolPos;
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

  VARIANT_SAVE_ANY_D( stream, _d, maxSoldier )
  VARIANT_SAVE_ANY_D( stream, _d, attackAnimals )
  VARIANT_SAVE_ANY_D( stream, _d, lastPatrolPos )
  VARIANT_SAVE_ENUM_D( stream, _d, formation )
}

void Fort::load(const VariantMap& stream)
{
  WorkingBuilding::load( stream );

  TilePos patrolPos = stream.get( "patrolPoint", pos() + TilePos( 3, 4 ) );
  _d->patrolPoint->setPos( patrolPos );

  VARIANT_LOAD_ANYDEF_D( _d, lastPatrolPos, TilePos(-1, -1), stream )
  VARIANT_LOAD_ANY_D( _d, maxSoldier, stream )
  VARIANT_LOAD_ANY_D( _d, attackAnimals, stream )
  VARIANT_LOAD_ENUM_D( _d, formation, stream )
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

world::PlayerArmyPtr Fort::expedition() const
{
  world::PlayerArmyPtr ret;
  ret << _city()->empire()->findObject( _d->expeditionName );

  return ret;
}


void Fort::sendExpedition(Point location)
{
  world::PlayerArmyPtr army = world::PlayerArmy::create( _city()->empire(), ptr_cast<world::City>( _city() ) );
  army->setFortPos( pos() );

  RomeSoldierList soldiers;
  soldiers << walkers();

  army->move2location( location );
  army->addSoldiers( soldiers );

  army->attach();

  _d->expeditionName = army->name();

  foreach( it, soldiers )
  {
    (*it)->send2expedition( army->name() );
  }
}

void Fort::setAttackAnimals(bool value) { _d->attackAnimals = value; }
void Fort::resetExpedition() { _d->expeditionName.clear(); }

bool Fort::isAttackAnimals() const { return _d->attackAnimals; }
void Fort::_setPatrolPoint(PatrolPointPtr patrolPoint) {  _d->patrolPoint = patrolPoint; }
void Fort::_setEmblem(Picture pic) { _d->emblem.pic = pic; }
void Fort::_setName(const std::string& name) { _d->emblem.name = name; }
int  Fort::_setFlagIndex( int index ) { return _d->flagIndex = index; }

void Fort::_addFormation(Fort::TroopsFormation formation)
{
  _d->availableFormations.push_back( formation );
}

bool Fort::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool isFreeFort = Building::canBuild( areaInfo );
  CityAreaInfo fortArea = areaInfo;
  fortArea.pos += TilePos( 3, 0 );
  bool isFreeArea = _d->area->canBuild( fortArea );

  return (isFreeFort && isFreeArea);
}

bool Fort::build( const CityAreaInfo& info )
{
  FortList forts;
  forts << info.city->overlays();

  const city::development::Options& bOpts = info.city->buildOptions();
  if( forts.size() >= bOpts.maximumForts() )
  {
    _setError( "##not_enought_place_for_legion##" );
    return false;
  }

  Building::build( info );

  CityAreaInfo areaInfo = info;
  areaInfo.pos += TilePos( 3, 0 );
  _d->area->build( areaInfo );
  _d->area->setBase( this );

  _d->emblem = _findFreeEmblem( info.city );

  info.city->addOverlay( _d->area.object() );

  _fgPicturesRef().resize(1);

  BarracksList barracks;
  barracks << info.city->overlays();

  if( barracks.empty() )
  {
    _setError( "##need_barracks_for_work##" );
  }

  _setPatrolPoint( PatrolPoint::create( info.city, this,
                                        ResourceGroup::sprites, _d->flagIndex, 8,
                                        info.pos + TilePos( 3, 3 ) ) );

  return true;
}

bool Fort::isNeedRoadAccess() const {  return false; }
